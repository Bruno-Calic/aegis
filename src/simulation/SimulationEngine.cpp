#include "aegis/simulation/SimulationEngine.hpp"
#include <algorithm>
#include <unordered_map>
#include <vector>

namespace aegis {

void SimulationEngine::initialize(const Scenario& scenario) {
    state_ = SimulationState{};

    state_.nodes = scenario.nodes;
    state_.edges = scenario.edges;
    state_.agents = scenario.agents;

    state_.exits.clear();
    for (const auto& node : scenario.nodes) {
        if (node.type == NodeType::Exit) {
            state_.exits.push_back(node.id);
        }
    }

    graph_ = BuildingGraph{};
    for (const auto& node : scenario.nodes) {
        graph_.addNode(node);
    }
    for (const auto& edge : scenario.edges) {
        graph_.addEdge(edge);
    }

    state_.edgeOccupancy.clear();
    for (const auto& edge : scenario.edges) {
        state_.edgeOccupancy[edge.id] = 0;
    }

    for (auto& agent : state_.agents) {
        agent.path.clear();
        agent.pathIndex = 0;
        agent.currentEdge = -1;
        agent.edgeTargetNode = -1;
        agent.remainingEdgeTime = 0.0;
        agent.waitingTime = 0.0;
        agent.evacuationTime = 0.0;
    }

    state_.running = true;
    state_.finished = false;
    state_.currentTime = 0.0;
    state_.evacuatedCount = 0;
    state_.trappedCount = 0;

    congestionModel_.update(state_.edgeOccupancy, graph_.edges());
}

void SimulationEngine::step(double deltaTime) {
    if (!state_.running || state_.finished) {
        return;
    }

    state_.currentTime += deltaTime;

    // 1) Ažuriraj gužvu prema trenutnom zauzeću
    congestionModel_.update(state_.edgeOccupancy, graph_.edges());

    // 2) Agenti koji su već na bridu napreduju
    for (auto& agent : state_.agents) {
        if (agent.state == AgentState::Evacuated ||
            agent.state == AgentState::Trapped) {
            continue;
        }

        if (agent.currentEdge == -1) {
            continue;
        }

        EdgeCongestion ec = congestionModel_.getEdgeCongestion(agent.currentEdge);

        double progressFactor = ec.speedMultiplier * agent.speed;
        if (progressFactor <= 0.0) {
            progressFactor = 0.1;
        }

        agent.remainingEdgeTime -= deltaTime * progressFactor;

        if (agent.remainingEdgeTime <= 0.0) {
            agent.currentNode = agent.edgeTargetNode;
            agent.currentEdge = -1;
            agent.edgeTargetNode = -1;
            agent.remainingEdgeTime = 0.0;
            agent.pathIndex++;

            auto occIt = state_.edgeOccupancy.find(ec.edgeId);
            if (occIt != state_.edgeOccupancy.end() && occIt->second > 0) {
                occIt->second--;
            }

            agent.state = AgentState::Initial;
        } else {
            agent.state = AgentState::Moving;
        }
    }

    // 3) Ponovno ažuriraj gužvu nakon izlazaka s bridova
    congestionModel_.update(state_.edgeOccupancy, graph_.edges());

    // Privremene rezervacije za ovaj korak
    std::unordered_map<int, int> plannedOccupancy;
    for (const auto& edge : state_.edges) {
        plannedOccupancy[edge.id] = 0;
    }

    // Parametar koliko jako agenti izbjegavaju gužvu
    constexpr double congestionWeight = 2.0;

    // Funkcija dinamičke cijene brida:
    // uključuje trenutnu gužvu + planirane rezervacije
    auto dynamicEdgeCost =
        [&](const Edge& edge) -> double {
            EdgeCongestion congestion =
                congestionModel_.getEdgeCongestion(edge.id);

            int effectiveCap = congestionModel_.effectiveCapacity(edge);
            if (effectiveCap <= 0) {
                effectiveCap = 1;
            }

            int planned = 0;
            auto it = plannedOccupancy.find(edge.id);
            if (it != plannedOccupancy.end()) {
                planned = it->second;
            }

            int totalLoad = congestion.occupancy + planned;
            double density = static_cast<double>(totalLoad) / effectiveCap;
            if (density < 0.0) density = 0.0;
            if (density > 1.0) density = 1.0;

            double multiplier = 1.0 - density;
            if (multiplier < 0.1) {
                multiplier = 0.1;
            }

            // Osnovno vrijeme + kazna za gužvu
            double base = edge.traversalTime;
            double congestionPenalty = base * congestionWeight * density;

            return base + congestionPenalty;
        };

    struct MoveIntent {
        std::size_t agentIndex;
        int edgeId;
        int nextNode;
        double traversalTime;
    };

    std::vector<MoveIntent> intents;
    std::vector<bool> shouldEvacuate(state_.agents.size(), false);
    std::vector<bool> shouldTrap(state_.agents.size(), false);

    // 4) Svi agenti na čvorovima planiraju iz istog stanja
    for (std::size_t i = 0; i < state_.agents.size(); ++i) {
        auto& agent = state_.agents[i];

        if (agent.state == AgentState::Evacuated ||
            agent.state == AgentState::Trapped) {
            continue;
        }

        if (agent.currentEdge != -1) {
            continue;
        }

        if (agent.currentNode == -1) {
            continue;
        }

        bool isOnExit = false;
        for (int exitId : state_.exits) {
            if (agent.currentNode == exitId) {
                isOnExit = true;
                break;
            }
        }

        if (isOnExit) {
            shouldEvacuate[i] = true;
            continue;
        }

        PathResult bestPath;
        for (int exitId : state_.exits) {
            PathResult path = pathFinder_.dijkstra(
                graph_,
                agent.currentNode,
                exitId,
                dynamicEdgeCost
            );

            if (path.found) {
                if (!bestPath.found || path.totalCost < bestPath.totalCost) {
                    bestPath = path;
                }
            }
        }

        if (!bestPath.found) {
            shouldTrap[i] = true;
            continue;
        }

        if (agent.path.empty() || agent.path != bestPath.nodePath) {
            agent.path = bestPath.nodePath;
            agent.pathIndex = 0;
        }

        if (agent.pathIndex >= agent.path.size() - 1) {
            agent.state = AgentState::Waiting;
            continue;
        }

        int nextNode = agent.path[agent.pathIndex + 1];

        Edge* nextEdge = nullptr;
        for (auto& edge : state_.edges) {
            if ((edge.from == agent.currentNode && edge.to == nextNode) ||
                (!edge.directed && edge.from == nextNode && edge.to == agent.currentNode)) {
                nextEdge = &edge;
                break;
            }
        }

        if (!nextEdge || nextEdge->blocked) {
            agent.state = AgentState::Waiting;
            agent.waitingTime += deltaTime;
            continue;
        }

        intents.push_back(MoveIntent{
            i,
            nextEdge->id,
            nextNode,
            nextEdge->traversalTime
        });

        // Rezerviraj mjesto na cijeloj odabranoj ruti
        for (std::size_t k = agent.pathIndex + 1; k < agent.path.size(); ++k) {
            int fromNode = agent.path[k - 1];
            int toNode = agent.path[k];

            for (const auto& edge : state_.edges) {
                if ((edge.from == fromNode && edge.to == toNode) ||
                    (!edge.directed && edge.from == toNode && edge.to == fromNode)) {
                    plannedOccupancy[edge.id]++;
                    break;
                }
            }
        }
    }

    // 5) Grupiraj zahtjeve po bridu
    std::unordered_map<int, std::vector<MoveIntent>> intentsByEdge;
    for (const auto& intent : intents) {
        intentsByEdge[intent.edgeId].push_back(intent);
    }

    // 6) Agenti na čvorovima privremeno čekaju dok se ne riješe ulazi
    for (auto& agent : state_.agents) {
        if (agent.state != AgentState::Evacuated &&
            agent.state != AgentState::Trapped &&
            agent.currentEdge == -1) {
            agent.state = AgentState::Waiting;
        }
    }

    // 7) Sinkrono odobri ulazak na bridove
    for (auto& [edgeId, requests] : intentsByEdge) {
        Edge* edgePtr = nullptr;
        for (auto& edge : state_.edges) {
            if (edge.id == edgeId) {
                edgePtr = &edge;
                break;
            }
        }

        if (!edgePtr) {
            continue;
        }

        int occupancy = state_.edgeOccupancy[edgeId];
        int totalCapacity = congestionModel_.effectiveCapacity(*edgePtr);
        int available = std::max(0, totalCapacity - occupancy);

        std::sort(requests.begin(), requests.end(),
                  [&](const MoveIntent& a, const MoveIntent& b) {
                      return state_.agents[a.agentIndex].id < state_.agents[b.agentIndex].id;
                  });

        int accepted = std::min<int>(available, requests.size());

        for (int k = 0; k < accepted; ++k) {
            const auto& intent = requests[k];
            auto& agent = state_.agents[intent.agentIndex];

            agent.currentEdge = intent.edgeId;
            agent.edgeTargetNode = intent.nextNode;
            agent.remainingEdgeTime = intent.traversalTime;
            agent.state = AgentState::Moving;

            state_.edgeOccupancy[edgeId]++;
        }

        for (std::size_t k = accepted; k < requests.size(); ++k) {
            auto& agent = state_.agents[requests[k].agentIndex];
            agent.state = AgentState::Waiting;
            agent.waitingTime += deltaTime;
        }
    }

    // 8) Primijeni evakuacije i trapped statuse
    for (std::size_t i = 0; i < state_.agents.size(); ++i) {
        auto& agent = state_.agents[i];

        if (shouldEvacuate[i]) {
            if (agent.state != AgentState::Evacuated) {
                agent.state = AgentState::Evacuated;
                agent.evacuationTime = state_.currentTime;
                state_.evacuatedCount++;
            }
        }

        if (shouldTrap[i]) {
            if (agent.state != AgentState::Trapped) {
                agent.state = AgentState::Trapped;
                state_.trappedCount++;
            }
        }
    }

    // 9) Završno ažuriranje congestiona za GUI
    congestionModel_.update(state_.edgeOccupancy, graph_.edges());

    int activeCount = 0;
    for (const auto& agent : state_.agents) {
        if (agent.state != AgentState::Evacuated &&
            agent.state != AgentState::Trapped) {
            activeCount++;
        }
    }

    if (activeCount == 0) {
        state_.finished = true;
        state_.running = false;
    }
}

void SimulationEngine::runUntilFinished() {
    while (!state_.finished) {
        step(0.25);
    }
}

const SimulationState& SimulationEngine::state() const {
    return state_;
}

bool SimulationEngine::isFinished() const {
    return state_.finished;
}

EdgeCongestion SimulationEngine::getEdgeCongestion(int edgeId) const {
    return congestionModel_.getEdgeCongestion(edgeId);
}

} // namespace aegis
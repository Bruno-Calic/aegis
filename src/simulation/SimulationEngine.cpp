#include "aegis/simulation/SimulationEngine.hpp"
#include <algorithm>
#include <iostream>

namespace aegis {

void SimulationEngine::initialize(const Scenario& scenario) {
    state_ = SimulationState{};
    
    // Kopiraj sve iz scenarija
    state_.nodes = scenario.nodes;
    state_.edges = scenario.edges;
    state_.agents = scenario.agents;

    // Pronađi izlaze
    state_.exits.clear();
    for (const auto& node : scenario.nodes) {
        if (node.type == NodeType::Exit) {
            state_.exits.push_back(node.id);
        }
    }

    // Izgradi graf
    graph_ = BuildingGraph{};
    for (const auto& node : scenario.nodes) {
        graph_.addNode(node);
    }
    for (const auto& edge : scenario.edges) {
        graph_.addEdge(edge);
    }

    // Inicijaliziraj zauzeće bridova na 0
    state_.edgeOccupancy.clear();
    for (const auto& edge : scenario.edges) {
        state_.edgeOccupancy[edge.id] = 0;
    }

    // Postavi stanje
    state_.running = true;
    state_.finished = false;
    state_.currentTime = 0.0;
    state_.evacuatedCount = 0;
    state_.trappedCount = 0;
}

void SimulationEngine::step(double deltaTime) {
    if (!state_.running || state_.finished) {
        return;
    }

    state_.currentTime += deltaTime;

    // A) Agenti u tranziciji preko brida: odmotaj vrijeme prelaska
    for (auto& agent : state_.agents) {
        if (agent.state == AgentState::Evacuated ||
            agent.state == AgentState::Trapped) {
            continue;
        }
        if (agent.currentEdge == -1) {
            continue; // nije na bridu
        }

        // Gušći brid = manji speedMultiplier = sporije odmotavanje vremena
        double mult = congestionModel_
            .getEdgeCongestion(agent.currentEdge).speedMultiplier;
        agent.remainingTime -= deltaTime * mult;

        if (agent.remainingTime <= 0.0) {
            // Agent je stigao: oslobodi kapacitet, prebaci ga na sljedeći čvor
            auto& occ = state_.edgeOccupancy[agent.currentEdge];
            if (occ > 0) occ--;

            agent.currentNode = agent.path[agent.pathIndex + 1];
            agent.pathIndex++;
            agent.currentEdge = -1;
            agent.remainingTime = 0.0;
        }
    }

    // B) Agenti na čvoru: provjera izlaza, routing, ulazak na brid
    for (auto& agent : state_.agents) {
        if (agent.state == AgentState::Evacuated ||
            agent.state == AgentState::Trapped) {
            continue;
        }
        if (agent.currentEdge != -1) {
            continue; // jos je na bridu, ne diraj ga
        }

        bool isOnExit = false;
        for (int exitId : state_.exits) {
            if (agent.currentNode == exitId) { isOnExit = true; break; }
        }
        if (isOnExit) {
            agent.state = AgentState::Evacuated;
            agent.evacuationTime = state_.currentTime;
            state_.evacuatedCount++;
            continue;
        }

        PathResult bestPath;
        for (int exitId : state_.exits) {
            PathResult path = pathFinder_.dijkstra(graph_, agent.currentNode, exitId);
            if (path.found && (!bestPath.found || path.totalCost < bestPath.totalCost)) {
                bestPath = path;
            }
        }
        if (!bestPath.found) {
            agent.state = AgentState::Trapped;
            state_.trappedCount++;
            continue;
        }

        if (agent.path.empty() || agent.path != bestPath.nodePath) {
            agent.path = bestPath.nodePath;
            agent.pathIndex = 0;
        }

        if (agent.pathIndex < agent.path.size() - 1) {
            int nextNode = agent.path[agent.pathIndex + 1];

            Edge* nextEdge = nullptr;
            for (auto& edge : state_.edges) {
                if ((edge.from == agent.currentNode && edge.to == nextNode) ||
                    (!edge.directed && edge.from == nextNode && edge.to == agent.currentNode)) {
                    nextEdge = &edge;
                    break;
                }
            }

            if (nextEdge) {
                int occupancy = state_.edgeOccupancy[nextEdge->id];
                if (!nextEdge->blocked && occupancy < nextEdge->capacity) {
                    // NE pomiči currentNode ovdje — agent je sada NA bridu
                    agent.currentEdge = nextEdge->id;
                    agent.remainingTime = nextEdge->traversalTime;
                    state_.edgeOccupancy[nextEdge->id] = occupancy + 1;
                    agent.state = AgentState::Moving;
                } else {
                    agent.state = AgentState::Waiting;
                    agent.waitingTime += deltaTime;
                }
            }
        }
    }

    // C) UKLONI staru petlju koja je sve dekrementirala!

    // D) update() na KRAJU koraka — ispis onda vidi stvarno trenutno stanje
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

}
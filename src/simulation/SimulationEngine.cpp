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

    // Ažuriraj gužvu prije obrade agenata
    congestionModel_.update(state_.edgeOccupancy, graph_.edges());

    for (auto& agent : state_.agents) {
        // Preskoči već evakuirane ili zarobljene agente
        if (agent.state == AgentState::Evacuated || 
            agent.state == AgentState::Trapped) {
            continue;
        }

        if (agent.currentNode == -1) {
            continue;
        }

        // Provjeri je li agent već na izlazu
        bool isOnExit = false;
        for (int exitId : state_.exits) {
            if (agent.currentNode == exitId) {
                isOnExit = true;
                break;
            }
        }

        if (isOnExit) {
            agent.state = AgentState::Evacuated;
            state_.evacuatedCount++;
            continue;
        }

        // Pronađi najkraći put do bilo kojeg izlaza
        PathResult bestPath;
        for (int exitId : state_.exits) {
            PathResult path = pathFinder_.dijkstra(graph_, agent.currentNode, exitId);
            if (path.found) {
                if (!bestPath.found || path.totalCost < bestPath.totalCost) {
                    bestPath = path;
                }
            }
        }

        // Ako nema puta, agent je zarobljen
        if (!bestPath.found) {
            agent.state = AgentState::Trapped;
            state_.trappedCount++;
            continue;
        }

        // Ažuriraj put ako je novi ili prazan
        if (agent.path.empty() || agent.path != bestPath.nodePath) {
            agent.path = bestPath.nodePath;
            agent.pathIndex = 0;
        }

        // Provjeri kapacitet sljedećeg brida
        if (agent.pathIndex < bestPath.nodePath.size() - 1) {
            int nextNode = bestPath.nodePath[agent.pathIndex + 1];
            
            // Pronađi brid između trenutnog i sljedećeg čvora
            Edge* nextEdge = nullptr;
            for (auto& edge : state_.edges) {
                if ((edge.from == agent.currentNode && edge.to == nextNode) ||
                    (!edge.directed && edge.from == nextNode && edge.to == agent.currentNode)) {
                    nextEdge = &edge;
                    break;
                }
            }
            
            if (nextEdge) {
                // Provjeri kapacitet
                int occupancy = state_.edgeOccupancy[nextEdge->id];
                
                if (occupancy < nextEdge->capacity) {
                    // Prolaz je slobodan, pomakni agenta
                    agent.pathIndex++;
                    agent.currentNode = nextNode;
                    
                    // Povećaj zauzeće na novom bridu
                    state_.edgeOccupancy[nextEdge->id] = occupancy + 1;
                    
                    agent.state = AgentState::Moving;
                } else {
                    // Prolaz je pun, agent čeka
                    agent.state = AgentState::Waiting;
                    agent.waitingTime += deltaTime;
                }
            }
        }
    }

    // Smanji zauzeće na svim bridovima na kraju koraka
    // (agenti koji su prošli kroz brid u ovom koraku napuštaju ga)
    for (auto& [edgeId, occupancy] : state_.edgeOccupancy) {
        if (occupancy > 0) {
            occupancy--;
        }
    }

    // Provjeri je li gotovo
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
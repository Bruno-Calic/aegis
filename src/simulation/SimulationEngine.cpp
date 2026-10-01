#include "aegis/simulation/SimulationEngine.hpp"

namespace aegis {

void SimulationEngine::initialize(const Scenario& scenario) {
    state_ = SimulationState{};
    state_.nodes = scenario.nodes;
    state_.edges = scenario.edges;

    for (const auto& node : scenario.nodes) {
        if (node.type == NodeType::Exit) {
            state_.exits.push_back(node.id);
        }
    }

    for (const auto& node : scenario.nodes) {
        graph_.addNode(node);
    }
    for (const auto& edge : scenario.edges) {
        graph_.addEdge(edge);
    }

    state_.running = true;
    state_.finished = false;
}

void SimulationEngine::step(double deltaTime) {
    if (!state_.running || state_.finished) {
        return;
    }

    state_.currentTime += deltaTime;

    for (auto& agent : state_.agents) {
        if (agent.currentNode == -1) {
            continue;
        }

        PathResult path = pathFinder_.bestPathToAnyExit(
            graph_,
            agent.currentNode,
            state_.exits
        );

        if (!path.found) {
            agent.state = AgentState::Trapped;
            state_.trappedCount++;
            continue;
        }

        if (path.nodePath.size() <= 1) {
            agent.state = AgentState::Evacuated;
            state_.evacuatedCount++;
            continue;
        }

        int nextNode = path.nodePath[1];
        agent.currentNode = nextNode;
        agent.path = path.nodePath;
    }

    if (state_.evacuatedCount + state_.trappedCount == static_cast<int>(state_.agents.size())) {
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

}
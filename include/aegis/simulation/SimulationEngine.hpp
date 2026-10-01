#pragma once

#include "../Scenario.hpp"
#include "../graph/BuildingGraph.hpp"
#include "../pathfinding/PathFinder.hpp"

namespace aegis {

struct SimulationState {
    double currentTime = 0.0;
    bool running = false;
    bool finished = false;

    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<Agent> agents;
    std::vector<int> exits;

    int evacuatedCount = 0;
    int trappedCount = 0;
};

class SimulationEngine {
public:
    void initialize(const Scenario& scenario);
    void step(double deltaTime);
    void runUntilFinished();

    const SimulationState& state() const;
    bool isFinished() const;

private:
    SimulationState state_;
    BuildingGraph graph_;
    PathFinder pathFinder_;
};

}
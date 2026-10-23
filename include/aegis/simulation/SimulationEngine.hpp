#pragma once

#include "aegis/Scenario.hpp"
#include "aegis/graph/BuildingGraph.hpp"
#include "aegis/pathfinding/PathFinder.hpp"
#include "aegis/congestion/CongestionModel.hpp"
#include <unordered_map>

namespace aegis {

struct SimulationState {
    double currentTime = 0.0;
    bool running = false;
    bool finished = false;

    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<Agent> agents;
    std::vector<int> exits;

    //Praćenje zauzeća bridova
    std::unordered_map<int, int> edgeOccupancy;

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

    //Dohvati gužvu za brid
    EdgeCongestion getEdgeCongestion(int edgeId) const;

private:
    SimulationState state_;
    BuildingGraph graph_;
    PathFinder pathFinder_;
    CongestionModel congestionModel_;
};

}
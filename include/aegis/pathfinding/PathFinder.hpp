#pragma once

#include "aegis/graph/BuildingGraph.hpp"
#include <vector>
#include <optional>

namespace aegis {

struct PathResult {
    bool found = false;
    std::vector<int> nodePath;
    double totalCost = 0.0; 
};

class PathFinder {
public:
    PathResult dijkstra(
        const BuildingGraph& graph,
        int startNode,
        int targetNode
    ) const;

    PathResult bestPathToAnyExit(
        const BuildingGraph& graph,
        int startNode,
        const std::vector<int>& exits
    ) const;
};

}
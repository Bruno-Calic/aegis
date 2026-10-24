#pragma once

#include "aegis/graph/BuildingGraph.hpp"
#include <functional>
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
    // Funkcija koja za svaki brid vraća dinamičku cijenu prelaska.
    // Ako nije zadana, koristi se edge.traversalTime.
    using EdgeCostFunction =
        std::function<double(const Edge&)>;

    PathResult dijkstra(
        const BuildingGraph& graph,
        int startNode,
        int targetNode,
        EdgeCostFunction edgeCost = nullptr
    ) const;

    PathResult bestPathToAnyExit(
        const BuildingGraph& graph,
        int startNode,
        const std::vector<int>& exits,
        EdgeCostFunction edgeCost = nullptr
    ) const;
};

}
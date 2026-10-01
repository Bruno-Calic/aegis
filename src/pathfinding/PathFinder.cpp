#include "../../include/aegis/pathfinding/PathFinder.hpp"
#include <queue>
#include <limits>
#include <unordered_map>
#include <algorithm>

namespace aegis {

PathResult PathFinder::dijkstra(
    const BuildingGraph& graph,
    int startNode,
    int targetNode
) const {
    PathResult result;

    if (!graph.hasNode(startNode) || !graph.hasNode(targetNode)) {
        return result;
    }

    using Dist = double;
    const Dist INF = std::numeric_limits<Dist>::infinity();

    std::unordered_map<int, Dist> dist;
    std::unordered_map<int, int> previous; 

    dist[startNode] = 0.0;

    using PQEntry = std::pair<Dist, int>; // (distance, node)
    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<>> pq; // Min-heap priority queue
    pq.emplace(0.0, startNode);

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) {
            continue;
        }

        if (u == targetNode) {
            break;
        }

        for (const auto& edge : graph.outgoingEdges(u)) {
            if (edge.blocked) {
                continue;
            }

            int v = (edge.from == u) ? edge.to : edge.from;
            Dist newDist = dist[u] + edge.traversalTime;

            if (newDist < dist[v]) {
                dist[v] = newDist;
                previous[v] = u;
                pq.emplace(newDist, v);
            }
        }
    }

    if (dist.find(targetNode) == dist.end()) {
        return result;
    }

    result.found = true;
    result.totalCost = dist[targetNode];

    int current = targetNode;
    while (current != startNode) {
        result.nodePath.push_back(current);
        current = previous[current];
    }
    result.nodePath.push_back(startNode);
    std::reverse(result.nodePath.begin(), result.nodePath.end());

    return result;
}

PathResult PathFinder::bestPathToAnyExit(
    const BuildingGraph& graph,
    int startNode,
    const std::vector<int>& exits
) const {
    PathResult best;

    for (int exitNode : exits) {
        PathResult r = dijkstra(graph, startNode, exitNode);
        if (r.found && (!best.found || r.totalCost < best.totalCost)) {
            best = r;
        }
    }

    return best;
}

}
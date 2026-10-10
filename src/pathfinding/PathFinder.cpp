#include "aegis/pathfinding/PathFinder.hpp"
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

    using PQEntry = std::pair<Dist, int>;
    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<>> pq;
    pq.emplace(0.0, startNode);

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // POPRAVAK: dist[u] može biti default (0.0) ako čvor nije bio u mapi,
        // pa koristimo find() umjesto operator[].
        auto du = dist.find(u);
        if (du == dist.end() || d > du->second) {
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
            Dist newDist = du->second + edge.traversalTime;  // at() umjesto []

            // POPRAVAK: dist[v] preko operator[] default-konstruira na 0.0, ne INF.
            auto it = dist.find(v);
            if (it == dist.end() || newDist < it->second) {
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
    result.totalCost = dist.at(targetNode);

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
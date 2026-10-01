#pragma once

#include "../Node.hpp"
#include "../Edge.hpp"
#include <unordered_map>
#include <vector>

namespace aegis {

class BuildingGraph {
public:
    void addNode(const Node& node);
    void addEdge(const Edge& edge);

    const Node& node(int id) const;
    const Edge& edge(int id) const;

    std::vector<int> neighbors(int nodeId) const;
    std::vector<Edge> outgoingEdges(int nodeId) const;

    bool hasNode(int id) const;
    bool hasEdge(int id) const;

private:
    std::unordered_map<int, Node> nodes_;
    std::unordered_map<int, Edge> edges_;
    std::unordered_map<int, std::vector<int>> adjacency_;
    // [a, b, c, d] 
};

}
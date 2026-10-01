#include "../../include/aegis/graph/BuildingGraph.hpp"
#include <stdexcept>

namespace aegis {

void BuildingGraph::addNode(const Node& node) {
    nodes_[node.id] = node;
}

void BuildingGraph::addEdge(const Edge& edge) {
    edges_[edge.id] = edge;
    adjacency_[edge.from].push_back(edge.to);
    if (!edge.directed) {
        adjacency_[edge.to].push_back(edge.from);
    }
}

// Retrieves a node by its ID. Throws an exception if the node does not exist.
const Node& BuildingGraph::node(int id) const {
    auto it = nodes_.find(id);
    if (it == nodes_.end()) {
        throw std::runtime_error("Node not found: " + std::to_string(id));
    }
    return it->second;
}

const Edge& BuildingGraph::edge(int id) const {
    auto it = edges_.find(id);
    if (it == edges_.end()) {
        throw std::runtime_error("Edge not found: " + std::to_string(id));
    }
    return it->second;
}

std::vector<int> BuildingGraph::neighbors(int nodeId) const {
    auto it = adjacency_.find(nodeId);
    if (it == adjacency_.end()) {
        return {};
    }
    return it->second;
}

std::vector<Edge> BuildingGraph::outgoingEdges(int nodeId) const {
    std::vector<Edge> result;
    for (const auto& edge : edges_) {
        if (edge.second.from == nodeId || (!edge.second.directed && edge.second.to == nodeId)) {
            result.push_back(edge.second);
        }
    }
    return result;
}

bool BuildingGraph::hasNode(int id) const {
    return nodes_.count(id) > 0;
}

bool BuildingGraph::hasEdge(int id) const {
    return edges_.count(id) > 0;
}

}
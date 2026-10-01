#pragma once
#include <string>

namespace aegis {

enum class NodeType {
    Room,
    Corridor,
    Staircase,
    Exit
};

// Node represents a point in the graph, such as a room or corridor
struct Node {
    int id = -1;
    NodeType type = NodeType::Room;
    std::string name;
    double x = 0.0;
    double y = 0.0;
};

}
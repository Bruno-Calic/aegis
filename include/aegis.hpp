#pragma once

#include <string>
#include <vector>

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

// Edge represents a connection between two nodes in the graph
struct Edge {
    int id = -1; // Unique identifier for the edge
    int from = -1; // ID of the starting node
    int to = -1; // ID of the ending node

    double length = 1.0; // Length of the edge in meters
    double width = 1.0; // Width of the edge in meters
    double traversalTime = 1.0; // Time to traverse the edge in seconds m/s

    int capacity = 1; // Maximum number of people that can traverse the edge at once
    bool blocked = false; // Indicates if the edge is blocked or inaccessible
    bool directed = false; // Indicates if the edge is directed (one-way) or undirected (two-way) (example: stairs are directed, corridors are undirected)
};

enum class AgentState {
    Initial,
    Waiting,
    Moving,
    Blocked,
    Evacuated,
    Trapped
};

struct Agent {
    int id = -1;
    int currentNode = -1;
    int targetExit = -1;

    double speed = 1.0;
    double reactionDelay = 0.0;
    double waitingTime = 0.0;
    double evacuationTime = 0.0;

    AgentState state = AgentState::Initial;

    std::vector<int> path;
    std::size_t pathIndex = 0;
};

}
#pragma once
#include <vector>

namespace aegis {

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

}
#pragma once

#include "aegis/Edge.hpp"
#include <unordered_map>

namespace aegis {

struct EdgeCongestion {
    int edgeId = -1;
    int occupancy = 0;
    int capacity = 1;
    double density = 0.0;
    double speedMultiplier = 1.0;
};

class CongestionModel {
public:
    double density(int occupancy, int capacity) const;
    double speedMultiplier(double density) const;
    double congestionPenalty(const Edge& edge, int occupancy) const;
    
    void update(
        const std::unordered_map<int, int>& edgeOccupancy,
        const std::unordered_map<int, Edge>& edges
    );
    
    EdgeCongestion getEdgeCongestion(int edgeId) const;

private:
    std::unordered_map<int, EdgeCongestion> congestion_;
};

}
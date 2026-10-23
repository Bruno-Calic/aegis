#pragma once

#include "aegis/Edge.hpp"
#include <unordered_map>

namespace aegis {

struct EdgeCongestion {
    int edgeId = -1;
    int occupancy = 0;
    int capacity = 1;          // efektivni ukupni kapacitet cijelog brida
    double density = 0.0;
    double speedMultiplier = 1.0;
};

class CongestionModel {
public:
    double density(int occupancy, int capacity) const;
    double speedMultiplier(double density) const;
    double congestionPenalty(const Edge& edge, int occupancy) const;

    // Novo: capacity više nije "ukupno ljudi na edgeu" nego "ljudi po redu".
    // Ovdje računamo stvarni ukupni kapacitet cijelog hodnika.
    int effectiveCapacity(const Edge& edge) const;

    void update(
        const std::unordered_map<int, int>& edgeOccupancy,
        const std::unordered_map<int, Edge>& edges
    );

    EdgeCongestion getEdgeCongestion(int edgeId) const;

private:
    std::unordered_map<int, EdgeCongestion> congestion_;
};

}
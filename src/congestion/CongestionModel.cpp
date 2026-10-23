#include "aegis/congestion/CongestionModel.hpp"
#include <algorithm>

namespace aegis {

double CongestionModel::density(int occupancy, int capacity) const {
    if (capacity <= 0) {
        return 1.0;
    }
    
    double d = static_cast<double>(occupancy) / capacity;
    return std::min(1.0, std::max(0.0, d));
}

double CongestionModel::speedMultiplier(double density) const {
    double multiplier = 1.0 - density;
    
    if (multiplier < 0.1) {
        multiplier = 0.1;
    }
    
    return multiplier;
}

double CongestionModel::congestionPenalty(
    const Edge& edge,
    int occupancy
) const {
    double d = density(occupancy, edge.capacity);
    return d * edge.traversalTime;
}

void CongestionModel::update(
    const std::unordered_map<int, int>& edgeOccupancy,
    const std::unordered_map<int, Edge>& edges
) {
    congestion_.clear();
    
    for (const auto& [edgeId, edge] : edges) {
        EdgeCongestion ec;
        ec.edgeId = edgeId;
        ec.capacity = edge.capacity;
        
        auto it = edgeOccupancy.find(edgeId);
        if (it != edgeOccupancy.end()) {
            ec.occupancy = it->second;
        }
        
        ec.density = density(ec.occupancy, ec.capacity);
        ec.speedMultiplier = speedMultiplier(ec.density);
        
        congestion_[edgeId] = ec;
    }
}

EdgeCongestion CongestionModel::getEdgeCongestion(int edgeId) const {
    auto it = congestion_.find(edgeId);
    if (it != congestion_.end()) {
        return it->second;
    }
    
    EdgeCongestion empty;
    return empty;
}

}
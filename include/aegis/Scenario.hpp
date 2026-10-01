#pragma once

#include "aegis/Node.hpp"
#include "aegis/Edge.hpp"
#include "aegis/Agent.hpp"
#include <vector>
#include <string>

namespace aegis {

struct Scenario {
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<Agent> agents;
    std::string name;
};

}
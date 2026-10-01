#pragma once

#include <vector>

namespace aegis {

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
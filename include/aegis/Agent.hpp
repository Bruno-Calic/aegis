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
    int currentEdge = -1;       // brid na kojem agent trenutno jeste (-1 = na čvoru)
    double remainingTime = 0.0; // koliko mu još treba da prijeđe brid

    double speed = 1.0;
    double reactionDelay = 0.0;
    double waitingTime = 0.0;
    double evacuationTime = 0.0;

    AgentState state = AgentState::Initial;

    std::vector<int> path;
    std::size_t pathIndex = 0;
};

}
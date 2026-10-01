#add_library(programs programs.cpp)
add_library(core 
            core/Node.cpp 
            core/Edge.cpp 
            graph/BuildingGraph.cpp
            pathfinding/PathFinder.cpp
            simulation/SimulationEngine.cpp)


target_include_directories(core PRIVATE ../include)
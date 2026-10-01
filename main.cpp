#include "aegis/Scenario.hpp"
#include "aegis/simulation/SimulationEngine.hpp"
#include <iostream>

int main() {
    using namespace aegis;

    Scenario scenario;
    scenario.name = "SimpleSchool";

    Node n1{1, NodeType::Room, "Ucionica1", 0, 0};
    Node n2{2, NodeType::Corridor, "Hodnik", 1, 0};
    Node n3{3, NodeType::Exit, "Izlaz", 2, 0};

    scenario.nodes = {n1, n2, n3};

    Edge e1{10, 1, 2, 4.0, 1.2, 4.0, 3, false, false};
    Edge e2{11, 2, 3, 5.0, 1.0, 5.0, 2, false, false};

    scenario.edges = {e1, e2};

    Agent a1;
    a1.id = 1;
    a1.currentNode = 1;
    a1.state = AgentState::Initial;

    scenario.agents.push_back(a1);

    SimulationEngine engine;
    engine.initialize(scenario);
    engine.runUntilFinished();

    const auto& s = engine.state();
    std::cout << "Evacuated: " << s.evacuatedCount << "\n";
    std::cout << "Trapped: " << s.trappedCount << "\n";
    std::cout << "Time: " << s.currentTime << " s\n";

    return 0;
}
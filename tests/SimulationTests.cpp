#include <gtest/gtest.h>
#include "aegis/Scenario.hpp"
#include "aegis/simulation/SimulationEngine.hpp"

using namespace aegis;

TEST(SimulationTests, OneAgentEvacuates) {
    Scenario scenario;

    Node n1{1, NodeType::Room, "R1", 0, 0};
    Node n2{2, NodeType::Exit, "E1", 1, 0};

    scenario.nodes = {n1, n2};

    Edge e1{10, 1, 2, 5.0, 1.0, 5.0, 1, false, false};
    scenario.edges = {e1};

    Agent a1;
    a1.id = 1;
    a1.currentNode = 1;
    a1.state = AgentState::Initial;

    scenario.agents.push_back(a1);

    SimulationEngine engine;
    engine.initialize(scenario);
    engine.runUntilFinished();

    const auto& s = engine.state();
    EXPECT_EQ(s.evacuatedCount, 1);
    EXPECT_EQ(s.trappedCount, 0);
    EXPECT_TRUE(s.finished);
}
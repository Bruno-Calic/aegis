#include <gtest/gtest.h>
#include "../include/aegis/graph/BuildingGraph.hpp"
#include "../include/aegis/pathfinding/PathFinder.hpp"

using namespace aegis;

TEST(PathFinderTests, SimplePathExists) {
    BuildingGraph graph;

    Node n1{1, NodeType::Room, "R1", 0, 0};
    Node n2{2, NodeType::Corridor, "C1", 1, 0};
    Node n3{3, NodeType::Exit, "E1", 2, 0};

    graph.addNode(n1);
    graph.addNode(n2);
    graph.addNode(n3);

    Edge e1{10, 1, 2, 4.0, 1.0, 4.0, 1, false, false};
    Edge e2{11, 2, 3, 5.0, 1.0, 5.0, 1, false, false};

    graph.addEdge(e1);
    graph.addEdge(e2);

    PathFinder pf;
    PathResult r = pf.dijkstra(graph, 1, 3);

    EXPECT_TRUE(r.found);
    EXPECT_DOUBLE_EQ(r.totalCost, 9.0);
    EXPECT_EQ(r.nodePath.size(), 3);
    EXPECT_EQ(r.nodePath[0], 1);
    EXPECT_EQ(r.nodePath[2], 3);
}

TEST(PathFinderTests, NoPathWhenBlocked) {
    BuildingGraph graph;

    Node n1{1, NodeType::Room, "R1", 0, 0};
    Node n2{2, NodeType::Exit, "E1", 1, 0};

    graph.addNode(n1);
    graph.addNode(n2);

    Edge e1{10, 1, 2, 5.0, 1.0, 5.0, 1, true, false};
    e1.blocked = true;

    graph.addEdge(e1);

    PathFinder pf;
    PathResult r = pf.dijkstra(graph, 1, 2);

    EXPECT_FALSE(r.found);
}
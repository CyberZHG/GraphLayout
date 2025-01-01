#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "graph_layout.h"
using namespace std;
using namespace graph_layout;

constexpr double UNIT_SIZE = VertexPositioning::DEFAULT_VERTEX_SIZE + VertexPositioning::DEFAULT_VERTEX_MARGIN;

TEST(TestDirectedGraphHierarchialLayout, EmptyGraph) {
    SPDirectedGraph graph(0);
    const DirectedGraphHierarchicalLayout layout;
    const auto [xs, ys] = layout.layoutGraph(graph);
    EXPECT_EQ(xs, vector<double>({}));
    EXPECT_EQ(ys, vector<double>({}));
}

TEST(TestDirectedGraphHierarchialLayout, SingleVertexSelfCycle) {
    SPDirectedGraph graph(1);
    graph.addEdge(0, 0);
    const DirectedGraphHierarchicalLayout layout;
    const auto [xs, ys] = layout.layoutGraph(graph);
    EXPECT_EQ(xs, vector({0.0}));
    EXPECT_EQ(ys, vector({0.0}));
    EXPECT_EQ(graph.numEdges(), 1);
    EXPECT_EQ(graph.edges()[0].u, 0);
    EXPECT_EQ(graph.edges()[0].v, 0);
}

TEST(TestDirectedGraphHierarchialLayout, TwoVerticesCycle) {
    SPDirectedGraph graph(2);
    graph.addEdge(0, 1);
    graph.addEdge(1, 0);
    const DirectedGraphHierarchicalLayout layout;
    const auto [xs, ys] = layout.layoutGraph(graph);
    EXPECT_EQ(xs, vector({0.0, 0.0}));
    EXPECT_EQ(ys, vector({0.0, UNIT_SIZE}));
    EXPECT_EQ(graph.numEdges(), 2);
    EXPECT_EQ(graph.edges()[0].u, 0);
    EXPECT_EQ(graph.edges()[0].v, 1);
    EXPECT_EQ(graph.edges()[1].u, 1);
    EXPECT_EQ(graph.edges()[1].v, 0);
}

TEST(TestDirectedGraphHierarchialLayout, SpecialCase1) {
    SPDirectedGraph graph(4);
    graph.addEdge(0, 1); graph.addEdge(0, 2); graph.addEdge(0, 3);
    graph.addEdge(1, 3); graph.addEdge(2, 3);
    graph.addEdge(3, 0);
    const DirectedGraphHierarchicalLayout layout;
    const auto [xs, ys] = layout.layoutGraph(graph);
    EXPECT_EQ(xs, vector({UNIT_SIZE * 1.5, 0.0, UNIT_SIZE, UNIT_SIZE * 1.5, UNIT_SIZE * 2, UNIT_SIZE * 3}));
    EXPECT_EQ(ys, vector({0.0, UNIT_SIZE, UNIT_SIZE, UNIT_SIZE * 2, UNIT_SIZE, UNIT_SIZE}));
    EXPECT_EQ(graph.numEdges(), 8);
}

TEST(TestDirectedGraphHierarchialLayout, RandomNoCheck) {
    const RandomSimpleDirectedGraphGenerator graphGen(128);
    for (size_t caseIndex = 0; caseIndex < 128; ++caseIndex) {
        auto graph = graphGen.generateRandomGraph();
        const DirectedGraphHierarchicalLayout layout;
        const auto [xs, ys] = layout.layoutGraph(graph);
        EXPECT_EQ(xs.size(), graph.numVertices());
        EXPECT_EQ(ys.size(), graph.numVertices());
    }
}

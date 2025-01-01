#include <unordered_set>
#include <gtest/gtest.h>
#include "cross_minimization.h"
#include "graph_def.h"
using namespace std;
using namespace graph_layout;

TEST(TestCrossMinimizationAddVirtualEdges, OneEdgeNoVirtual) {
    SPDirectedGraph graph(2);
    graph.addEdge(0, 1);
    auto ranks = vector({0, 1});
    const auto virtualEdges = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_TRUE(virtualEdges.empty());
    EXPECT_EQ(graph.numVertices(), 2);
    EXPECT_EQ(ranks, vector({0, 1}));
}

TEST(TestCrossMinimizationAddVirtualEdges, OneEdgeGap2) {
    constexpr int offset = CrossMinimization::VIRTUAL_EDGE_ID_OFFSET;
    SPDirectedGraph graph(2);
    graph.addEdge(0, 1);
    auto ranks = vector({0, 2});
    const auto virtualEdges = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_EQ(virtualEdges.size(), 1);
    EXPECT_EQ(virtualEdges[0].originalEdge, SPEdge(0, 0, 1));
    EXPECT_EQ(virtualEdges[0].virtualEdgeIds, vector({offset, offset + 1}));
    EXPECT_EQ(graph.numVertices(), 3);
    EXPECT_EQ(ranks, vector({0, 2, 1}));
}

TEST(TestCrossMinimizationAddVirtualEdges, OneEdgeGap3) {
    constexpr int offset = CrossMinimization::VIRTUAL_EDGE_ID_OFFSET;
    SPDirectedGraph graph(2);
    graph.addEdge(0, 1);
    auto ranks = vector({0, 3});
    const auto virtualEdges = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_EQ(virtualEdges.size(), 1);
    EXPECT_EQ(virtualEdges[0].originalEdge, SPEdge(0, 0, 1));
    EXPECT_EQ(virtualEdges[0].virtualEdgeIds, vector({offset, offset + 1, offset + 2}));
    EXPECT_EQ(graph.numVertices(), 4);
    EXPECT_EQ(ranks, vector({0, 3, 1, 2}));
}

TEST(TestCrossMinimizationAddVirtualEdges, TwoEdgesGap2Gap3) {
    constexpr int offset = CrossMinimization::VIRTUAL_EDGE_ID_OFFSET;
    SPDirectedGraph graph(3);
    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    auto ranks = vector({0, 2, 3});
    const auto virtualEdges = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_EQ(virtualEdges.size(), 2);
    EXPECT_EQ(virtualEdges[0].originalEdge, SPEdge(0, 0, 1));
    EXPECT_EQ(virtualEdges[0].virtualEdgeIds, vector({offset, offset + 1}));
    EXPECT_EQ(virtualEdges[1].originalEdge, SPEdge(1, 0, 2));
    EXPECT_EQ(virtualEdges[1].virtualEdgeIds, vector({offset + 2, offset + 3, offset + 4}));
    EXPECT_EQ(graph.numVertices(), 6);
    EXPECT_EQ(ranks, vector({0, 2, 3, 1, 1, 2}));
}
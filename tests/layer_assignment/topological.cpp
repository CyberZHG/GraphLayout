#include <unordered_set>
#include <gtest/gtest.h>
#include "feedback_arcs.h"
#include "layer_assignment.h"
#include "graph_def.h"
#include "graph_def_utils.h"
using namespace std;
using namespace graph_layout;

TEST(TestLayerAssignmentTopological, EmptyGraph) {
    SimpleDirectedGraph graph(0);
    const LayerAssignment layerAssigment(LayerAssignmentMethod::TOPOLOGICAL);
    const auto ranks = layerAssigment.rankVertices(graph);
    EXPECT_EQ(ranks, vector<int>());
    EXPECT_EQ(layerAssigment.calcRankCost(graph, ranks), 0);
}

TEST(TestLayerAssignmentTopological, SingleNodeNoEdge) {
    SimpleDirectedGraph graph(1);
    const LayerAssignment layerAssigment(LayerAssignmentMethod::TOPOLOGICAL);
    const auto ranks = layerAssigment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({0}));
    EXPECT_EQ(layerAssigment.calcRankCost(graph, ranks), 0);
}

TEST(TestLayerAssignmentTopological, TwoNodesSingleEdge1) {
    SimpleDirectedGraph graph(2);
    graph.addEdge(0, 1);
    const LayerAssignment layerAssigment(LayerAssignmentMethod::TOPOLOGICAL);
    const auto ranks = layerAssigment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({0, 1}));
    EXPECT_EQ(layerAssigment.calcRankCost(graph, ranks), 1);
}

TEST(TestLayerAssignmentTopological, TwoNodesSingleEdge2) {
    SimpleDirectedGraph graph(2);
    graph.addEdge(1, 0);
    const LayerAssignment layerAssigment(LayerAssignmentMethod::TOPOLOGICAL);
    const auto ranks = layerAssigment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({1, 0}));
    EXPECT_EQ(layerAssigment.calcRankCost(graph, ranks), 1);
}

TEST(TestLayerAssignmentTopological, TwoNodesDuplicateEdge) {
    SimpleDirectedGraph graph(2);
    graph.addEdge(0, 1);
    graph.addEdge(0, 1);
    const LayerAssignment layerAssigment(LayerAssignmentMethod::TOPOLOGICAL);
    const auto ranks = layerAssigment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({0, 1}));
    EXPECT_EQ(layerAssigment.calcRankCost(graph, ranks), 2);
}

TEST(TestLayerAssignmentTopological, ThreeNodesLine1) {
    SimpleDirectedGraph graph(3);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    const LayerAssignment layerAssigment(LayerAssignmentMethod::TOPOLOGICAL);
    const auto ranks = layerAssigment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({0, 1, 2}));
    EXPECT_EQ(layerAssigment.calcRankCost(graph, ranks), 2);
}

TEST(TestLayerAssignmentTopological, SpecialCase1) {
    SimpleDirectedGraph graph(4);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(0, 3);
    graph.addEdge(3, 2);
    const LayerAssignment layerAssigment(LayerAssignmentMethod::TOPOLOGICAL);
    const auto ranks = layerAssigment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({0, 1, 2, 1}));
    EXPECT_EQ(layerAssigment.calcRankCost(graph, ranks), 4);
    graph.addEdge(1, 3);
    const auto newRanks = layerAssigment.rankVertices(graph);
    EXPECT_EQ(newRanks, vector({0, 1, 3, 2}));
    EXPECT_EQ(layerAssigment.calcRankCost(graph, newRanks), 7);
}

TEST(TestLayerAssignmentTopological, Random) {
    const RandomGraphGenerator graphGen(128);
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const LayerAssignment layerAssigment(LayerAssignmentMethod::TOPOLOGICAL);
    for (size_t caseIndex = 0; caseIndex < 128; ++caseIndex) {
        auto graph = graphGen.generateGraph();
        if (graph.hasCycle()) {
            const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
            graph.reverseEdges(feedbackArcs);
            EXPECT_FALSE(graph.hasCycle());
        }
        const auto ranks = layerAssigment.rankVertices(graph);
        for (const auto &edge : graph.edges()) {
            EXPECT_LT(ranks[edge.u], ranks[edge.v]);
            GTEST_ASSERT_LT(ranks[edge.u], ranks[edge.v]);
        }
    }
}
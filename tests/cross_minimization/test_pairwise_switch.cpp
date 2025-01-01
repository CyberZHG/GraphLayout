#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "directed/cross_minimization.h"
#include "directed/feedback_arcs.h"
#include "directed/layer_assignment.h"
using namespace std;
using namespace graph_layout;


TEST(TestCrossMinimizationPairwiseSwitch, TwoEdgesCross) {
    SPDirectedGraph graph(4);
    graph.addEdge(0, 3);
    graph.addEdge(1, 2);
    auto ranks = vector({0, 0, 1, 1});
    const CrossMinimization crossMinimization(CrossMinimizationMethod::PAIRWISE_SWITCH);
    auto [initLayering, initVirtualEdges] = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_EQ(CrossMinimization::computeNumCross(graph, initLayering), 1);
    auto [layering, virtualEdges] = crossMinimization.reduceNumCross(graph, ranks);
    EXPECT_EQ(CrossMinimization::computeNumCross(graph, layering), 0);
}

TEST(TestCrossMinimizationPairwiseSwitch, SpecialCase1) {
    SPDirectedGraph graph(5);
    graph.addEdge(0, 4);
    graph.addEdge(1, 4);
    graph.addEdge(2, 3);
    auto ranks = vector({0, 0, 0, 1, 1});
    const CrossMinimization crossMinimization(CrossMinimizationMethod::PAIRWISE_SWITCH);
    auto [initLayering, initVirtualEdges] = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_EQ(CrossMinimization::computeNumCross(graph, initLayering),2);
    auto [layering, virtualEdges] = crossMinimization.reduceNumCross(graph, ranks);
    EXPECT_EQ(CrossMinimization::computeNumCross(graph, layering), 0);
}

TEST(TestCrossMinimizationPairwiseSwitch, SpecialCase2) {
    SPDirectedGraph graph(11);
    graph.addEdge(9, 4); graph.addEdge(8, 1); graph.addEdge(2, 3); graph.addEdge(2, 5); graph.addEdge(9, 10);
    graph.addEdge(0, 9); graph.addEdge(0, 6); graph.addEdge(7, 1); graph.addEdge(9, 1); graph.addEdge(8, 2);
    auto ranks = vector({0, 2, 2, 3, 2, 3, 1, 1, 1, 1, 2});
    const CrossMinimization crossMinimization(CrossMinimizationMethod::PAIRWISE_SWITCH);
    auto [initLayering, initVirtualEdges] = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_EQ(CrossMinimization::computeNumCross(graph, initLayering),1);
    auto [layering, virtualEdges] = crossMinimization.reduceNumCross(graph, ranks);
    EXPECT_EQ(CrossMinimization::computeNumCross(graph, layering), 1);
}

#ifdef GRAPH_LAYOUT_ENABLE_RANDOM_TESTS
TEST(TestCrossMinimizationPairwiseSwitch, RandomPairwiseSwitch) {
    const RandomSimpleDirectedGraphGenerator graphGen(128);
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const LayerAssignment layerAssignment(LayerAssignmentMethod::GANSNER_93);
    CrossMinimization crossMinimization(CrossMinimizationMethod::BARYCENTER);
    GraphComponentSplitter splitter;
    for (size_t caseIndex = 0; caseIndex < 128; ++caseIndex) {
        auto graph = graphGen.generateRandomGraph();
        if (graph.hasCycle()) {
            const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
            graph.reverseEdges(feedbackArcs);
            EXPECT_FALSE(graph.hasCycle());
        }
        for (auto subGraphs = splitter.splitGraph(graph); auto &subGraph : subGraphs) {
            auto ranks = layerAssignment.rankVertices(subGraph);
            auto [layering, virtualEdges] = crossMinimization.reduceNumCross(subGraph, ranks);

            auto [initLayering, initVirtualEdges] = CrossMinimization::addVirtualEdges(subGraph, ranks);
            const auto initNumCross = CrossMinimization::computeNumCross(subGraph, initLayering);
            crossMinimization.setMethod(CrossMinimizationMethod::PAIRWISE_SWITCH);
            const auto numCross = CrossMinimization::computeNumCross(subGraph, layering);
            EXPECT_GE(initNumCross, numCross);
        }
    }
}
#endif

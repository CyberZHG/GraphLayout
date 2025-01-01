#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "directed/cross_minimization.h"
#include "directed/feedback_arcs.h"
#include "directed/layer_assignment.h"
using namespace std;
using namespace graph_layout;


TEST(TestCrossMinimizationBaryCenter, TwoEdgesCross) {
    SPDirectedGraph graph(4);
    graph.addEdge(0, 3);
    graph.addEdge(1, 2);
    auto ranks = vector({0, 0, 1, 1});
    const CrossMinimization crossMinimization(CrossMinimizationMethod::BARYCENTER);
    auto [initLayering, initVirtualEdges] = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_EQ(CrossMinimization::computeNumCross(graph, initLayering), 1);
    auto [layering, virtualEdges] = crossMinimization.reduceNumCross(graph, ranks);
    EXPECT_EQ(CrossMinimization::computeNumCross(graph, layering), 0);
}

TEST(TestCrossMinimizationBaryCenter, SpecialCase1) {
    SPDirectedGraph graph(5);
    graph.addEdge(0, 4);
    graph.addEdge(1, 4);
    graph.addEdge(2, 3);
    auto ranks = vector({0, 0, 0, 1, 1});
    const CrossMinimization crossMinimization(CrossMinimizationMethod::BARYCENTER);
    auto [initLayering, initVirtualEdges] = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_EQ(CrossMinimization::computeNumCross(graph, initLayering),2);
    auto [layering, virtualEdges] = crossMinimization.reduceNumCross(graph, ranks);
    EXPECT_EQ(CrossMinimization::computeNumCross(graph, layering), 0);
}

TEST(TestCrossMinimizationBaryCenter, RandomBaryCenter) {
    const RandomSimpleDirectedGraphGenerator graphGen(128);
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const LayerAssignment layerAssignment(LayerAssignmentMethod::GANSNER_93);
    const CrossMinimization crossMinimization(CrossMinimizationMethod::BARYCENTER);
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
            auto [initLayering, initVirtualEdges] = CrossMinimization::addVirtualEdges(subGraph, ranks);
            auto [layering, virtualEdges] = crossMinimization.reduceNumCross(subGraph, ranks);
            const auto initNumCross = CrossMinimization::computeNumCross(subGraph, initLayering);
            const auto numCross = CrossMinimization::computeNumCross(subGraph, layering);
            EXPECT_GE(initNumCross, numCross);
        }
    }
}

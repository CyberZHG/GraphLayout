#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "cross_minimization.h"
#include "graph_def.h"
#include "feedback_arcs.h"
#include "layer_assignment.h"
using namespace std;
using namespace graph_layout;


TEST(TestCrossMinimizationBaryCenter, TwoEdgesCross) {
    SPDirectedGraph graph(4);
    graph.addEdge(0, 3);
    graph.addEdge(1, 2);
    auto ranks = vector({0, 0, 1, 1});
    const CrossMinimization crossMinimization(CrossMinimizationMethod::BARYCENTER);
    auto [initLayeredOrder, initVirtualEdges] = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, initLayeredOrder), 1);
    auto [layeredOrder, virtualEdges] = crossMinimization.reduceNumCross(graph, ranks);
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layeredOrder), 0);
}

TEST(TestCrossMinimizationBaryCenter, SpecialCase1) {
    SPDirectedGraph graph(5);
    graph.addEdge(0, 4);
    graph.addEdge(1, 4);
    graph.addEdge(2, 3);
    auto ranks = vector({0, 0, 0, 1, 1});
    const CrossMinimization crossMinimization(CrossMinimizationMethod::BARYCENTER);
    auto [initLayeredOrder, initVirtualEdges] = CrossMinimization::addVirtualEdges(graph, ranks);
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, initLayeredOrder),2);
    auto [layeredOrder, virtualEdges] = crossMinimization.reduceNumCross(graph, ranks);
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layeredOrder), 0);
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
            auto [initLayeredOrder, initVirtualEdges] = CrossMinimization::addVirtualEdges(subGraph, ranks);
            auto [layeredOrder, virtualEdges] = crossMinimization.reduceNumCross(subGraph, ranks);
            const auto initNumCross = CrossMinimization::calcNumCross(subGraph, initLayeredOrder);
            const auto numCross = CrossMinimization::calcNumCross(subGraph, layeredOrder);
            EXPECT_GE(initNumCross, numCross);
        }
    }
}

#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "cross_minimization.h"
#include "graph_def.h"
#include "feedback_arcs.h"
#include "layer_assignment.h"
using namespace std;
using namespace graph_layout;

class TestCrossMinimization : public CrossMinimization {
public:
    using CrossMinimization::CrossMinimization;
    using CrossMinimization::getInitialLayers;
};

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

void testNumCrossing(SPDirectedGraph &graph, const vector<vector<int>> &layers) {
    long long numCross = 0;
    for (size_t i = 0; i + 1 < layers.size(); ++i) {
        unordered_map<int, int> positions;
        for (int j = 0; j < layers[i + 1].size(); ++j) {
            positions[layers[i + 1][j]] = j;
        }
        for (int j = 0; j < layers[i].size(); ++j) {
            for (int k = j + 1; k < layers[i].size(); ++k) {
                for (const auto &edge1 : graph.getOutEdges(layers[i][j])) {
                    for (const auto &edge2 : graph.getOutEdges(layers[i][k])) {
                        if (positions[edge1.v] > positions[edge2.v]) {
                            ++numCross;
                        }
                    }
                }
            }
        }
    }
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layers), numCross);
}

TEST(TestCrossMinimizationNumCross, SpecialCase1) {
    SPDirectedGraph graph(4);
    graph.addEdge(0, 2);
    graph.addEdge(1, 3);
    const auto layers = vector<vector<int>>({{0, 1}, {3, 2}});
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layers), 1);
    testNumCrossing(graph, layers);
}

TEST(TestCrossMinimizationNumCross, SpecialCase2) {
    SPDirectedGraph graph(6);
    graph.addEdge(0, 5);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    const auto layers = vector<vector<int>>({{0, 1, 2}, {3, 4, 5}});
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layers), 2);
    testNumCrossing(graph, layers);
    graph.addEdge(1, 5);
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layers), 3);
    testNumCrossing(graph, layers);
    graph.addEdge(2, 3);
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layers), 5);
    testNumCrossing(graph, layers);
    graph.addEdge(0, 4);
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layers), 7);
    testNumCrossing(graph, layers);
    graph.addEdge(1, 4);
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layers), 9);
    testNumCrossing(graph, layers);
}

TEST(TestCrossMinimizationNumCross, SpecialCase3) {
    SPDirectedGraph graph(9);
    graph.addEdge(0, 5);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    graph.addEdge(3, 7);
    graph.addEdge(4, 8);
    graph.addEdge(5, 6);
    const auto layers = vector<vector<int>>({{0, 1, 2}, {3, 4, 5}, {6, 7, 8}});
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layers), 4);
    testNumCrossing(graph, layers);
}

TEST(TestCrossMinimizationNumCross, SpecialCase4) {
    SPDirectedGraph graph(8);
    graph.addEdge(0, 7);
    graph.addEdge(0, 6);
    graph.addEdge(3, 5);
    graph.addEdge(0, 6);
    graph.addEdge(0, 7);
    graph.addEdge(2, 7);
    graph.addEdge(2, 4);
    graph.addEdge(1, 5);
    const auto layers = vector<vector<int>>({{0, 1, 2, 3}, {4, 5, 6, 7}});
    EXPECT_EQ(CrossMinimization::calcNumCross(graph, layers), 14);
    testNumCrossing(graph, layers);
}

TEST(TestCrossMinimizationNumCross, RandomTwoLayers) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> numVertices(1, 64);
    for (size_t caseIndex = 0; caseIndex < 128; ++caseIndex) {
        const int n1 = numVertices(gen);
        const int n2 = numVertices(gen);
        SPDirectedGraph graph(n1 + n2);
        uniform_int_distribution<> randNumEdges(0, (n1 + n2) * 2);
        uniform_int_distribution<> randU(0, n1 - 1);
        uniform_int_distribution<> randV(n1, n1 + n2 - 1);
        int m = randNumEdges(gen);
        while (m--) {
            const int u = randU(gen);
            const int v = randV(gen);
            graph.addEdge(u, v);
        }
        vector<vector<int>> layers(2);
        for (int i = 0; i < n1; ++i) {
            layers[0].push_back(i);
        }
        for (int i = n1; i < n1 + n2; ++i) {
            layers[1].push_back(i);
        }
        testNumCrossing(graph, layers);
    }
}

TEST(TestCrossMinimizationNumCross, Random) {
    const RandomSimpleDirectedGraphGenerator graphGen(128);
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const LayerAssignment layerAssignment(LayerAssignmentMethod::GANSNER_93);
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
            CrossMinimization::addVirtualEdges(subGraph, ranks);
            const auto layers = TestCrossMinimization::getInitialLayers(ranks);
            testNumCrossing(subGraph, layers);
        }
    }
}

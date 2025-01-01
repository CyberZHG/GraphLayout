#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "feedback_arcs.h"
#include "layer_assignment.h"
#include "graph_def.h"
using namespace std;
using namespace graph_layout;

class TestLayerAssignment : public LayerAssignment {
public:
    using LayerAssignment::LayerAssignment;
    using LayerAssignment::networkSimplexInitFeasibleTree;
};

TEST(TestLayerAssignmentNetworkSimplex, EmptyGraph) {
    SimpleDirectedGraph graph(0);
    const LayerAssignment layerAssignment(LayerAssignmentMethod::NETWORK_SIMPLEX);
    const auto ranks = layerAssignment.rankVertices(graph);
    EXPECT_EQ(ranks, vector<int>());
}

TEST(TestLayerAssignmentNetworkSimplex, SingleNodeNoEdge) {
    SimpleDirectedGraph graph(1);
    const LayerAssignment layerAssignment(LayerAssignmentMethod::NETWORK_SIMPLEX);
    const auto ranks = layerAssignment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({0}));
}

void testInitialFeasibleTree(SimpleDirectedGraph &graph,
    const TestLayerAssignment &layerAssignment,
    const vector<int> &ranks, const
    vector<int> &parents) {
    const size_t n = graph.numVertices();
    EXPECT_EQ(parents.size(), n);
    int noParentCount = 0;
    SimpleDirectedGraph newGraph(n);
    for (const auto id : parents) {
        if (id == -1) {
            noParentCount++;
            continue;
        }
        const auto &edge = graph.getEdge(id);
        EXPECT_EQ(edge.id, id);
        EXPECT_EQ(ranks[edge.v] - ranks[edge.u], layerAssignment.minEdgeLength(id));
    }
    EXPECT_EQ(noParentCount, 1);
    GraphComponentSplitter splitter;
    const auto &subGraphs = splitter.splitGraph(graph);
    EXPECT_EQ(subGraphs.size(), 1);
}

unordered_map<int, int> genRandomEdgeMinLengths(const SimpleDirectedGraph &graph) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> lengthDist(1, 5);
    unordered_map<int, int> minEdgeLengths;
    for (const auto &edge : graph.edges()) {
        minEdgeLengths[edge.id] = lengthDist(gen);
    }
    return minEdgeLengths;
}

TEST(TestLayerAssignmentNetworkSimplexInitialFeasibleTree, SpecialCase1) {
    SimpleDirectedGraph graph(4);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(0, 3);
    const TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    auto ranks = layerAssignment.rankVertices(graph);
    const auto parents = layerAssignment.networkSimplexInitFeasibleTree(graph, ranks);
    testInitialFeasibleTree(graph, layerAssignment, ranks, parents);
}

TEST(TestLayerAssignmentNetworkSimplexInitialFeasibleTree, SpecialCase2) {
    SimpleDirectedGraph graph(5);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(0, 4);
    graph.addEdge(3, 3);
    const TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    auto ranks = layerAssignment.rankVertices(graph);
    const auto parents = layerAssignment.networkSimplexInitFeasibleTree(graph, ranks);
    testInitialFeasibleTree(graph, layerAssignment, ranks, parents);
}

TEST(TestLayerAssignmentNetworkSimplexInitialFeasibleTree, Random) {
    const RandomSimpleDirectedGraphGenerator graphGen(128);
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    GraphComponentSplitter splitter;
    for (size_t caseIndex = 0; caseIndex < 128; ++caseIndex) {
        auto graph = graphGen.generateGraph();
        if (graph.hasCycle()) {
            const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
            graph.reverseEdges(feedbackArcs);
            EXPECT_FALSE(graph.hasCycle());
        }
        for (auto subGraphs = splitter.splitGraph(graph); auto &subGraph : subGraphs) {
            auto ranks = layerAssignment.rankVertices(subGraph);
            auto minEdgeLengths = genRandomEdgeMinLengths(subGraph);
            layerAssignment.setMinimumEdgeLengths(std::move(minEdgeLengths));
            const auto parents = layerAssignment.networkSimplexInitFeasibleTree(subGraph, ranks);
            testInitialFeasibleTree(subGraph, layerAssignment, ranks, parents);
            layerAssignment.cleanMinimumEdgeLengths();
        }
    }
}

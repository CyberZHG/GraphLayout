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
    using LayerAssignment::networkSimplexComputeCutValues;
    using LayerAssignment::NO_PARENT;
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
        if (id == TestLayerAssignment::NO_PARENT) {
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
    const auto [root, parents] = layerAssignment.networkSimplexInitFeasibleTree(graph, ranks);
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
    const auto [root, parents] = layerAssignment.networkSimplexInitFeasibleTree(graph, ranks);
    testInitialFeasibleTree(graph, layerAssignment, ranks, parents);
}

void testCutValues(SimpleDirectedGraph &graph, const vector<int> &parents, const vector<int> &cuts) {
    const size_t n = graph.numVertices();
    vector<int> p(n);
    function<int(int)> find = [&](const int u) {
        return u == p[u] ? u : (p[u] = find(p[u]));
    };
    auto combine = [&](const int u, const int v) {
        p[find(u)] = find(v);
    };
    for (int i = 0; i < n; ++i) {
        const auto cutId = parents[i];
        if (cutId == TestLayerAssignment::NO_PARENT) {
            continue;
        }
        for (int u = 0; u < n; ++u) {
            p[u] = u;
        }
        for (const auto id : parents) {
            if (id != TestLayerAssignment::NO_PARENT && id != cutId) {
                const auto &edge = graph.getEdge(id);
                combine(edge.u, edge.v);
            }
        }
        const auto &cutEdge = graph.getEdge(cutId);
        int expectedCutValue = 0;
        for (const auto &edge : graph.edges()) {
            if (find(edge.u) == find(cutEdge.u) && find(edge.v) == find(cutEdge.v)) {
                --expectedCutValue;
            } else if (find(edge.u) == find(cutEdge.v) && find(edge.v) == find(cutEdge.u)) {
                ++expectedCutValue;
            }
        }
        EXPECT_EQ(cuts[i], expectedCutValue);
    }
}

TEST(TestLayerAssignmentNetworkSimplexComputeCutValues, SpecialCase100) {
    SimpleDirectedGraph graph(4);
    graph.addEdge(2, 0);
    graph.addEdge(0, 1);
    graph.addEdge(3, 1);
    graph.addEdge(0, 3);
    const TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    auto ranks = layerAssignment.rankVertices(graph);
    const auto [root, parents] = layerAssignment.networkSimplexInitFeasibleTree(graph, ranks);
    testInitialFeasibleTree(graph, layerAssignment, ranks, parents);
    const auto cuts = layerAssignment.networkSimplexComputeCutValues(graph, root, parents);
    testCutValues(graph, parents, cuts);
}

TEST(TestLayerAssignmentNetworkSimplexComputeCutValues, Random) {
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
            const auto [root, parents] = layerAssignment.networkSimplexInitFeasibleTree(subGraph, ranks);
            testInitialFeasibleTree(subGraph, layerAssignment, ranks, parents);
            const auto cuts = layerAssignment.networkSimplexComputeCutValues(subGraph, root, parents);
            testCutValues(subGraph, parents, cuts);
            layerAssignment.cleanMinimumEdgeLengths();
        }
    }
}

#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "directed/feedback_arcs.h"
#include "directed/layer_assignment.h"
using namespace std;
using namespace graph_layout;

class TestLayerAssignment : public LayerAssignment {
public:
    using LayerAssignment::LayerAssignment;
    using LayerAssignment::gansner93InitFeasibleTree;
    using LayerAssignment::gansner93ComputeCutValues;
    using LayerAssignment::NO_PARENT;
};

TEST(TestLayerAssignmentGansner93, EmptyGraph) {
    SPDirectedGraph graph(0);
    const LayerAssignment layerAssignment(LayerAssignmentMethod::GANSNER_93);
    const auto ranks = layerAssignment.rankVertices(graph);
    EXPECT_EQ(ranks, vector<int>());
}

TEST(TestLayerAssignmentGansner93, SingleNodeNoEdge) {
    SPDirectedGraph graph(1);
    const LayerAssignment layerAssignment(LayerAssignmentMethod::GANSNER_93);
    const auto ranks = layerAssignment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({0}));
}

void testInitialFeasibleTree(SPDirectedGraph &graph,
    const TestLayerAssignment &layerAssignment,
    const vector<int> &ranks, const
    vector<int> &parents) {
    const size_t n = graph.numVertices();
    EXPECT_EQ(parents.size(), n);
    int noParentCount = 0;
    SPDirectedGraph newGraph(n);
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

unordered_map<int, int> genRandomEdgeMinLengths(const SPDirectedGraph &graph) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> lengthDist(1, 5);
    unordered_map<int, int> minEdgeLengths;
    for (const auto &edge : graph.edges()) {
        minEdgeLengths[edge.id] = lengthDist(gen);
    }
    return minEdgeLengths;
}

TEST(TestLayerAssignmentGansner93InitialFeasibleTree, SpecialCase1) {
    SPDirectedGraph graph(4);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(0, 3);
    const TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    auto ranks = layerAssignment.rankVertices(graph);
    const auto [root, parents] = layerAssignment.gansner93InitFeasibleTree(graph, ranks);
    testInitialFeasibleTree(graph, layerAssignment, ranks, parents);
}

TEST(TestLayerAssignmentGansner93InitialFeasibleTree, SpecialCase2) {
    SPDirectedGraph graph(5);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(0, 4);
    graph.addEdge(4, 3);
    const TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    auto ranks = layerAssignment.rankVertices(graph);
    const auto [root, parents] = layerAssignment.gansner93InitFeasibleTree(graph, ranks);
    testInitialFeasibleTree(graph, layerAssignment, ranks, parents);
}

void testCutValues(SPDirectedGraph &graph, const vector<int> &parents, const vector<int> &cuts) {
    const size_t n = graph.numVertices();
    vector<int> p(n);
    function<int(int)> find = [&](const int u) {
        return u == p[u] ? u : (p[u] = find(p[u]));
    };
    auto combine = [&](const int u, const int v) {
        p[find(u)] = find(v);
    };
    for (int i = 0; i < static_cast<int>(n); ++i) {
        const auto cutId = parents[i];
        if (cutId == TestLayerAssignment::NO_PARENT) {
            continue;
        }
        for (int u = 0; u < static_cast<int>(n); ++u) {
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
                ++expectedCutValue;
            } else if (find(edge.u) == find(cutEdge.v) && find(edge.v) == find(cutEdge.u)) {
                --expectedCutValue;
            }
        }
        EXPECT_EQ(cuts[i], expectedCutValue);
    }
}

TEST(TestLayerAssignmentGansner93, SpecialCase100) {
    SPDirectedGraph graph(4);
    graph.addEdge(2, 0);
    graph.addEdge(0, 1);
    graph.addEdge(3, 1);
    graph.addEdge(0, 3);
    TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    auto ranks = layerAssignment.rankVertices(graph);
    const auto [root, parents] = layerAssignment.gansner93InitFeasibleTree(graph, ranks);
    testInitialFeasibleTree(graph, layerAssignment, ranks, parents);
    auto tree = graph.buildSpanningTree(parents);
    const auto [minCutIndex, cuts] = layerAssignment.gansner93ComputeCutValues(graph, tree, root, parents);
    testCutValues(graph, parents, cuts);
    layerAssignment.setMethod(LayerAssignmentMethod::GANSNER_93);
    ranks = layerAssignment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({1, 3, 0, 2}));
}

TEST(TestLayerAssignmentGansner93, SpecialCase101) {
    SPDirectedGraph graph(6);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(0, 4);
    graph.addEdge(4, 3);
    graph.addEdge(2, 5);
    graph.addEdge(4, 5);
    TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    auto ranks = layerAssignment.rankVertices(graph);
    const auto [root, parents] = layerAssignment.gansner93InitFeasibleTree(graph, ranks);
    testInitialFeasibleTree(graph, layerAssignment, ranks, parents);
    auto tree = graph.buildSpanningTree(parents);
    const auto [minCutIndex, cuts] = layerAssignment.gansner93ComputeCutValues(graph, tree, root, parents);
    testCutValues(graph, parents, cuts);
    auto parents2 = vector({-1, 0, 1, 2, 4, 5});
    auto ranks2 = vector({-1, 0, 1, 2, 1, 2});
    auto tree2 = graph.buildSpanningTree(parents2);
    const auto [minCutIndex2, cuts2] = layerAssignment.gansner93ComputeCutValues(graph, tree2, root, parents2);
    testCutValues(graph, parents, cuts);
    layerAssignment.setMethod(LayerAssignmentMethod::GANSNER_93);
    ranks = layerAssignment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({0, 1, 2, 3, 2, 3}));
}

TEST(TestLayerAssignmentGansner93, SpecialCase102) {
    SPDirectedGraph graph(6);
    graph.addEdge(4, 0);
    graph.addEdge(0, 1);
    graph.addEdge(2, 3);
    graph.addEdge(5, 3);
    graph.addEdge(5, 1);
    graph.addEdge(5, 0);
    graph.addEdge(5, 0);
    unordered_map<int, int> minEdgeLengths({{6, 4}, {5, 4}, {4, 5}, {3, 4}, {2, 1}, {1, 1}, {0, 4}});
    TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    layerAssignment.setMinEdgeLengths(std::move(minEdgeLengths));
    auto ranks = layerAssignment.rankVertices(graph);
    const auto [root, parents] = layerAssignment.gansner93InitFeasibleTree(graph, ranks);
    testInitialFeasibleTree(graph, layerAssignment, ranks, parents);
    auto tree = graph.buildSpanningTree(parents);
    const auto [minCutIndex, cuts] = layerAssignment.gansner93ComputeCutValues(graph, tree, root, parents);
    testCutValues(graph, parents, cuts);
    layerAssignment.setMethod(LayerAssignmentMethod::GANSNER_93);
    ranks = layerAssignment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({4, 5, 3, 4, 0, 0}));
}

TEST(TestLayerAssignmentGansner93, SpecialCase103) {
    SPDirectedGraph graph(6);
    graph.addEdge(0, 4); graph.addEdge(2, 1); graph.addEdge(2, 1); graph.addEdge(0, 2); graph.addEdge(1, 5);
    graph.addEdge(0, 3); graph.addEdge(4, 5); graph.addEdge(0, 5); graph.addEdge(4, 5); graph.addEdge(1, 5);
    graph.addEdge(0, 4); graph.addEdge(0, 3); graph.addEdge(0, 1); graph.addEdge(3, 4); graph.addEdge(2, 5);
    unordered_map<int, int> minEdgeLengths({
        {0, 5}, {1, 4}, {2, 1}, {3, 2}, {4, 4},
        {5, 1}, {6, 3}, {7, 2}, {8, 5}, {9, 2},
        {10, 2}, {11, 5}, {12, 5}, {13, 4}, {14, 1},
    });
    TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    layerAssignment.setMinEdgeLengths(std::move(minEdgeLengths));
    auto ranks = layerAssignment.rankVertices(graph);
    const auto [root, parents] = layerAssignment.gansner93InitFeasibleTree(graph, ranks);
    testInitialFeasibleTree(graph, layerAssignment, ranks, parents);
    auto tree = graph.buildSpanningTree(parents);
    const auto [minCutIndex, cuts] = layerAssignment.gansner93ComputeCutValues(graph, tree, root, parents);
    testCutValues(graph, parents, cuts);
    layerAssignment.setMethod(LayerAssignmentMethod::GANSNER_93);
    ranks = layerAssignment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({0, 10, 6, 5, 9, 14}));
}

TEST(TestLayerAssignmentGansner93, SpecialCase104) {
    SPDirectedGraph graph(5);
    graph.addEdge(1, 0); graph.addEdge(4, 3); graph.addEdge(2, 4); graph.addEdge(1, 2);
    graph.addEdge(0, 3); graph.addEdge(2, 3); graph.addEdge(1, 4); graph.addEdge(2, 0);
    unordered_map<int, int> minEdgeLengths({
        {0, 2}, {1, 5}, {2, 1}, {3, 2},
        {4, 2}, {5, 1}, {6, 4}, {7, 1},
    });
    testCutValues(graph, {7, -1, 3, 1, 6}, {1, 0, -1, 3, 4});
    TestLayerAssignment layerAssignment(LayerAssignmentMethod::GANSNER_93);
    layerAssignment.setMinEdgeLengths(std::move(minEdgeLengths));
    const auto ranks = layerAssignment.rankVertices(graph);
    EXPECT_EQ(ranks, vector({4, 0, 3, 9, 4}));
}

#ifdef GRAPH_LAYOUT_BUILD_RANDOM_TESTS
TEST(TestLayerAssignmentGansner93ComputeCutValues, Random) {
    const RandomSimpleDirectedGraphGenerator graphGen(128);
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    TestLayerAssignment layerAssignment(LayerAssignmentMethod::TOPOLOGICAL);
    GraphComponentSplitter splitter;
    for (size_t caseIndex = 0; caseIndex < 128; ++caseIndex) {
        auto graph = graphGen.generateRandomGraph();
        if (graph.hasCycle()) {
            const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
            graph.reverseEdges(feedbackArcs);
            EXPECT_FALSE(graph.hasCycle());
        }
        for (auto subGraphs = splitter.splitGraph(graph); auto &subGraph : subGraphs) {
            auto minEdgeLengths = genRandomEdgeMinLengths(subGraph);
            layerAssignment.setMinEdgeLengths(std::move(minEdgeLengths));
            layerAssignment.setMethod(LayerAssignmentMethod::TOPOLOGICAL);
            auto ranks = layerAssignment.rankVertices(subGraph);
            const long long initialTotalEdgeLength = LayerAssignment::computeTotalEdgeLength(subGraph, ranks);
            const auto [root, parents] = layerAssignment.gansner93InitFeasibleTree(subGraph, ranks);
            testInitialFeasibleTree(subGraph, layerAssignment, ranks, parents);
            auto tree = subGraph.buildSpanningTree(parents);
            const auto [minCutIndex, cuts] = layerAssignment.gansner93ComputeCutValues(subGraph, tree, root, parents);
            testCutValues(subGraph, parents, cuts);
            layerAssignment.setMethod(LayerAssignmentMethod::GANSNER_93);
            ranks = layerAssignment.rankVertices(subGraph);
            const long long finalTotalEdgeLength = LayerAssignment::computeTotalEdgeLength(subGraph, ranks);
            EXPECT_LE(finalTotalEdgeLength, initialTotalEdgeLength);
            layerAssignment.clearMinEdgeLengths();
        }
    }
}
#endif

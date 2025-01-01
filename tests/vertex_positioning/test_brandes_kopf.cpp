#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "feedback_arcs.h"
#include "layer_assignment.h"
#include "cross_minimization.h"
#include "vertex_positioning.h"
using namespace std;
using namespace graph_layout;

class TestVertexPositioning : public VertexPositioning {
public:
    using VertexPositioning::VertexPositioning;
    using VertexPositioning::sortIncidentEdges;
    using VertexPositioning::verticalAlignment;
    using VertexPositioning::horizontalCompaction;
    using VertexPositioning::assignYCoordinates;
};

TEST(TestVertexPositioningSortIncidentEdges, EmptyGraph) {
    SPDirectedGraph graph(0);
    SPLayering layering;
    TestVertexPositioning::sortIncidentEdges(graph, layering);
}

TEST(TestVertexPositioningSortIncidentEdges, SpecialCase1) {
    SPDirectedGraph graph(5);
    graph.addEdge(0, 1); graph.addEdge(0, 2); graph.addEdge(0, 3);
    graph.addEdge(1, 4); graph.addEdge(2, 4); graph.addEdge(3, 4);
    SPLayering layering;
    layering.orders = {{0}, {2, 1, 3}, {4}};
    layering.initMappings();
    TestVertexPositioning::sortIncidentEdges(graph, layering);
    vector<int> vertices;
    for (const auto& edge : graph.getOutEdges(0)) {
        vertices.emplace_back(edge.v);
    }
    EXPECT_EQ(vertices, vector({2, 1, 3}));
    vertices.clear();
    for (const auto& edge : graph.getInEdges(4)) {
        vertices.emplace_back(edge.u);
    }
    EXPECT_EQ(vertices, vector({2, 1, 3}));
}

TEST(TestVertexPositioningHorizontalCompaction, SpecialCase101) {
    SPDirectedGraph graph(4);
    graph.addEdge(0, 1); graph.addEdge(0, 2);
    graph.addEdge(1, 3); graph.addEdge(2, 3);
    SPLayering layering;
    layering.layerRanks = {0, 1, 2};
    layering.orders = {{0}, {1, 2}, {3}};
    layering.initMappings();
    TestVertexPositioning vertexPositioning;
    constexpr double UNIT_SIZE = TestVertexPositioning::DEFAULT_VERTEX_SIZE + TestVertexPositioning::DEFAULT_VERTEX_MARGIN;
    const auto [rootsFL, alignsFL] = TestVertexPositioning::verticalAlignment(graph, layering, true, true);
    EXPECT_EQ(rootsFL, vector({0, 0, 2, 0}));
    EXPECT_EQ(alignsFL, vector({1, 3, 2, 0}));
    const auto positionsFL = vertexPositioning.horizontalCompaction(graph, layering, rootsFL, alignsFL, true);
    EXPECT_EQ(positionsFL, vector({0.0, 0.0, UNIT_SIZE, 0.0}));
    const auto [rootsFR, alignsFR] = TestVertexPositioning::verticalAlignment(graph, layering, true, false);
    EXPECT_EQ(rootsFR, vector({0, 1, 0, 0}));
    EXPECT_EQ(alignsFR, vector({2, 1, 3, 0}));
    const auto positionsFR = vertexPositioning.horizontalCompaction(graph, layering, rootsFR, alignsFR, false);
    EXPECT_EQ(positionsFR, vector({UNIT_SIZE, 0.0, UNIT_SIZE, UNIT_SIZE}));
    const auto [rootsBL, alignsBL] = TestVertexPositioning::verticalAlignment(graph, layering, false, true);
    EXPECT_EQ(rootsBL, vector({3, 3, 2, 3}));
    EXPECT_EQ(alignsBL, vector({3, 0, 2, 1}));
    const auto positionsBL = vertexPositioning.horizontalCompaction(graph, layering, rootsBL, alignsBL, true);
    EXPECT_EQ(positionsBL, vector({0.0, 0.0, UNIT_SIZE, 0.0}));
    const auto [rootsBR, alignsBR] = TestVertexPositioning::verticalAlignment(graph, layering, false, false);
    EXPECT_EQ(rootsBR, vector({3, 1, 3, 3}));
    EXPECT_EQ(alignsBR, vector({3, 1, 0, 2}));
    const auto positionsBR = vertexPositioning.horizontalCompaction(graph, layering, rootsBR, alignsBR, false);
    EXPECT_EQ(positionsBR, vector({UNIT_SIZE, 0.0, UNIT_SIZE, UNIT_SIZE}));
    const auto positionsY = vertexPositioning.assignYCoordinates(graph, layering);
    EXPECT_EQ(positionsY, vector({0.0, UNIT_SIZE, UNIT_SIZE, UNIT_SIZE * 2}));
}

pair<SPDirectedGraph, SPLayering> createTestGraphSpecialCase102() {
    SPDirectedGraph graph(23);
    const vector<vector<int>> edges = {
        {3, 4, 10, 13}, {3, 20}, {4, 5}, {6}, {7},
        {8, 16, 23}, {9}, {10, 11}, {12}, {13, 14, 15},
        {15, 16}, {20}, {17}, {17, 18}, {},
        {18, 19, 20}, {}, {21}, {22}, {},
        {23}, {23},
    };
    for (int u = 0; u < edges.size(); ++u) {
        for (const auto v : edges[u]) {
            graph.addEdge(u, v - 1);
        }
    }
    vector ranks {0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 9};
    const auto [layering, virtualEdges] = CrossMinimization::addVirtualEdges(graph, ranks);
    return {graph, layering};
}

TEST(TestVertexPositioningHorizontalCompaction, SpecialCase102FL) {
    auto [graph, layering] = createTestGraphSpecialCase102();
    const TestVertexPositioning vertexPositioning;
    const auto [rootsFL, alignsFL] = TestVertexPositioning::verticalAlignment(graph, layering, true, true);
    const auto positionsFL = vertexPositioning.horizontalCompaction(graph, layering, rootsFL, alignsFL, true);
    EXPECT_EQ(vector({positionsFL[0], positionsFL[12], positionsFL[16]}), vector<double>(3, 0.0));
}

TEST(TestVertexPositioningBrandesKopf, RandomNoCheck) {
    const RandomSimpleDirectedGraphGenerator graphGen(128);
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const LayerAssignment layerAssignment(LayerAssignmentMethod::GANSNER_93);
    const CrossMinimization crossMinimization(CrossMinimizationMethod::BARYCENTER);
    const VertexPositioning vertexPositioning(VertexPositioningMethod::BRANDES_KOPF);
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
            const auto [xs, ys] = vertexPositioning.assignCoordinates(subGraph, layering);
            EXPECT_EQ(xs.size(), subGraph.numVertices());
            EXPECT_EQ(ys.size(), subGraph.numVertices());
        }
    }
}
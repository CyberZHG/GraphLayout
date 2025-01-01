#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "vertex_positioning.h"
using namespace std;
using namespace graph_layout;

class TestVertexPositioning : public VertexPositioning {
public:
    using VertexPositioning::VertexPositioning;
    using VertexPositioning::sortIncidentEdges;
    using VertexPositioning::verticalAlignment;
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
    layering.initPositions();
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

TEST(TestVertexPositioningVerticalAlignment, SpecialCase101) {
    SPDirectedGraph graph(4);
    graph.addEdge(0, 1); graph.addEdge(0, 2);
    graph.addEdge(1, 3); graph.addEdge(2, 3);
    SPLayering layering;
    layering.orders = {{0}, {1, 2}, {3}};
    layering.initPositions();
    const auto [rootsFL, alignsFL] = TestVertexPositioning::verticalAlignment(graph, layering, true, true);
    EXPECT_EQ(rootsFL, vector({0, 0, 2, 0}));
    EXPECT_EQ(alignsFL, vector({1, 3, 2, 0}));
    const auto [rootsFR, alignsFR] = TestVertexPositioning::verticalAlignment(graph, layering, true, false);
    EXPECT_EQ(rootsFR, vector({0, 1, 0, 0}));
    EXPECT_EQ(alignsFR, vector({2, 1, 3, 0}));
    const auto [rootsBL, alignsBL] = TestVertexPositioning::verticalAlignment(graph, layering, false, true);
    EXPECT_EQ(rootsBL, vector({3, 3, 2, 3}));
    EXPECT_EQ(alignsBL, vector({3, 0, 2, 1}));
    const auto [rootsBR, alignsBR] = TestVertexPositioning::verticalAlignment(graph, layering, false, false);
    EXPECT_EQ(rootsBR, vector({3, 1, 3, 3}));
    EXPECT_EQ(alignsBR, vector({3, 1, 0, 2}));
}
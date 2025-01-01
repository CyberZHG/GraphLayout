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
#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "graph_layout.h"
using namespace std;
using namespace graph_layout;

constexpr double UNIT_SIZE = VertexPositioning::DEFAULT_VERTEX_SIZE + VertexPositioning::DEFAULT_VERTEX_MARGIN;

TEST(TestDirectedGraphHierarchialLayout, EmptyGraph) {
    const auto graph = make_shared<SPDirectedGraph>(0);
    DirectedGraphHierarchicalLayout layout;
    layout.setGraph(graph);
    const auto [xs, ys] = layout.layoutGraph();
    EXPECT_EQ(xs, vector<double>({}));
    EXPECT_EQ(ys, vector<double>({}));
}

TEST(TestDirectedGraphHierarchialLayout, SingleVertexSelfCycle) {
    const auto graph = make_shared<SPDirectedGraph>(1);
    graph->addEdge(0, 0);
    DirectedGraphHierarchicalLayout layout;
    layout.setGraph(graph);
    const auto [xs, ys] = layout.layoutGraph();
    EXPECT_EQ(xs, vector({0.0}));
    EXPECT_EQ(ys, vector({0.0}));
    EXPECT_EQ(graph->numEdges(), 1);
    EXPECT_EQ(graph->edges()[0].u, 0);
    EXPECT_EQ(graph->edges()[0].v, 0);
}

TEST(TestDirectedGraphHierarchialLayout, TwoVerticesCycle) {
    const auto graph = make_shared<SPDirectedGraph>(2);
    graph->addEdge(0, 1);
    graph->addEdge(1, 0);
    DirectedGraphHierarchicalLayout layout;
    layout.setGraph(graph);
    const auto [xs, ys] = layout.layoutGraph();
    EXPECT_EQ(xs, vector({0.0, 0.0}));
    EXPECT_EQ(ys, vector({0.0, UNIT_SIZE}));
    EXPECT_EQ(graph->numEdges(), 2);
    EXPECT_EQ(graph->edges()[0].u, 0);
    EXPECT_EQ(graph->edges()[0].v, 1);
    EXPECT_EQ(graph->edges()[1].u, 1);
    EXPECT_EQ(graph->edges()[1].v, 0);
}

TEST(TestDirectedGraphHierarchialLayout, SpecialCase1) {
    const auto graph = make_shared<SPDirectedGraph>(4);
    graph->addEdge(0, 1); graph->addEdge(0, 2); graph->addEdge(0, 3);
    graph->addEdge(1, 3); graph->addEdge(2, 3);
    graph->addEdge(3, 0);
    DirectedGraphHierarchicalLayout layout;
    layout.setGraph(graph);
    const auto [xs, ys] = layout.layoutGraph();
    EXPECT_EQ(xs, vector({UNIT_SIZE * 1.5, 0.0, UNIT_SIZE, UNIT_SIZE * 1.5, UNIT_SIZE * 2, UNIT_SIZE * 3}));
    EXPECT_EQ(ys, vector({0.0, UNIT_SIZE, UNIT_SIZE, UNIT_SIZE * 2, UNIT_SIZE, UNIT_SIZE}));
    EXPECT_EQ(graph->numEdges(), 8);
    layout.setVertexLabels({"A", "B", "C", "D"});
    layout.drawSVG("test_directed_graph_hierarchical_layout__special_case_1.svg");
}

TEST(TestDirectedGraphHierarchialLayout, SpecialCase2) {
    auto test = [](const GraphAttributes::Rank rank) {
        const auto graph = make_shared<SPDirectedGraph>(23);
        const vector<vector<int>> edges = {
            {3, 4, 10, 13}, {3, 20}, {4, 5}, {6}, {7},
            {8, 16, 23}, {9}, {10, 11}, {12}, {13, 14, 15},
            {15, 16}, {20}, {17}, {17, 18}, {},
            {18, 19, 20}, {}, {21}, {22}, {},
            {23}, {23},
        };
        for (int u = 0; u < edges.size(); ++u) {
            for (const auto v : edges[u]) {
                graph->addEdge(u, v - 1);
            }
        }
        DirectedGraphHierarchicalLayout layout;
        layout.graphAttributes().bgcolor.set("white");
        layout.graphAttributes().rank = rank;
        layout.setVertexAttributes(14, ATTRIBUTE_KEY_SHAPE, "doublecircle");
        layout.setVertexAttributes(16, ATTRIBUTE_KEY_SHAPE, "doublecircle");
        layout.setVertexAttributes(19, ATTRIBUTE_KEY_SHAPE, "doublecircle");
        layout.setVertexAttributes(22, ATTRIBUTE_KEY_SHAPE, "doublecircle");
        layout.setEdgeAttributes(5, ATTRIBUTE_KEY_LABEL, "边");
        layout.setGraph(graph);
        layout.layoutGraph();
        layout.initializeVertexLabelsWithNumericalValues();
        layout.drawSVG(format("test_directed_graph_hierarchical_layout__special_case_2__{}.svg", static_cast<int>(rank)));
    };
    test(GraphAttributes::Rank::TopToBottom);
    test(GraphAttributes::Rank::BottomToTop);
    test(GraphAttributes::Rank::LeftToRight);
    test(GraphAttributes::Rank::RightToLeft);
}

TEST(TestDirectedGraphHierarchialLayout, SpecialCase3) {
    const auto graph = make_shared<SPDirectedGraph>(12);
    const vector<vector<int>> edges = {
        {0}, {1, 10}, {2, 4}, {3}, {6},
        {5}, {6}, {7, 9},  {8}, {7, 9},
        {1, 10}
    };
    for (int u = 0; u < edges.size(); ++u) {
        for (const auto v : edges[u]) {
            graph->addEdge(u, v + 1);
        }
    }
    DirectedGraphHierarchicalLayout layout;
    layout.setFeedbackArcsMethod(FeedbackArcsMethod::MIN_ID);
    layout.graphAttributes().bgcolor.set("white");
    layout.graphAttributes().rank = GraphAttributes::Rank::LeftToRight;
    layout.setVertexAttributes(0, ATTRIBUTE_KEY_LABEL, "start");
    for (int u = 1; u < 12; ++u) {
        layout.setVertexAttributes(u, ATTRIBUTE_KEY_LABEL, format("{}", u - 1));
    }
    layout.setVertexAttributes(0, ATTRIBUTE_KEY_SHAPE, "none");
    layout.setVertexAttributes(11, ATTRIBUTE_KEY_SHAPE, "doublecircle");
    layout.edgeAttributes().label = "ϵ";
    for (const auto& [id, u, v] : graph->edges()) {
        if (u == 5 && v == 6) {
            layout.setEdgeAttributes(id, ATTRIBUTE_KEY_LABEL, "a");
        }
        if (u == 8 && v == 9) {
            layout.setEdgeAttributes(id, ATTRIBUTE_KEY_LABEL, "b");
        }
    }
    layout.setGraph(graph);
    layout.layoutGraph();
    layout.drawSVG("test_directed_graph_hierarchical_layout__special_case_3.svg");
}

TEST(TestDirectedGraphHierarchialLayout, SpecialCase4) {
    auto test = [](const GraphAttributes::Rank rank) {
        const auto graph = make_shared<SPDirectedGraph>(5);
        graph->addEdge(0, 1);
        graph->addEdge(1, 1); graph->addEdge(1, 2);
        graph->addEdge(2, 2); graph->addEdge(2, 3);
        graph->addEdge(3, 2); graph->addEdge(3, 4);
        graph->addEdge(4, 4);
        DirectedGraphHierarchicalLayout layout;
        layout.setFeedbackArcsMethod(FeedbackArcsMethod::MIN_ID);
        layout.graphAttributes().bgcolor.set("white");
        layout.graphAttributes().rank = rank;
        layout.setVertexAttributes(0, ATTRIBUTE_KEY_LABEL, "start");
        for (int u = 1; u <= 4; ++u) {
            layout.setVertexAttributes(u, ATTRIBUTE_KEY_LABEL, format("{}", u));
        }
        layout.setVertexAttributes(0, ATTRIBUTE_KEY_SHAPE, "none");
        layout.setVertexAttributes(4, ATTRIBUTE_KEY_SHAPE, "doublecircle");
        layout.setEdgeAttributes(1, ATTRIBUTE_KEY_LABEL, "b");
        layout.setEdgeAttributes(2, ATTRIBUTE_KEY_LABEL, "a");
        layout.setEdgeAttributes(3, ATTRIBUTE_KEY_LABEL, "a");
        layout.setEdgeAttributes(4, ATTRIBUTE_KEY_LABEL, "b");
        layout.setEdgeAttributes(5, ATTRIBUTE_KEY_LABEL, "a");
        layout.setEdgeAttributes(6, ATTRIBUTE_KEY_LABEL, "b");
        layout.setEdgeAttributes(7, ATTRIBUTE_KEY_LABEL, "a,b");
        layout.setGraph(graph);
        layout.layoutGraph();
        layout.drawSVG(format("test_directed_graph_hierarchical_layout__special_case_4__{}.svg", static_cast<int>(rank)));
    };
    test(GraphAttributes::Rank::TopToBottom);
    test(GraphAttributes::Rank::BottomToTop);
    test(GraphAttributes::Rank::LeftToRight);
    test(GraphAttributes::Rank::RightToLeft);
}

TEST(TestDirectedGraphHierarchialLayout, RandomNoCheck) {
    const RandomSimpleDirectedGraphGenerator graphGen(128);
    for (size_t caseIndex = 0; caseIndex < 128; ++caseIndex) {
        auto graph = graphGen.generateRandomGraph();
        DirectedGraphHierarchicalLayout layout;
        auto graphPtr = make_shared<SPDirectedGraph>(graph);
        layout.setGraph(graphPtr);
        const auto [xs, ys] = layout.layoutGraph();
        EXPECT_EQ(xs.size(), graphPtr->numVertices());
        EXPECT_EQ(ys.size(), graphPtr->numVertices());
    }
}

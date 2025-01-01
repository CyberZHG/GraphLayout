#include <random>
#include <unordered_set>
#include <format>
#include <gtest/gtest.h>
#include "graph_layout.h"
using namespace std;
using namespace graph_layout;

TEST(TestDirectedGraphHierarchialLayout, EmptyGraph) {
    const auto graph = make_shared<SPDirectedGraph>(0);
    DirectedGraphHierarchicalLayout layout;
    layout.setGraph(graph);
    layout.layoutGraph();
}

TEST(TestDirectedGraphHierarchialLayout, SingleVertexSelfCycle) {
    const auto graph = make_shared<SPDirectedGraph>(1);
    graph->addEdge(0, 0);
    DirectedGraphHierarchicalLayout layout;
    layout.setGraph(graph);
    layout.layoutGraph();
    EXPECT_EQ(graph->numEdges(), 1);
    EXPECT_EQ(graph->edges()[0].u, 0);
    EXPECT_EQ(graph->edges()[0].v, 0);
}

TEST(TestDirectedGraphHierarchialLayout, SpecialCase1) {
    const auto graph = make_shared<SPDirectedGraph>(4);
    graph->addEdge(0, 1); graph->addEdge(0, 2); graph->addEdge(0, 3);
    graph->addEdge(1, 3); graph->addEdge(2, 3);
    graph->addEdge(3, 0);
    DirectedGraphHierarchicalLayout layout;
    layout.setGraph(graph);
    layout.setVertexLabels({"A", "BB", "CCC", "DD\nDD"});
    layout.layoutGraph();
    EXPECT_EQ(graph->numEdges(), 8);
    layout.render("test_directed_graph_hierarchical_layout__special_case_1.svg");
}

TEST(TestDirectedGraphHierarchialLayout, SpecialCase2) {
    auto test = [](const string& rankDir) {
        const auto graph = make_shared<SPDirectedGraph>(23);
        const vector<vector<int>> edges = {
            {3, 4, 10, 13}, {3, 20}, {4, 5}, {6}, {7},
            {8, 16, 23}, {9}, {10, 11}, {12}, {13, 14, 15},
            {15, 16}, {20}, {17}, {17, 18}, {},
            {18, 19, 20}, {}, {21}, {22}, {},
            {23}, {23},
        };
        for (int u = 0; u < static_cast<int>(edges.size()); ++u) {
            for (const auto v : edges[u]) {
                graph->addEdge(u, v - 1);
            }
        }
        DirectedGraphHierarchicalLayout layout;
        layout.attributes().setGraphAttributes(ATTRIBUTE_KEY_BG_COLOR, "white");
        layout.attributes().setRankDir(rankDir);
        layout.attributes().setVertexAttributes(14, ATTRIBUTE_KEY_SHAPE, "doublecircle");
        layout.attributes().setVertexAttributes(16, ATTRIBUTE_KEY_SHAPE, "doublecircle");
        layout.attributes().setVertexAttributes(19, ATTRIBUTE_KEY_SHAPE, "doublecircle");
        layout.attributes().setVertexAttributes(22, ATTRIBUTE_KEY_SHAPE, "doublecircle");
        layout.attributes().setEdgeAttributes(5, ATTRIBUTE_KEY_LABEL, "边");
        layout.setGraph(graph);
        layout.initVertexLabelsWithNumericalValues();
        layout.layoutGraph();
        layout.render(format("test_directed_graph_hierarchical_layout__special_case_2__{}.svg", rankDir));
    };
    test(AttributeRankDir::TOP_TO_BOTTOM);
    test(AttributeRankDir::BOTTOM_TO_TOP);
    test(AttributeRankDir::LEFT_TO_RIGHT);
    test(AttributeRankDir::RIGHT_TO_LEFT);
}

TEST(TestDirectedGraphHierarchialLayout, SpecialCase3) {
    const auto graph = make_shared<SPDirectedGraph>(12);
    const vector<vector<int>> edges = {
        {0}, {1, 10}, {2, 4}, {3}, {6},
        {5}, {6}, {7, 9},  {8}, {7, 9},
        {1, 10}
    };
    for (int u = 0; u < static_cast<int>(edges.size()); ++u) {
        for (const auto v : edges[u]) {
            graph->addEdge(u, v + 1);
        }
    }
    DirectedGraphHierarchicalLayout layout;
    layout.setFeedbackArcsMethod(FeedbackArcsMethod::MIN_ID);
        layout.attributes().setGraphAttributes(ATTRIBUTE_KEY_BG_COLOR, "white");
    layout.attributes().setRankDir(AttributeRankDir::LEFT_TO_RIGHT);
    layout.attributes().setVertexAttributes(0, ATTRIBUTE_KEY_LABEL, "start");
    for (int u = 1; u < 12; ++u) {
        layout.attributes().setVertexAttributes(u, ATTRIBUTE_KEY_LABEL, format("{}", u - 1));
    }
    layout.attributes().setVertexAttributes(0, ATTRIBUTE_KEY_SHAPE, AttributeShape::NONE);
    layout.attributes().setVertexAttributes(11, ATTRIBUTE_KEY_SHAPE, AttributeShape::DOUBLE_CIRCLE);
    for (const auto& [id, u, v] : graph->edges()) {
        if (u == 5 && v == 6) {
            layout.attributes().setEdgeAttributes(id, ATTRIBUTE_KEY_LABEL, "a");
        }
        if (u == 8 && v == 9) {
            layout.attributes().setEdgeAttributes(id, ATTRIBUTE_KEY_LABEL, "b");
        }
    }
    layout.setGraph(graph);
    layout.layoutGraph();
    layout.render("test_directed_graph_hierarchical_layout__special_case_3.svg");
}

TEST(TestDirectedGraphHierarchialLayout, SpecialCase4) {
    auto test = [](const string& rankDir) {
        const auto graph = make_shared<SPDirectedGraph>(5);
        graph->addEdge(0, 1);
        graph->addEdge(1, 1); graph->addEdge(1, 2);
        graph->addEdge(2, 2); graph->addEdge(2, 3);
        graph->addEdge(3, 2); graph->addEdge(3, 4);
        graph->addEdge(4, 4);
        DirectedGraphHierarchicalLayout layout;
        layout.setFeedbackArcsMethod(FeedbackArcsMethod::MIN_ID);
        layout.attributes().setGraphAttributes(ATTRIBUTE_KEY_BG_COLOR, "white");
        layout.attributes().setRankDir(rankDir);
        layout.attributes().setVertexAttributes(0, ATTRIBUTE_KEY_LABEL, "start");
        for (int u = 1; u <= 4; ++u) {
            layout.attributes().setVertexAttributes(u, ATTRIBUTE_KEY_LABEL, format("{}", u));
        }
        layout.attributes().setVertexAttributes(0, ATTRIBUTE_KEY_SHAPE, "none");
        layout.attributes().setVertexAttributes(4, ATTRIBUTE_KEY_SHAPE, "doublecircle");
        layout.attributes().setEdgeAttributes(1, ATTRIBUTE_KEY_LABEL, "b");
        layout.attributes().setEdgeAttributes(2, ATTRIBUTE_KEY_LABEL, "a");
        layout.attributes().setEdgeAttributes(3, ATTRIBUTE_KEY_LABEL, "a");
        layout.attributes().setEdgeAttributes(4, ATTRIBUTE_KEY_LABEL, "b");
        layout.attributes().setEdgeAttributes(5, ATTRIBUTE_KEY_LABEL, "a");
        layout.attributes().setEdgeAttributes(6, ATTRIBUTE_KEY_LABEL, "b");
        layout.attributes().setEdgeAttributes(7, ATTRIBUTE_KEY_LABEL, "a,b");
        layout.setGraph(graph);
        layout.layoutGraph();
        layout.render(format("test_directed_graph_hierarchical_layout__special_case_4__{}.svg", rankDir));
    };
    test(AttributeRankDir::TOP_TO_BOTTOM);
    test(AttributeRankDir::BOTTOM_TO_TOP);
    test(AttributeRankDir::LEFT_TO_RIGHT);
    test(AttributeRankDir::RIGHT_TO_LEFT);
}

#ifdef GRAPH_LAYOUT_ENABLE_RANDOM_TESTS
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
#endif

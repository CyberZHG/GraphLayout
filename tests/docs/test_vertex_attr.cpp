#include <random>
#include <unordered_set>
#include <format>
#include <vector>
#include <gtest/gtest.h>
#include "graph_layout.h"
using namespace graph_layout;

static const auto OUTPUT_DIR = std::string("../docs/source/_static/images/vertex_attr/");

TEST(TestDocsVertexAttributes, Shape) {
    DirectedGraphHierarchicalLayout layout;
    const auto graph = layout.createGraph(3);
    graph->addEdges({{0, 1}, {1, 2}, {2, 0}});
    layout.attributes().setVertexShape(0, AttributeShape::DOUBLE_CIRCLE);
    layout.attributes().setVertexShape(1, AttributeShape::RECT);
    layout.attributes().setVertexShape(2, AttributeShape::ELLIPSE);
    layout.attributes().setRankDir(AttributeRankDir::LEFT_TO_RIGHT);
    layout.initVertexLabelsWithNumericalValues(0);
    layout.layoutGraph();
    layout.render(OUTPUT_DIR + "shape.svg");
}

TEST(TestDocsVertexAttributes, Label) {
    DirectedGraphHierarchicalLayout layout;
    const auto graph = layout.createGraph(3);
    graph->addEdges({{0, 1}, {1, 2}, {2, 0}});
    layout.setVertexLabels({"Eat", "Sleep", "Play"});
    for (int u = 0; u < 3; ++u) {
        layout.attributes().setVertexShape(u, AttributeShape::ELLIPSE);
    }
    layout.attributes().setRankDir(AttributeRankDir::LEFT_TO_RIGHT);
    layout.layoutGraph();
    layout.render(OUTPUT_DIR + "label.svg");
}
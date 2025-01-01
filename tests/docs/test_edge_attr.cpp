#include <random>
#include <unordered_set>
#include <format>
#include <vector>
#include <gtest/gtest.h>
#include "graph_layout.h"
using namespace graph_layout;

static const auto OUTPUT_DIR = std::string("../docs/source/_static/images/edge_attr/");

TEST(TestDocsEdgeAttributes, Label) {
    DirectedGraphHierarchicalLayout layout;
    const auto graph = layout.createGraph(3);
    graph->addEdges({{0, 1}, {1, 2}, {2, 2}});
    layout.initVertexLabelsWithNumericalValues(0);
    layout.setEdgeLabel(0, "A");
    layout.setEdgeLabel(1, "B");
    layout.setEdgeLabel(2, "C");
    layout.attributes().setRankDir(AttributeRankDir::LEFT_TO_RIGHT);
    layout.layoutGraph();
    layout.render(OUTPUT_DIR + "label.svg");
}
#include <random>
#include <unordered_set>
#include <format>
#include <vector>
#include <gtest/gtest.h>
#include "graph_layout.h"
using namespace graph_layout;

static const auto OUTPUT_DIR = std::string("../docs/source/_static/images/basic/");

TEST(TestDocsBasic, Directed) {
    DirectedGraphHierarchicalLayout layout;
    const auto graph = layout.createGraph(8);
    graph->addEdges({
        {0, 1}, {1, 2}, {1, 3}, {2, 5}, {2, 6},
        {3, 4}, {4, 5}, {4, 6}, {5, 7}, {6, 7},
        {7, 0},
    });
    layout.initVertexLabelsWithNumericalValues(0);
    layout.layoutGraph();
    layout.render(OUTPUT_DIR + "directed.svg");
}
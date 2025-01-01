#include <random>
#include <unordered_set>
#include <format>
#include <vector>
#include <gtest/gtest.h>
#include "graph_layout.h"
using namespace graph_layout;

static const auto OUTPUT_DIR = std::string("../docs/source/_static/images/graph_attr/");

TEST(TestDocsGraphAttributes, RankDir) {
    const auto rankDirs = {
        AttributeRankDir::TOP_TO_BOTTOM,
        AttributeRankDir::BOTTOM_TO_TOP,
        AttributeRankDir::LEFT_TO_RIGHT,
        AttributeRankDir::RIGHT_TO_LEFT,
    };
    for (const auto& rankDir : rankDirs) {
        DirectedGraphHierarchicalLayout layout;
        const auto graph = layout.createGraph(13);
        graph->addEdges({
            {0, 1}, {1, 2}, {1, 3}, {2, 5}, {2, 6},
            {3, 4}, {4, 5}, {4, 6}, {5, 7}, {6, 7},
            {7, 0}, {1, 0},
            {8, 8}, {8, 9}, {8, 10}, {9, 12}, {10, 12},
            {11, 12}
        });
        layout.initVertexLabelsWithNumericalValues(0);
        layout.attributes().setRankDir(rankDir);
        layout.layoutGraph();
        layout.render(OUTPUT_DIR + std::format("rank_dir_{}.svg", rankDir));
    }
}
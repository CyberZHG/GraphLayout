#include <unordered_set>
#include <gtest/gtest.h>
#include "graph_def.h"
using namespace std;
using namespace graph_layout;

TEST(TestGraphComponentSplitter, EmptyGraph) {
    SPDirectedGraph graph(0);
    GraphComponentSplitter splitter;
    const auto &graphs = splitter.splitGraph(graph);
    EXPECT_EQ(graphs.size(), 0);
}

TEST(TestGraphComponentSplitter, SingleNode) {
    SPDirectedGraph graph(1);
    GraphComponentSplitter splitter;
    const auto &graphs = splitter.splitGraph(graph);
    EXPECT_EQ(graphs.size(), 1);
}

TEST(TestGraphComponentSplitter, SpecialCase1) {
    SPDirectedGraph graph(4);
    graph.addEdge(0, 2);
    graph.addEdge(1, 3);
    GraphComponentSplitter splitter;
    const auto &graphs = splitter.splitGraph(graph);
    EXPECT_EQ(graphs.size(), 2);
    EXPECT_EQ(graphs[0].numEdges(), 1);
    EXPECT_EQ(graphs[1].numEdges(), 1);
    EXPECT_EQ(graphs[0].edges()[0], SPEdge({0, 0, 1}));
    EXPECT_EQ(graphs[1].edges()[0], SPEdge({1, 0, 1}));
    const auto newGraph = splitter.mergeBack();
    EXPECT_EQ(newGraph, graph);
}

TEST(TestGraphComponentSplitter, Random) {
    const RandomSimpleDirectedGraphGenerator graphGen(128);
    GraphComponentSplitter splitter;
    for (size_t caseIndex = 0; caseIndex < 128; ++caseIndex) {
        auto graph = graphGen.generateRandomGraph();
        splitter.splitGraph(graph);
        auto newGraph = splitter.mergeBack();
        EXPECT_EQ(newGraph, graph);
    }
}
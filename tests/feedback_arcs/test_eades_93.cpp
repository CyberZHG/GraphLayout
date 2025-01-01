#include <unordered_set>
#include <gtest/gtest.h>
#include "feedback_arcs.h"
#include "graph_def.h"
#include "graph_def_utils.h"
#include "../include/graph_def_utils.h"
using namespace std;
using namespace graph_layout;

TEST(TestFeedbackArcsEades93, EmptyGraph) {
    SimpleDirectedGraph graph(0);
    EXPECT_FALSE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set<int>());
}

TEST(TestFeedbackArcsEades93, SingleNodeNoEdge) {
    SimpleDirectedGraph graph(1);
    EXPECT_FALSE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set<int>());
}

TEST(TestFeedbackArcsEades93, SingleEdge) {
    SimpleDirectedGraph graph(2);
    graph.addEdge(0, 1);
    EXPECT_FALSE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set<int>());
}

TEST(TestFeedbackArcsEades93, TwoNodesCycle) {
    SimpleDirectedGraph graph(2);
    graph.addEdge(0, 1);
    graph.addEdge(1, 0);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({1}));
}

TEST(TestFeedbackArcsEades93, ThreeNodesCycle) {
    SimpleDirectedGraph graph(3);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 0);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({2}));
}

TEST(TestFeedbackArcsEades93, ParallelEdges) {
    SimpleDirectedGraph graph(2);
    graph.addEdge(0, 1);
    graph.addEdge(0, 1);
    graph.addEdge(1, 0);
    graph.addEdge(1, 0);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({2, 3}));
}

TEST(TestFeedbackArcsEades93, SpecialCase1) {
    SimpleDirectedGraph graph(12);
    graph.addOutEdges(0, {1, 2, 7});
    graph.addEdge(1, 3);
    graph.addEdge(2, 3);
    graph.addOutEdges(3, {4, 5, 6});
    graph.addOutEdges(4, {5, 6});
    graph.addOutEdges(5, {1, 9});
    graph.addOutEdges(6, {2, 9});
    graph.addEdge(7, 8);
    graph.addEdge(8, 11);
    graph.addEdge(9, 11);
    graph.addEdge(10, 11);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({3, 4}));
}

TEST(TestFeedbackArcsEades93, SpecialCase2) {
    SimpleDirectedGraph graph(4);
    graph.addEdge(2, 2);
    graph.addEdge(2, 1);
    EXPECT_TRUE(graph.hasCycle());
    graph.disableSelfCycleEdges();
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set<int>({}));
}

TEST(TestFeedbackArcsEades93, Random) {
    const RandomGraphGenerator graphGen(128);
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    for (size_t caseIndex = 0; caseIndex < 128; ++caseIndex) {
        auto graph = graphGen.generateGraph();
        const bool hasCycle = graph.hasCycle();
        const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
        if (hasCycle) {
            graph.reverseEdges(feedbackArcs);
            EXPECT_FALSE(graph.hasCycle());
            graph.reverseEdgesBack();
            EXPECT_TRUE(graph.hasCycle());
        } else {
            EXPECT_EQ(feedbackArcs, unordered_set<int>());
        }
    }
}

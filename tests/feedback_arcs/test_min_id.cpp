#include <unordered_set>
#include <gtest/gtest.h>
#include "directed/feedback_arcs.h"
using namespace std;
using namespace graph_layout;

TEST(TestFeedbackArcsMinID, EmptyGraph) {
    SPDirectedGraph graph(0);
    EXPECT_FALSE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::MIN_ID);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set<int>());
}

TEST(TestFeedbackArcsMinID, SingleNodeNoEdge) {
    SPDirectedGraph graph(1);
    EXPECT_FALSE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::MIN_ID);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set<int>());
}

TEST(TestFeedbackArcsMinID, SingleEdge) {
    SPDirectedGraph graph(2);
    graph.addEdge(0, 1);
    EXPECT_FALSE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::MIN_ID);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set<int>());
}

TEST(TestFeedbackArcsMinID, TwoNodesCycle) {
    SPDirectedGraph graph(2);
    graph.addEdge(0, 1);
    graph.addEdge(1, 0);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::MIN_ID);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({1}));
}

TEST(TestFeedbackArcsMinID, ThreeNodesCycle) {
    SPDirectedGraph graph(3);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 0);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::MIN_ID);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({2}));
}

TEST(TestFeedbackArcsMinID, ParallelEdges) {
    SPDirectedGraph graph(2);
    graph.addEdge(0, 1);
    graph.addEdge(0, 1);
    graph.addEdge(1, 0);
    graph.addEdge(1, 0);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::MIN_ID);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({2, 3}));
}

#ifdef GRAPH_LAYOUT_BUILD_RANDOM_TESTS
TEST(TestFeedbackArcsMinID, Random) {
    const RandomSimpleDirectedGraphGenerator graphGen(128);
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::MIN_ID);
    for (size_t caseIndex = 0; caseIndex < 128; ++caseIndex) {
        auto graph = graphGen.generateRandomGraph();
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
#endif

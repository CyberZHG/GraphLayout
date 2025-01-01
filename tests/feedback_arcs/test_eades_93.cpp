#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "feedback_arcs.h"
#include "graph_def.h"
using namespace std;
using namespace graph_layout;

TEST(TestFeedbackArcsEades93, EmptyGraph) {
    SimpleGraph graph(0);
    EXPECT_FALSE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set<int>());
}

TEST(TestFeedbackArcsEades93, SingleNodeNoEdge) {
    SimpleGraph graph(1);
    EXPECT_FALSE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set<int>());
}

TEST(TestFeedbackArcsEades93, SingleNodeSelfCycle) {
    SimpleGraph graph(1);
    graph.addEdge(0, 0);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({0}));
}

TEST(TestFeedbackArcsEades93, SingleEdge) {
    SimpleGraph graph(2);
    graph.addEdge(0, 1);
    EXPECT_FALSE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set<int>());
}

TEST(TestFeedbackArcsEades93, TwoNodesCycle) {
    SimpleGraph graph(2);
    graph.addEdge(0, 1);
    graph.addEdge(1, 0);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({1}));
}

TEST(TestFeedbackArcsEades93, ThreeNodesCycle) {
    SimpleGraph graph(3);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 0);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({2}));
}

TEST(TestFeedbackArcsEades93, ParallelEdges) {
    SimpleGraph graph(2);
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
    SimpleGraph graph(12);
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
    SimpleGraph graph(4);
    graph.addEdge(2, 2);
    graph.addEdge(2, 1);
    EXPECT_TRUE(graph.hasCycle());
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
    EXPECT_EQ(feedbackArcs, unordered_set({0}));
}

TEST(TestFeedbackArcsEades93, Random) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> numVerticesDist(2, 128);
    const FeedbackArcsFinder feedbackArcsFinder(FeedbackArcsMethod::EADES_93);
    for (size_t caseIndex = 0; caseIndex < 1024; ++caseIndex) {
        const size_t n = numVerticesDist(gen);
        SimpleGraph graph(n);
        uniform_int_distribution<> numEdgesDist(0, static_cast<int>(n * n));
        uniform_int_distribution<> verticeIndexDist(0, static_cast<int>(n - 1));
        const size_t m = numEdgesDist(gen);
        for (size_t edgeIndex = 0; edgeIndex < m; ++edgeIndex) {
            const int &u = verticeIndexDist(gen);
            const int &v = verticeIndexDist(gen);
            graph.addEdge(u, v);
        }
        const bool hasCycle = graph.hasCycle();
        const auto feedbackArcs = feedbackArcsFinder.findFeedbackArcs(graph);
        if (hasCycle) {
            SimpleGraph newGraph(n);
            unordered_set feedBackArcIds(feedbackArcs.begin(), feedbackArcs.end());
            for (const auto &edge : graph.edges()) {
                if (!feedBackArcIds.contains(edge.id)) {
                    newGraph.addEdge(edge);
                }
            }
            EXPECT_FALSE(newGraph.hasCycle());
        } else {
            EXPECT_EQ(feedbackArcs, unordered_set<int>());
        }
    }
}

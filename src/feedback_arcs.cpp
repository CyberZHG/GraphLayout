#include <queue>
#include "feedback_arcs.h"
using namespace std;
using namespace graph_layout;

FeedbackArcsFinder::FeedbackArcsFinder(const FeedbackArcsMethod method) : _method(method) {
}

unordered_set<int> FeedbackArcsFinder::findFeedbackArcs(SimpleDirectedGraph &graph) const {
    switch (_method) {
        case FeedbackArcsMethod::EADES_93:
            return findFeedbackArcsEades93(graph);
    }
    return {};
}

unordered_set<int> FeedbackArcsFinder::findFeedbackArcsEades93(SimpleDirectedGraph &graph) {
    const auto n = graph.numVertices();
    auto &edges = graph.edges();
    const auto m = edges.size();
    vector inDegree(graph.getInDegrees());
    vector outDegree(graph.getOutDegrees());
    // Group edges by outdegree minus indegree.
    // so that we can find the edge with the largest difference in amortized O(1) time complexity.
    int bucketUpperBound = static_cast<int>(m) * 2;
    vector<vector<int>> differenceBuckets(bucketUpperBound + 1);
    vector<pair<size_t, size_t>> bucketIndices(n);
    auto removeFromDifferenceBuckets = [&](const int u) {
        auto [bucketIndex, vectorIndex] = bucketIndices[u];
        const auto lastElement = differenceBuckets[bucketIndex][differenceBuckets[bucketIndex].size() - 1];
        differenceBuckets[bucketIndex][vectorIndex] = lastElement;
        differenceBuckets[bucketIndex].pop_back();
        bucketIndices[lastElement].second = vectorIndex;
    };
    auto addToDifferenceBuckets = [&](const int u) {
        size_t index = outDegree[u] - inDegree[u] + m;
        bucketIndices[u] = {index, differenceBuckets[index].size()};
        differenceBuckets[index].emplace_back(u);
        bucketUpperBound = max(bucketUpperBound, static_cast<int>(index));
    };
    queue<int> sources, sinks;
    for (int i = 0; i < n; ++i) {
        if (outDegree[i] == 0) {
            sinks.emplace(i);
        }
        if (inDegree[i] == 0) {
            sources.emplace(i);
        }
        addToDifferenceBuckets(i);
    }
    int num_popped_vertices = 0;
    vector<bool> popped(n);
    auto &edgeIdToIndexMap = graph.getEdgeIdToIndexMap();
    auto &inEdges = graph.getInEdges();
    auto &outEdges = graph.getOutEdges();
    auto removeNode = [&](const int u) {
        if (popped[u]) {
            return;
        }
        ++num_popped_vertices;
        popped[u] = true;
        removeFromDifferenceBuckets(u);
        for (const auto id : inEdges[u]) {
            const int v = edges[edgeIdToIndexMap.find(id)->second].u;
            if (!popped[v]) {
                if (--outDegree[v] == 0) {
                    sinks.push(v);
                }
                removeFromDifferenceBuckets(v);
                addToDifferenceBuckets(v);
            }
        }
        for (const auto id : outEdges[u]) {
            const int v = edges[edgeIdToIndexMap.find(id)->second].v;
            if (!popped[v]) {
                if (--inDegree[v] == 0) {
                    sources.push(v);
                }
                removeFromDifferenceBuckets(v);
                addToDifferenceBuckets(v);
            }
        }
    };
    // In each iteration, we first remove all source and sink nodes,
    // and then break cycles by reversing all incoming edges of the node
    // with the largest outdegree minus indegree.
    unordered_set<int> feedbackArcs;
    while (num_popped_vertices < n) {
        while (!sinks.empty()) {
            removeNode(sinks.front());
            sinks.pop();
        }
        while (!sources.empty()) {
            removeNode(sources.front());
            sources.pop();
        }
        if (num_popped_vertices < n) {
            while (bucketUpperBound >= 0) {
                if (differenceBuckets[bucketUpperBound].empty()) {
                    --bucketUpperBound;
                } else {
                    const int u = differenceBuckets[bucketUpperBound][0];
                    removeNode(u);
                    for (const auto id : inEdges[u]) {
                        if (const int v = edges[edgeIdToIndexMap.find(id)->second].u; !popped[v]) {
                            feedbackArcs.emplace(id);
                        }
                    }
                    break;
                }
            }
        }
    }
    return feedbackArcs;
}

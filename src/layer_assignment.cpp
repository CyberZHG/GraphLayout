#include <queue>
#include "layer_assignment.h"
using namespace std;
using namespace graph_layout;

LayerAssignment::LayerAssignment(const LayerAssignmentMethod method) : _method(method) {
}

vector<int> LayerAssignment::rankVertices(SimpleDirectedGraph &graph) const {
    switch (_method) {
        case LayerAssignmentMethod::TOPOLOGICAL:
            return rankVerticesTopological(graph);
    }
    return {};
}

long long LayerAssignment::calcRankCost(const SimpleDirectedGraph &graph, const std::vector<int> &ranks) {
    long long cost = 0;
    for (const auto &edge : graph.edges()) {
        cost += ranks[edge.v] - ranks[edge.u];
    }
    return cost;
}

std::vector<int> LayerAssignment::rankVerticesTopological(SimpleDirectedGraph &graph) {
    const size_t n = graph.numVertices();
    const auto edges = graph.edges();
    std::vector inDegrees(graph.getInDegrees());
    queue<int> q;
    std::vector<int> ranks(n);
    for (size_t i = 0; i < n; i++) {
        if (inDegrees[i] == 0) {
            q.emplace(i);
            ranks[i] = 0;
        }
    }
    while (!q.empty()) {
        const auto u = q.front();
        q.pop();
        for (const auto &edge : graph.getOutEdges(u)) {
            const int v = edge.v;
            ranks[v] = max(ranks[v], ranks[u] + 1);
            if (--inDegrees[v] == 0) {
                q.push(v);
            }
        }
    }
    return ranks;
}

#include <queue>
#include <utility>
#include "layer_assignment.h"
using namespace std;
using namespace graph_layout;

LayerAssignment::LayerAssignment(const LayerAssignmentMethod method) : _method(method) {
}

void LayerAssignment::setMinimumEdgeLengths(unordered_map<int, int> &&minEdgeLens) {
    _minEdgeLens = std::move(minEdgeLens);
}

void LayerAssignment::cleanMinimumEdgeLengths() {
    _minEdgeLens = unordered_map<int, int>();
}

int LayerAssignment::minEdgeLength(const int id) const {
    if (const auto it = _minEdgeLens.find(id); it != _minEdgeLens.end()) {
        return it->second;
    }
    return 1;
}

vector<int> LayerAssignment::rankVertices(SimpleDirectedGraph &graph) const {
    switch (_method) {
        case LayerAssignmentMethod::TOPOLOGICAL:
        case LayerAssignmentMethod::MIN_NUM_OF_LAYERS:
            return rankVerticesTopological(graph);
        case LayerAssignmentMethod::NETWORK_SIMPLEX:
        case LayerAssignmentMethod::MIN_TOTAL_EDGE_LENGTH:
            return rankVerticesNetworkSimplex(graph);
    }
    return {};
}

long long LayerAssignment::calcTotalEdgeLength(const SimpleDirectedGraph &graph, const vector<int> &ranks) {
    long long cost = 0;
    for (const auto &edge : graph.edges()) {
        cost += ranks[edge.v] - ranks[edge.u];
    }
    return cost;
}

vector<int> LayerAssignment::rankVerticesTopological(SimpleDirectedGraph &graph) const {
    const size_t n = graph.numVertices();
    const auto edges = graph.edges();
    vector inDegrees(graph.getInDegrees());
    queue<int> q;
    vector<int> ranks(n);
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
            ranks[v] = max(ranks[v], ranks[u] + minEdgeLength(edge.id));
            if (--inDegrees[v] == 0) {
                q.push(v);
            }
        }
    }
    return ranks;
}

vector<int> LayerAssignment::rankVerticesNetworkSimplex(SimpleDirectedGraph &graph) const {
    vector<int> ranks(graph.getInDegrees());
    return ranks;
}
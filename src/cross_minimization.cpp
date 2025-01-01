#include "cross_minimization.h"
using namespace std;
using namespace graph_layout;

CrossMinimization::CrossMinimization(const CrossMinimizationMethod method) : _method(method) {
}

vector<SPVirtualEdge> CrossMinimization::addVirtualEdges(SPDirectedGraph &graph, vector<int> &ranks) {
    int n = static_cast<int>(graph.numVertices());
    vector<SPVirtualEdge> virtualEdges;
    for (const auto &edge : graph.edges()) {
        if (ranks[edge.v] - ranks[edge.u] > 1) {
            virtualEdges.emplace_back(edge);
        }
    }
    int numNewNodes = 0;
    for (const auto &[originalEdge, virtualEdgeIds] : virtualEdges) {
        graph.removeEdge(originalEdge.id);
        numNewNodes += ranks[originalEdge.v] - ranks[originalEdge.u] - 1;
    }
    graph.updateNumVertices(n + numNewNodes);
    int edgeId = VIRTUAL_EDGE_ID_OFFSET;
    for (auto &[originalEdge, virtualEdgeIds] : virtualEdges) {
        const int u = originalEdge.u;
        const int v = originalEdge.v;
        int last = u;
        for (int r = ranks[u] + 1; r < ranks[v]; ++r) {
            graph.addEdge(SPEdge(edgeId, last, n));
            virtualEdgeIds.emplace_back(edgeId++);
            ranks.emplace_back(r);
            last = n++;
        }
        graph.addEdge(SPEdge(edgeId, last, v));
        virtualEdgeIds.emplace_back(edgeId++);
    }
    return virtualEdges;
}

#include <ranges>
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

std::vector<std::vector<int>> CrossMinimization::getInitialLayers(const std::vector<int> &ranks) {
    vector<vector<int>> layers;
    for (int i = 0; i < ranks.size(); ++i) {
        while (ranks[i] >= layers.size()) {
            layers.emplace_back();
        }
        layers[ranks[i]].emplace_back(i);
    }
    return layers;
}

long long CrossMinimization::calcNumCross(
    SPDirectedGraph &graph,
    BinaryIndexedTree &bit,
    const std::vector<int> &layer1,
    const std::vector<int> &layer2,
    const bool forward) {
    bit.clear();
    unordered_map<int, int> positions;
    for (int i = 0; i < layer1.size(); ++i) {
        positions[layer1[i]] = i;
    }
    long long numCross = 0;
    if (forward) {
        for (const int v : views::reverse(layer2)) {
            for (const auto &edge : graph.getInEdges(v)) {
                const int u = edge.u;
                if (const auto it = positions.find(u); it != positions.end()) {
                    numCross += bit.prefixSum(it->second - 1);
                }
            }
            for (const auto &edge : graph.getInEdges(v)) {
                const int u = edge.u;
                if (const auto it = positions.find(u); it != positions.end()) {
                    bit.add(it->second);
                }
            }
        }
    } else {
        for (const int v : views::reverse(layer2)) {
            for (const auto &edge : graph.getOutEdges(v)) {
                const int u = edge.v;
                if (const auto it = positions.find(u); it != positions.end()) {
                    numCross += bit.prefixSum(it->second - 1);
                }
            }
            for (const auto &edge : graph.getOutEdges(v)) {
                const int u = edge.v;
                if (const auto it = positions.find(u); it != positions.end()) {
                    bit.add(it->second);
                }
            }
        }
    }
    return numCross;
}

long long CrossMinimization::calcNumCross(SPDirectedGraph &graph, const vector<vector<int>> &layers) {
    size_t width = 0;
    for (const auto &layer : layers) {
        width = max(width, layer.size());
    }
    BinaryIndexedTree bit(width);
    long long numCross = 0;
    for (size_t i = 1; i < layers.size(); ++i) {
        numCross += calcNumCross(graph, bit, layers[i - 1], layers[i], true);
    }
    return numCross;
}

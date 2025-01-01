#include <ranges>
#include <set>
#include "cross_minimization.h"
using namespace std;
using namespace graph_layout;

CrossMinimization::CrossMinimization(const CrossMinimizationMethod method) : _method(method) {
}

std::pair<SPLayeredOrder, std::vector<SPVirtualEdge>> CrossMinimization::addVirtualEdges(SPDirectedGraph &graph, vector<int> &ranks) {
    SPLayeredOrder layeredOrder;
    auto &discreteRanks = layeredOrder.layerRanks;
    auto &orders = layeredOrder.orders;
    discreteRanks = vector(ranks);
    ranges::sort(discreteRanks);
    discreteRanks.erase(ranges::unique(discreteRanks).begin(), discreteRanks.end());
    unordered_map<int, int> rankToIndex;
    for (int i = 0; i < discreteRanks.size(); i++) {
        rankToIndex[discreteRanks[i]] = i;
    }
    orders.resize(discreteRanks.size());
    for (int i = 0; i < ranks.size(); ++i) {
        orders[rankToIndex[ranks[i]]].emplace_back(i);
    }

    int n = static_cast<int>(graph.numVertices());
    vector<SPVirtualEdge> virtualEdges;
    for (const auto &edge : graph.edges()) {
        if (rankToIndex[ranks[edge.v]] - rankToIndex[ranks[edge.u]] > 1) {
            virtualEdges.emplace_back(edge);
        }
    }
    int numNewNodes = 0;
    for (const auto &[edge, edgeId] : virtualEdges) {
        graph.removeEdge(edge.id);
        numNewNodes += rankToIndex[ranks[edge.v]] - rankToIndex[ranks[edge.u]] - 1;
    }
    graph.updateNumVertices(n + numNewNodes);
    int edgeId = VIRTUAL_EDGE_ID_OFFSET;
    for (auto &[originalEdge, virtualEdgeIds] : virtualEdges) {
        const int u = originalEdge.u;
        const int v = originalEdge.v;
        int last = u;
        for (int rankIndex = rankToIndex[ranks[u]] + 1; rankIndex < rankToIndex[ranks[v]]; ++rankIndex) {
            graph.addEdge(SPEdge(edgeId, last, n));
            virtualEdgeIds.emplace_back(edgeId++);
            ranks.emplace_back(discreteRanks[rankIndex]);
            orders[rankIndex].emplace_back(n);
            last = n++;
        }
        graph.addEdge(SPEdge(edgeId, last, v));
        virtualEdgeIds.emplace_back(edgeId++);
    }
    return {layeredOrder, virtualEdges};
}

long long CrossMinimization::calcNumCross(
    SPDirectedGraph &graph,
    BinaryIndexedTree &bit,
    const std::vector<int> &order1,
    const std::vector<int> &order2,
    const bool forward) {
    bit.clear();
    unordered_map<int, int> positions;
    for (int i = 0; i < order1.size(); ++i) {
        positions[order1[i]] = i;
    }
    long long numCross = 0;
    if (forward) {
        for (const int v : views::reverse(order2)) {
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
        for (const int v : views::reverse(order2)) {
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

long long CrossMinimization::calcNumCross(SPDirectedGraph &graph, const SPLayeredOrder &layeredOrder) {
    size_t width = 0;
    const auto &orders = layeredOrder.orders;
    for (const auto &order : orders) {
        width = max(width, order.size());
    }
    BinaryIndexedTree bit(width);
    long long numCross = 0;
    for (size_t i = 1; i < orders.size(); ++i) {
        numCross += calcNumCross(graph, bit, orders[i - 1], orders[i], true);
    }
    return numCross;
}

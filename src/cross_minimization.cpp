#include <ranges>
#include <set>
#include "cross_minimization.h"
using namespace std;
using namespace graph_layout;

CrossMinimization::CrossMinimization(const CrossMinimizationMethod method) : _method(method) {
}


std::pair<SPLayeredOrder, std::vector<SPVirtualEdge>> CrossMinimization::reduceNumCross(SPDirectedGraph &graph, std::vector<int> &ranks) const {
    auto [layeredOrder, virtualEdges] = addVirtualEdges(graph, ranks);
    switch (_method) {
        case CrossMinimizationMethod::BARYCENTER:
            reduceNumCrossWithBaryCenterHeuristic(graph, layeredOrder);
            break;
        case CrossMinimizationMethod::MEDIAN:
        case CrossMinimizationMethod::PAIRWISE_SWITCH:
            break;
    }
    return {layeredOrder, virtualEdges};
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

    layeredOrder.width = 0;
    for (const auto &order : orders) {
        layeredOrder.width = max(layeredOrder.width, order.size());
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
    const auto &orders = layeredOrder.orders;
    BinaryIndexedTree bit(layeredOrder.width);
    long long numCross = 0;
    for (size_t i = 1; i < orders.size(); ++i) {
        numCross += calcNumCross(graph, bit, orders[i - 1], orders[i], true);
    }
    return numCross;
}

void CrossMinimization::reduceNumCrossWithBaryCenterHeuristic(SPDirectedGraph &graph,
    SPLayeredOrder &layeredOrder) {
    constexpr int NUM_REPEAT = 2;

    const int numLayers = static_cast<int>(layeredOrder.layerRanks.size());
    auto &orders = layeredOrder.orders;

    vector<pair<double, pair<int, int>>> weights(layeredOrder.width);

    SPLayeredOrder bestLayeredOrder;
    bestLayeredOrder.width = 0;
    long long bestNumCross = calcNumCross(graph, layeredOrder);
    bool lastIsBest = false, hasUpdate = false;
    for (int repeatIndex = 0; repeatIndex < NUM_REPEAT; ++repeatIndex) {
        hasUpdate = false;
        for (int layerIndex = 1; layerIndex < numLayers; ++layerIndex) {
            unordered_map<int, int> positions;
            for (int i = 0; i < orders[layerIndex - 1].size(); ++i) {
                positions[orders[layerIndex - 1][i]] = i;
            }
            const int n = static_cast<int>(orders[layerIndex].size());
            for (int i = 0; i < n; ++i) {
                const int v = orders[layerIndex][i];
                weights[i] = {0.0, {i, v}};
                for (const auto &edge : graph.getInEdges(v)) {
                    weights[i].first += positions[edge.u];
                }
                weights[i].first /= graph.getInDegrees()[v];
            }
            sort(weights.begin(), weights.begin() + n);
            for (int i = 0; i < n; ++i) {
                const int v = weights[i].second.second;
                if (orders[layerIndex][i] != v) {
                    hasUpdate = true;
                }
                orders[layerIndex][i] = v;
            }
        }
        if (const long long numCross = calcNumCross(graph, layeredOrder); numCross < bestNumCross) {
            bestLayeredOrder = layeredOrder;
            bestNumCross = numCross;
        }

        for (int layerIndex = numLayers - 2; layerIndex >= 0; --layerIndex) {
            unordered_map<int, int> positions;
            for (int i = 0; i < orders[layerIndex + 1].size(); ++i) {
                positions[orders[layerIndex + 1][i]] = i;
            }
            const int n = static_cast<int>(orders[layerIndex].size());
            for (int i = 0; i < n; ++i) {
                const int u = orders[layerIndex][i];
                weights[i] = {0.0, {i, u}};
                for (const auto &edge : graph.getOutEdges(u)) {
                    weights[i].first += positions[edge.v];
                }
                weights[i].first /= graph.getOutDegrees()[u];
            }
            sort(weights.begin(), weights.begin() + n);
            for (int i = 0; i < n; ++i) {
                orders[layerIndex][i] = weights[i].second.second;
            }
            for (int i = 0; i < n; ++i) {
                const int u = weights[i].second.second;
                if (orders[layerIndex][i] != u) {
                    hasUpdate = true;
                }
                orders[layerIndex][i] = u;
            }

            if (!hasUpdate) {
                break;
            }
        }
        lastIsBest = false;
        if (const long long numCross = calcNumCross(graph, layeredOrder); numCross < bestNumCross) {
            if (repeatIndex + 1 != NUM_REPEAT) {
                bestLayeredOrder = layeredOrder;
            }
            bestNumCross = numCross;
            lastIsBest = true;
        }
    }
    if (bestLayeredOrder.width > 0 && !lastIsBest) {
        layeredOrder = bestLayeredOrder;
    }
}

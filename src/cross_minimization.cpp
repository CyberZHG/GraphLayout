#include <ranges>
#include <set>
#include "cross_minimization.h"
using namespace std;
using namespace graph_layout;

CrossMinimization::CrossMinimization(const CrossMinimizationMethod method) : _method(method) {
}


void CrossMinimization::setMethod(CrossMinimizationMethod method) {
    _method = method;
}

std::pair<SPLayering, std::vector<SPVirtualEdge>> CrossMinimization::reduceNumCross(SPDirectedGraph &graph, std::vector<int> &ranks) const {
    auto [layering, virtualEdges] = addVirtualEdges(graph, ranks);
    switch (_method) {
        case CrossMinimizationMethod::BARYCENTER:
            reduceNumCrossWithBaryCenterHeuristic(graph, layering);
            break;
        case CrossMinimizationMethod::MEDIAN:
            reduceNumCrossWithMedianHeuristic(graph, layering);
            break;
        case CrossMinimizationMethod::PAIRWISE_SWITCH:
            reduceNumCrossWithPairwiseSwitchHeuristic(graph, layering);
            break;
    }
    return {layering, virtualEdges};
}

std::pair<SPLayering, std::vector<SPVirtualEdge>> CrossMinimization::addVirtualEdges(SPDirectedGraph &graph, vector<int> &ranks) {
    SPLayering layering;
    auto &discreteRanks = layering.layerRanks;
    auto &orders = layering.orders;
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

    layering.width = 0;
    for (const auto &order : orders) {
        layering.width = max(layering.width, order.size());
    }
    return {layering, virtualEdges};
}

long long CrossMinimization::computeNumCross(
    SPDirectedGraph &graph,
    BinaryIndexedTree &bit,
    const std::vector<int> &order1,
    const std::vector<int> &order2,
    const bool forward) {
    bit.clear(order1.size());
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

long long CrossMinimization::computeNumCross(SPDirectedGraph &graph, const SPLayering &layering) {
    const auto &orders = layering.orders;
    BinaryIndexedTree bit(layering.width);
    long long numCross = 0;
    for (size_t i = 1; i < orders.size(); ++i) {
        numCross += computeNumCross(graph, bit, orders[i - 1], orders[i], true);
    }
    return numCross;
}

void CrossMinimization::reduceNumCrossWithWeightingHeuristic(SPDirectedGraph &graph,
                                                             SPLayering &layering,
                                                             const std::function<double(SPDirectedGraph&, const unordered_map<int, int>&, int, bool)> &weighting) {
    constexpr int NUM_REPEAT = 2;

    auto &orders = layering.orders;
    const int numLayers = static_cast<int>(orders.size());

    vector<pair<double, pair<int, int>>> weights(layering.width);

    SPLayering bestLayeredOrder(layering);
    long long bestNumCross = computeNumCross(graph, layering);
    bool lastIsBest = false, hasUpdate = false;
    unordered_map<int, int> positions;
    for (int repeatIndex = 0; repeatIndex < NUM_REPEAT; ++repeatIndex) {
        hasUpdate = false;
        for (int layerIndex = 1; layerIndex < numLayers; ++layerIndex) {
            positions.clear();
            for (int i = 0; i < orders[layerIndex - 1].size(); ++i) {
                positions[orders[layerIndex - 1][i]] = i;
            }
            const int n = static_cast<int>(orders[layerIndex].size());
            for (int i = 0; i < n; ++i) {
                const int v = orders[layerIndex][i];
                weights[i] = {weighting(graph, positions, v, true), {i, v}};
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
        if (const long long numCross = computeNumCross(graph, layering); numCross < bestNumCross) {
            bestLayeredOrder = layering;
            bestNumCross = numCross;
        }

        for (int layerIndex = numLayers - 2; layerIndex >= 0; --layerIndex) {
            positions.clear();
            for (int i = 0; i < orders[layerIndex + 1].size(); ++i) {
                positions[orders[layerIndex + 1][i]] = i;
            }
            const int n = static_cast<int>(orders[layerIndex].size());
            for (int i = 0; i < n; ++i) {
                const int u = orders[layerIndex][i];
                weights[i] = {weighting(graph, positions, u, false), {i, u}};
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
        if (const long long numCross = computeNumCross(graph, layering); numCross < bestNumCross) {
            if (repeatIndex + 1 != NUM_REPEAT) {
                bestLayeredOrder = layering;
            }
            bestNumCross = numCross;
            lastIsBest = true;
        }
    }
    if (bestLayeredOrder.width > 0 && !lastIsBest) {
        layering = bestLayeredOrder;
    }
}

void CrossMinimization::reduceNumCrossWithBaryCenterHeuristic(SPDirectedGraph &graph, SPLayering &layering) {
    auto weighting = [](SPDirectedGraph &_graph, const unordered_map<int, int> &positions, const int u, const bool forward) {
        double weight = 0.0;
        if (forward) {
            for (const auto &edge : _graph.getInEdges(u)) {
                weight += positions.find(edge.u)->second;
            }
            weight /= _graph.getInDegrees()[u];
        } else {
            for (const auto &edge : _graph.getOutEdges(u)) {
                weight += positions.find(edge.v)->second;
            }
            weight /= _graph.getOutDegrees()[u];
        }
        return weight;
    };
    reduceNumCrossWithWeightingHeuristic(graph, layering, weighting);
}

void CrossMinimization::reduceNumCrossWithMedianHeuristic(SPDirectedGraph &graph, SPLayering &layering) {
    auto weighting = [](SPDirectedGraph &_graph, const unordered_map<int, int> &positions, const int u, const bool forward) {
        vector<int> adjPositions;
        if (forward) {
            for (const auto &edge : _graph.getInEdges(u)) {
                adjPositions.emplace_back(positions.find(edge.u)->second);
            }
            ranges::sort(adjPositions);
        } else {
            for (const auto &edge : _graph.getOutEdges(u)) {
                adjPositions.emplace_back(positions.find(edge.v)->second);
            }
        }
        const int n = static_cast<int>(adjPositions.size());
        if (n == 0) {
            return 0.0;
        }
        const auto mid = adjPositions.begin() + n / 2;
        ranges::nth_element(adjPositions, mid);
        double weight = *mid;
        if (n % 2 == 0) {
            const double weight2 = *max_element(adjPositions.begin(), mid);
            weight = (weight + weight2) * 0.5;
        }
        return weight;
    };
    reduceNumCrossWithWeightingHeuristic(graph, layering, weighting);
}

long long CrossMinimization::computeNumCross(SPDirectedGraph &graph,
    const std::vector<std::unordered_map<int, int>> &positions,
    std::vector<int> &adjPositionsU,
    std::vector<int> &adjPositionsV,
    const int layerIndex, const int u, const int v, const bool forward) {
    const int numLayers = static_cast<int>(positions.size());
    const int adjLayerIndex = forward ? layerIndex + 1 : layerIndex - 1;
    if (adjLayerIndex < 0 || adjLayerIndex >= numLayers) {
        return 0;
    }
    const auto &edgesU = forward ? graph.getOutEdges(u) : graph.getInEdges(u);
    const auto &edgesV = forward ? graph.getOutEdges(v) : graph.getInEdges(v);
    int degU = 0, degV = 0;
    for (const auto &edge : edgesU) {
        adjPositionsU[degU++] = positions[adjLayerIndex].find(edge.u == u ? edge.v : edge.u)->second;
    }
    for (const auto &edge : edgesV) {
        adjPositionsV[degV++] = positions[adjLayerIndex].find(edge.u == v ? edge.v : edge.u)->second;
    }
    sort(adjPositionsU.begin(), adjPositionsU.begin() + degU);
    sort(adjPositionsV.begin(), adjPositionsV.begin() + degV);
    long long numCross = 0;
    for (int indexU = 0, indexV = 0; indexU < degU && indexV < degV;) {
        if (adjPositionsU[indexU] <= adjPositionsV[indexV]) {
            ++indexU;
        } else {
            ++indexV;
            numCross += degU - indexU;
        }
    }
    return numCross;
}

long long CrossMinimization::computeNumCross(SPDirectedGraph &graph,
    const std::vector<std::unordered_map<int, int>> &positions,
    std::vector<int> &adjPositionsU,
    std::vector<int> &adjPositionsV,
    const int layerIndex, const int u, const int v) {
    const long long numCrossForward = computeNumCross(graph, positions, adjPositionsU, adjPositionsV, layerIndex, u, v, true);
    const long long numCrossBackward = computeNumCross(graph, positions, adjPositionsU, adjPositionsV, layerIndex, u, v, false);
    return numCrossForward + numCrossBackward;
}

void CrossMinimization::
reduceNumCrossWithPairwiseSwitchHeuristic(SPDirectedGraph &graph, SPLayering &layering) {
    constexpr int NUM_REPEAT = 2;

    auto &orders = layering.orders;
    const int numLayers = static_cast<int>(orders.size());

    vector<unordered_map<int, int>> positions(numLayers);
    for (int layerIndex = 0; layerIndex < numLayers; layerIndex++) {
        for (int i = 0; i < orders[layerIndex].size(); i++) {
            positions[layerIndex][orders[layerIndex][i]] = i;
        }
    }

    int maxDegree = 0;
    for (const auto &order : orders) {
        for (const auto &u : order) {
            maxDegree = max(maxDegree, max(graph.getInDegrees()[u], graph.getOutDegrees()[u]));
        }
    }
    vector<int> adjPositions1(maxDegree), adjPositions2(maxDegree);

    bool hasUpdate = false;
    const auto swapIfLessNumCross = [&](const int layerIndex, const int index) {
        const int u = orders[layerIndex][index];
        const int v = orders[layerIndex][index + 1];
        const long long currentNumCross = computeNumCross(graph, positions, adjPositions1, adjPositions2, layerIndex, u, v);
        const long long newNumCross = computeNumCross(graph, positions, adjPositions1, adjPositions2, layerIndex, v, u);
        if (currentNumCross > newNumCross) {
            hasUpdate = true;
            swap(orders[layerIndex][index], orders[layerIndex][index + 1]);
            swap(positions[layerIndex][u], positions[layerIndex][v]);
        }
    };

    for (int repeatIndex = 0; repeatIndex < NUM_REPEAT; ++repeatIndex) {
        hasUpdate = false;
        for (int layerIndex = 0; layerIndex < numLayers; ++layerIndex) {
            for (int i = 0; i + 1 < orders[layerIndex].size(); ++i) {
                swapIfLessNumCross(layerIndex, i);
            }
        }
        if (!hasUpdate) {
            break;
        }

        hasUpdate = false;
        for (int layerIndex = numLayers - 1; layerIndex >= 0; --layerIndex) {
            for (int i = 0; i + 1 < orders[layerIndex].size(); ++i) {
                swapIfLessNumCross(layerIndex, i);
            }
        }
    }
}

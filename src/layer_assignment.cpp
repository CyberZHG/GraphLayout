#include <queue>
#include "layer_assignment.h"
using namespace std;
using namespace graph_layout;

LayerAssignment::LayerAssignment(const LayerAssignmentMethod method) : _method(method) {
}

vector<int> LayerAssignment::rankVertices(SimpleGraph &graph) const {
    switch (_method) {
        case LayerAssignmentMethod::NETWORK_SIMPLEX:
            return rankVerticesNetworkSimplex(graph);
    }
    return {};
}

vector<int> LayerAssignment::rankVerticesNetworkSimplex(SimpleGraph &graph) {
    constexpr int INF_COST = 1000000000;
    const size_t n = graph.numVertices();
    const size_t m = graph.numEdges();
    if (n == 0) {
        return {};
    }
    if (n == 1) {
        return {0};
    }
    const auto &inDegrees = graph.getInDegrees();
    const auto &outDegrees = graph.getOutDegrees();
    // Build a new graph with a super source and sink node.
    // For existing nodes, balance_i = out_degree - in_degree; for the source and sink node, balance_i = 0;
    // For existing edges, cost_i = 1; for the edges connecting to the source or sink node, cost_i = inf;
    auto balance = [n, &inDegrees, &outDegrees](const size_t index) {
        return index < n ? outDegrees[index] - inDegrees[index] : 0;
    };
    SimpleGraph newGraph(n + 1);
    const int superSource = static_cast<int>(n);
    for (const auto &edge : graph.edges()) {
        newGraph.addEdge(edge);
    }
    vector flow(m + n, 0);
    vector potential(n + 1, 0);
    potential[n] = 0;
    for (int i = 0; i < n; i++) {
        if (balance(i) >= 0) {
            newGraph.addEdge(i, superSource);
            flow[m + i] = balance(i);
            potential[i] = INF_COST;
        } else {
            newGraph.addEdge(superSource, i);
            flow[m + i] = -balance(i);
            potential[i] = -INF_COST;
        }
    }
    const auto &edges = newGraph.edges();
    const auto &inEdges = newGraph.getInEdges();
    const auto &outEdges = newGraph.getOutEdges();
    auto cost = [m](const size_t index) {
        return index < m ? 1 : INF_COST;
    };
    // Initial basic tree solution: the source flows to all the other nodes directly.
    vector parent(n + 1, static_cast<int>(n));
    parent[n] = -1;
    vector<size_t> parentEdgeIndex(n + 1);
    for (size_t i = 0; i < n; ++i) {
        parentEdgeIndex[i] = m + i;
    }
    parentEdgeIndex[n] = -1;
    vector reducedCost(m + n, -1);
    size_t minReducedCostIndex = 0;
    for (size_t i = 0; i < m + n; ++i) {
        reducedCost[i] = cost(i) - (potential[edges[i].u] - potential[edges[i].v]);
        if (reducedCost[i] < reducedCost[minReducedCostIndex]) {
            minReducedCostIndex = i;
        }
    }
    int labelIndex = 0;
    vector labels(n + 1, 0);
    vector<pair<int, size_t>> reverseStack(n + 1);
    while (reducedCost[minReducedCostIndex] < 0) {
        // Find the cycle after adding this edge to the spanning tree.
        reducedCost[minReducedCostIndex] = 0;
        const size_t currentEdgeIndex = minReducedCostIndex;
        const auto &edge = edges[currentEdgeIndex];
        labels[edge.u] = ++labelIndex;
        size_t minFlowIndex = n + m + 1;
        for (int p = edge.u; p != -1; p = parent[p]) {
            if (edges[parentEdgeIndex[p]].u == p) {
                if (minFlowIndex == n + m + 1 || flow[parentEdgeIndex[p]] < flow[minFlowIndex]) {
                    minFlowIndex = parentEdgeIndex[p];
                }
            }
            labels[p] = labelIndex;
        }
        int ancestor = -1;
        bool breakOnUSide = true;
        for (int p = edge.v; p != -1; p = parent[p]) {
            if (edges[parentEdgeIndex[p]].v == p) {
                if (minFlowIndex == n + m + 1 || flow[parentEdgeIndex[p]] < flow[minFlowIndex]) {
                    minFlowIndex = parentEdgeIndex[p];
                    breakOnUSide = false;
                }
            }
            if (labels[p] == labelIndex) {
                ancestor = p;
                break;
            }
        }
        // Flow along the cycle.
        const int minFlow = flow[minFlowIndex];
        flow[currentEdgeIndex] = minFlow;
        for (int p = edge.u; p != ancestor; p = parent[p]) {
            if (const size_t &edgeIndex = parentEdgeIndex[p]; edges[edgeIndex].u == p) {
                flow[edgeIndex] -= minFlow;
            } else {
                flow[edgeIndex] += minFlow;
            }
        }
        for (int p = edge.v; p != ancestor; p = parent[p]) {
            if (const size_t &edgeIndex = parentEdgeIndex[p]; edges[edgeIndex].v == p) {
                flow[edgeIndex] -= minFlow;
            } else {
                flow[edgeIndex] += minFlow;
            }
        }
        // Update parent relation
        int beginNode = edge.u;
        if (breakOnUSide) {
            parent[edge.u] = edge.v;
            parentEdgeIndex[edge.u] = currentEdgeIndex;
        } else {
            parent[edge.v] = edge.u;
            parentEdgeIndex[edge.v] = currentEdgeIndex;
            beginNode = edge.v;
        }
        int stackIndex = 0;
        for (int p = beginNode; p != ancestor; p = parent[p]) {
            reverseStack[stackIndex++] = {p, parentEdgeIndex[p]};
            if (parentEdgeIndex[p] == minFlowIndex) {
                parent[p] = -1;
                break;
            }
        }
        for (int i = stackIndex - 1; i > 0; --i) {
            const auto &v = reverseStack[i].first;
            const auto &u = reverseStack[i - 1].first;
            parent[v] = u;
            parentEdgeIndex[v] = reverseStack[i - 1].second;
        }
        // Update potential and reduced cost
        int fixedNode = -1;
        beginNode = -1;
        if (breakOnUSide) {
            int newPotential = cost(currentEdgeIndex) + potential[edge.v];
            if (potential[edge.u] != newPotential) {
                potential[edge.u] = newPotential;
                beginNode = edge.u;;
                fixedNode = edge.v;
            }
        } else {
            int newPotential = potential[edge.u] - cost(currentEdgeIndex);
            if (potential[edge.v] != newPotential) {
                potential[edge.v] = newPotential;
                beginNode = edge.v;;
                fixedNode = edge.u;
            }
        }
        while (beginNode != -1) {
            int nextNode = -1;
            for (const auto &edgeIndex : inEdges[beginNode]) {
                const auto &edge = edges[edgeIndex];
                if (edge.u == fixedNode) {
                    continue;
                }
                if (flow[edgeIndex]) {
                    const int newPotential = cost(edgeIndex) + potential[beginNode];
                    if (potential[edge.u] != newPotential) {
                        potential[edge.u] = newPotential;
                        nextNode = edge.u;
                    }
                } else {
                    reducedCost[edgeIndex] = cost(edgeIndex) - (potential[edge.u] - potential[edge.v]);
                    if (reducedCost[edgeIndex] < reducedCost[minReducedCostIndex]) {
                        minReducedCostIndex = edgeIndex;
                    }
                }
            }
            for (const auto &edgeIndex : outEdges[beginNode]) {
                const auto &edge = edges[edgeIndex];
                if (edge.v == fixedNode) {
                    continue;
                }
                if (flow[edgeIndex]) {
                    const int newPotential = potential[beginNode] - cost(edgeIndex);
                    if (potential[edge.v] != newPotential) {
                        potential[edge.v] = newPotential;
                        nextNode = edge.v;
                    }
                } else {
                    reducedCost[edgeIndex] = cost(edgeIndex) - (potential[edge.u] - potential[edge.v]);
                    if (reducedCost[edgeIndex] < reducedCost[minReducedCostIndex]) {
                        minReducedCostIndex = edgeIndex;
                    }
                }
            }
            fixedNode = beginNode;
            beginNode = nextNode;
        }
    }
    int maxPotential = -INF_COST;
    potential.pop_back();
    for (auto & i : potential) {
        maxPotential = max(maxPotential, i);
    }
    for (int & i : potential) {
        i = maxPotential - i;
    }
    return potential;
}

#include <queue>
#include <set>
#include <utility>
#include "layer_assignment.h"

#include <iostream>
#include <ostream>
#include <ranges>
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
    const size_t n = graph.numVertices();
    if (n == 0) {
        return {};
    }
    if (n == 1) {
        return {0};
    }
    auto ranks = rankVerticesTopological(graph);
    auto results = networkSimplexInitFeasibleTree(graph, ranks);
    return ranks;
}

/**
 * Find an initial spanning tree based on the given initial ranks.
 *
 * The spanning tree should be "tight", i.e., for every edge in the tree, slack(e) = 0.
 * The "slack" of an edge is defined as its length minus the minimum length of that edge.
 * While the spanning tree does not cover all the vertices in the graph,
 * in each iteration we select a non-tree edge that is incident on the tree and has the minimum slack.
 * We then add it to the spanning tree and adjust the ranks of the vertices
 * that are already in the tree, so that the new edge remains tight.
 *
 * @param graph A DAG.
 * @param ranks Initial ranks, which could be the result of the topological solution,
 *      are also used to store the updated ranks.
 * @return A parent vector that represents the spanning tree.
 */
std::vector<int> LayerAssignment::networkSimplexInitFeasibleTree(SimpleDirectedGraph &graph,
                                                                 std::vector<int> &ranks) const {
    const size_t n = graph.numVertices();
    const size_t m = graph.numEdges();
    unordered_map<int, int> slacks;  // slack(e) = rank(e.v) - rank(e.u) - minEdgeLength(e)
    vector parents(n, -1);
    set<pair<int, int>> incidentEdges;
    int source = -1;
    auto calcSlack = [&](const SimpleEdge &edge) {
        return slacks[edge.id] = ranks[edge.v] - ranks[edge.u] - minEdgeLength(edge.id);
    };
    for (int u = 0; u < n; ++u) {
        if (ranks[u] == 0) {
            source = u;
            for (const auto &edge : graph.getOutEdges(u)) {
                incidentEdges.insert(make_pair(calcSlack(edge), edge.id));
            }
            break;
        }
    }
    unordered_set verticesInTree({source});
    auto inTree = [&](const int u) {
        return u == source || parents[u] != -1;
    };
    vector<int> slacksToUpdate(m);
    while (!incidentEdges.empty()) {
        const auto [slack, id] = *incidentEdges.begin();
        incidentEdges.erase(incidentEdges.begin());
        const auto &edge = graph.getEdge(id);
        const bool headInTree = inTree(edge.u);
        if (headInTree && inTree(edge.v)) {
            continue;
        }
        if (slack) {
            int delta = slack;
            if (!headInTree) {
                delta = -delta;
            }
            for (const auto u : verticesInTree) {
                ranks[u] += delta;
            }
            int numSlacksToUpdate = 0;
            for (const auto &val: incidentEdges | views::values) {
                const auto &incidentEdge = graph.getEdge(val);
                if (inTree(incidentEdge.u) ^ inTree(incidentEdge.v)) {
                    slacksToUpdate[numSlacksToUpdate++] = incidentEdge.id;
                }
            }
            for (int i = 0; i < numSlacksToUpdate; ++i) {
                const auto &incidentEdge = graph.getEdge(slacksToUpdate[i]);
                incidentEdges.erase({slacks[incidentEdge.id], incidentEdge.id});
                incidentEdges.insert(make_pair(calcSlack(incidentEdge), incidentEdge.id));
            }
        }
        const int newVertex = headInTree ? edge.v : edge.u;
        parents[newVertex] = id;
        verticesInTree.emplace(newVertex);
        for (const auto &newEdge : graph.getInEdges(newVertex)) {
            if (inTree(newEdge.u)) {
                continue;
            }
            incidentEdges.insert(make_pair(calcSlack(newEdge), newEdge.id));
        }
        for (const auto &newEdge : graph.getOutEdges(newVertex)) {
            if (inTree(newEdge.v)) {
                continue;
            }
            incidentEdges.insert(make_pair(calcSlack(newEdge), newEdge.id));
        }
    }
    return parents;
}

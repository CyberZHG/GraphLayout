#include "graph_def.h"

#include <cassert>
#include <queue>
using namespace std;
using namespace graph_layout;

SimpleGraph::SimpleGraph(const size_t num_vertices) : _numVertices(num_vertices) {
}

void SimpleGraph::addEdge(const SimpleEdge &edge) {
    assert(0 <= edge.u && edge.u < _numVertices);
    assert(0 <= edge.v && edge.v < _numVertices);
    _edges.emplace_back(edge);
    resetInitialization();
}

void SimpleGraph::addEdge(const int u, const int v) {
    addEdge(SimpleEdge(static_cast<int>(_edges.size()), u, v));
}

void SimpleGraph::addOutEdges(const int u, const vector<int> &vertices) {
    for (const auto &v: vertices) {
        addEdge(u, v);
    }
}

const unordered_map<int, size_t> & SimpleGraph::getEdgeIdToIndexMap() {
    if (!_edgeIdToIndexMapInitialized) {
        _edgeIdToIndexMap.clear();
        for (size_t i = 0; i < _edges.size(); i++) {
            _edgeIdToIndexMap[_edges[i].id] = i;
        }
        _edgeIdToIndexMapInitialized = true;
    }
    return _edgeIdToIndexMap;
}

const vector<int> &SimpleGraph::getInDegrees() {
    if (!_degreesInitialized) {
        initDegrees();
    }
    return _inDegrees;
}

const vector<int> &SimpleGraph::getOutDegrees() {
    if (!_degreesInitialized) {
        initDegrees();
    }
    return _outDegrees;
}

const vector<vector<int>> &SimpleGraph::getInVertices() {
    if (!_inOutVerticesInitialized) {
        initInOutVertices();
    }
    return _inVertices;
}

const vector<vector<int>> &SimpleGraph::getOutVertices() {
    if (!_inOutVerticesInitialized) {
        initInOutVertices();
    }
    return _outVertices;
}

const vector<vector<int>> & SimpleGraph::getInEdges() {
    if (!_inOutEdgesInitialized) {
        initInOutEdges();
    }
    return _inEdges;
}

const vector<vector<int>> & SimpleGraph::getOutEdges() {
    if (!_inOutEdgesInitialized) {
        initInOutEdges();
    }
    return _outEdges;
}

bool SimpleGraph::hasCycle() {
    if (!_hasCycleInitialized) {
        initHasCycle();
    }
    return _hasCycle;
}

void SimpleGraph::resetInitialization() {
    _edgeIdToIndexMapInitialized = false;
    _degreesInitialized = false;
    _inOutVerticesInitialized = false;
    _inOutEdgesInitialized = false;
    _hasCycleInitialized = false;
}

void SimpleGraph::initDegrees() {
    _inDegrees = vector(_numVertices, 0);
    _outDegrees = vector(_numVertices, 0);
    for (const auto &edge : _edges) {
        ++_outDegrees[edge.u];
        ++_inDegrees[edge.v];
    }
    _degreesInitialized = true;
}

void SimpleGraph::initInOutVertices() {
    _inVertices = vector(_numVertices, vector<int>());
    _outVertices = vector(_numVertices, vector<int>());
    for (const auto &edge : _edges) {
        _inVertices[edge.v].emplace_back(edge.u);
        _outVertices[edge.u].emplace_back(edge.v);
    }
    _inOutVerticesInitialized = true;
}

void SimpleGraph::initInOutEdges() {
    _inEdges = vector(_numVertices, vector<int>());
    _outEdges = vector(_numVertices, vector<int>());
    for (const auto &edge : _edges) {
        _inEdges[edge.v].emplace_back(edge.id);
        _outEdges[edge.u].emplace_back(edge.id);
    }
    _inOutEdgesInitialized = true;
}

void SimpleGraph::initHasCycle() {
    auto inDegrees = vector(getInDegrees());
    const auto &outEdges = getOutEdges();
    const auto &edgeIdToIndexMap = getEdgeIdToIndexMap();
    size_t numVisited = 0;
    queue<int> q;
    for (size_t i = 0; i < _numVertices; i++) {
        if (inDegrees[i] == 0) {
            q.push(static_cast<int>(i));
        }
    }
    while (!q.empty()) {
        const int u = q.front();
        q.pop();
        ++numVisited;
        for (const auto &id: outEdges[u]) {
            if (const auto &v = _edges[edgeIdToIndexMap.find(id)->second].v; --inDegrees[v] == 0) {
                q.push(static_cast<int>(v));
            }
        }
    }
    _hasCycle = numVisited != _numVertices;
    _hasCycleInitialized = true;
}

#include "graph_def.h"

#include <cassert>
#include <queue>
using namespace std;
using namespace graph_layout;

SimpleDirectedGraph::SimpleDirectedGraph(const size_t num_vertices) : _numVertices(num_vertices) {
}

void SimpleDirectedGraph::addEdge(const SimpleEdge &edge) {
    assert(0 <= edge.u && edge.u < _numVertices);
    assert(0 <= edge.v && edge.v < _numVertices);
    _edges.emplace_back(edge);
    resetInitialization();
}

void SimpleDirectedGraph::addEdge(const int u, const int v) {
    addEdge(SimpleEdge(static_cast<int>(_edges.size()), u, v));
}

void SimpleDirectedGraph::addOutEdges(const int u, const vector<int> &vertices) {
    for (const auto &v: vertices) {
        addEdge(u, v);
    }
}

void SimpleDirectedGraph::disableSelfCycleEdges() {
    resetInitialization();
    int newNumEdges = 0;
    for (auto & _edge : _edges) {
        if (_edge.u == _edge.v) {
            _selfCycleEdges.emplace_back(_edge);
        } else {
            _edges[newNumEdges++] = _edge;
        }
    }
    _edges.erase(_edges.begin() + newNumEdges, _edges.end());
}

void SimpleDirectedGraph::enableSelfCycleEdges() {
    resetInitialization();
    _edges.insert(_edges.end(), _selfCycleEdges.begin(), _selfCycleEdges.end());
}

void SimpleDirectedGraph::reverseEdges(const std::unordered_set<int> &ids) {
    resetInitialization();
    _reverseIds = ids;
    for (auto &edge : _edges) {
        if (_reverseIds.contains(edge.id)) {
            swap(edge.u, edge.v);
        }
    }
}

void SimpleDirectedGraph::reverseEdgesBack() {
    resetInitialization();
    for (auto &edge : _edges) {
        if (_reverseIds.contains(edge.id)) {
            swap(edge.u, edge.v);
        }
    }
    _reverseIds.clear();
}

const unordered_map<int, size_t> & SimpleDirectedGraph::getEdgeIdToIndexMap() {
    if (!_edgeIdToIndexMapInitialized) {
        _edgeIdToIndexMap.clear();
        for (size_t i = 0; i < _edges.size(); i++) {
            _edgeIdToIndexMap[_edges[i].id] = i;
        }
        _edgeIdToIndexMapInitialized = true;
    }
    return _edgeIdToIndexMap;
}

const vector<int> &SimpleDirectedGraph::getInDegrees() {
    if (!_degreesInitialized) {
        initDegrees();
    }
    return _inDegrees;
}

const vector<int> &SimpleDirectedGraph::getOutDegrees() {
    if (!_degreesInitialized) {
        initDegrees();
    }
    return _outDegrees;
}

const vector<vector<int>> &SimpleDirectedGraph::getInVertices() {
    if (!_inOutVerticesInitialized) {
        initInOutVertices();
    }
    return _inVertices;
}

const vector<vector<int>> &SimpleDirectedGraph::getOutVertices() {
    if (!_inOutVerticesInitialized) {
        initInOutVertices();
    }
    return _outVertices;
}

const vector<vector<int>> & SimpleDirectedGraph::getInEdgeIds() {
    if (!_inOutEdgesInitialized) {
        initInOutEdges();
    }
    return _inEdges;
}

const vector<vector<int>> & SimpleDirectedGraph::getOutEdgeIds() {
    if (!_inOutEdgesInitialized) {
        initInOutEdges();
    }
    return _outEdges;
}

EdgeIterationWithIDs SimpleDirectedGraph::getInEdges(const int v) {
    return {*this, getInEdgeIds()[v]};
}

EdgeIterationWithIDs SimpleDirectedGraph::getOutEdges(const int u) {
    return {*this, getOutEdgeIds()[u]};
}

bool SimpleDirectedGraph::hasCycle() {
    if (!_hasCycleInitialized) {
        initHasCycle();
    }
    return _hasCycle;
}

void SimpleDirectedGraph::resetInitialization() {
    _edgeIdToIndexMapInitialized = false;
    _degreesInitialized = false;
    _inOutVerticesInitialized = false;
    _inOutEdgesInitialized = false;
    _hasCycleInitialized = false;
}

void SimpleDirectedGraph::initDegrees() {
    _inDegrees = vector(_numVertices, 0);
    _outDegrees = vector(_numVertices, 0);
    for (const auto &edge : _edges) {
        ++_outDegrees[edge.u];
        ++_inDegrees[edge.v];
    }
    _degreesInitialized = true;
}

void SimpleDirectedGraph::initInOutVertices() {
    _inVertices = vector(_numVertices, vector<int>());
    _outVertices = vector(_numVertices, vector<int>());
    for (const auto &edge : _edges) {
        _inVertices[edge.v].emplace_back(edge.u);
        _outVertices[edge.u].emplace_back(edge.v);
    }
    _inOutVerticesInitialized = true;
}

void SimpleDirectedGraph::initInOutEdges() {
    _inEdges = vector(_numVertices, vector<int>());
    _outEdges = vector(_numVertices, vector<int>());
    for (const auto &edge : _edges) {
        _inEdges[edge.v].emplace_back(edge.id);
        _outEdges[edge.u].emplace_back(edge.id);
    }
    _inOutEdgesInitialized = true;
}

void SimpleDirectedGraph::initHasCycle() {
    auto inDegrees = vector(getInDegrees());
    const auto &outEdges = getOutEdgeIds();
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

const SimpleEdge & EdgeIterationWithIDs::iterator::operator*() const {
    const auto edgeIndex = _graph.getEdgeIdToIndexMap().find(_ids[_index])->second;
    return _graph.edges()[edgeIndex];
}

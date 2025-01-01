#include "vertex_positioning.h"

#include <chrono>
using namespace std;
using namespace graph_layout;

VertexPositioning::VertexPositioning(const VertexPositioningMethod method) : _method(method) {
}

void VertexPositioning::setVertexMargin(const double margin) {
    _vertexMargin = margin;
}

void VertexPositioning::setLayerMargin(const double margin) {
    _layerMargin = margin;
}

void VertexPositioning::setVertexSizes(const double size) {
    _vertexSize = size;
}

void VertexPositioning::setVertexSizes(std::vector<double> &&sizes) {
    _vertexSizes = std::move(sizes);
    if (!_vertexSizes.empty()) {
        _vertexSize = ranges::min(_vertexSizes);
    }
}

void VertexPositioning::sortIncidentEdges(SPDirectedGraph &graph, SPLayering &layering) {
    const size_t numLayers = layering.orders.size();
    auto &inEdgeIds = graph.getInEdgeIdsRef();
    auto &outEdgeIds = graph.getOutEdgeIdsRef();
    for (int i = 0; i < numLayers; i++) {
        for (const auto u : layering.orders[i]) {
            ranges::sort(inEdgeIds[u], [&](const int a, const int b) {
                return layering.positions[i - 1][graph.getEdge(a).u] < layering.positions[i - 1][graph.getEdge(b).u];
            });
            ranges::sort(outEdgeIds[u], [&](const int a, const int b) {
                return layering.positions[i + 1][graph.getEdge(a).v] < layering.positions[i + 1][graph.getEdge(b).v];
            });
        }
    }
}

std::pair<VertexPositioning::RootVec, VertexPositioning::AlignVec> VertexPositioning::verticalAlignment(
    SPDirectedGraph &graph, SPLayering &layering, const bool forward, const bool leftToRight) {
    const int n = static_cast<int>(graph.numVertices());
    if (n == 0) {
        return {{}, {}};
    }
    const int numLayers = static_cast<int>(layering.orders.size());
    RootVec roots(n);
    AlignVec aligns(n);
    for (int u = 0; u < n; ++u) {
        roots[u] = aligns[u] = u;
    }
    vector<int> candidates(2);
    for (int layerIndex = forward ? 1 : numLayers - 2;
        forward ? layerIndex < numLayers : layerIndex >= 0;
        forward ? ++layerIndex : --layerIndex) {
        const int lastLayerIndex = forward ? layerIndex - 1 : layerIndex + 1;
        const int numVertices = static_cast<int>(layering.orders[layerIndex].size());
        int lastPosition = forward ? -1 : INT32_MAX;
        for (int vertexIndex = leftToRight ? 0 : numVertices - 1;
            leftToRight ? vertexIndex < numVertices : vertexIndex >= 0;
            leftToRight ? ++vertexIndex : --vertexIndex) {
            const int u = layering.orders[layerIndex][vertexIndex];
            const auto &edgeIds = (forward ? graph.getInEdgeIds() : graph.getOutEdgeIds())[u];
            if (const int numEdges = static_cast<int>(edgeIds.size()); numEdges == 0) {
                continue;
            } else if (numEdges % 2 == 1) {
                const auto &edge = graph.getEdge(edgeIds[numEdges / 2]);
                candidates[0] = forward ? edge.u : edge.v;
                candidates[1] = -1;
            } else {
                const auto &edge1 = graph.getEdge(edgeIds[(numEdges - 1) / 2]);
                const auto &edge2 = graph.getEdge(edgeIds[numEdges / 2]);
                candidates[0] = forward ? edge1.u : edge1.v;
                candidates[1] = forward ? edge2.u : edge2.v;
                if (!leftToRight) {
                    swap(candidates[0], candidates[1]);
                }
            }
            for (const auto v : candidates) {
                if (const int posV = layering.positions[lastLayerIndex][v]; forward ? lastPosition < posV : lastPosition > posV) {
                    aligns[v] = u;
                    roots[u] = roots[v];
                    aligns[u] = roots[v];
                    lastPosition = posV;
                    break;
                }
            }
        }
    }
    return {roots, aligns};
}

std::vector<double> VertexPositioning::horizontalCompaction(const SPDirectedGraph &graph,
    SPLayering &layering,
    const RootVec &roots, const AlignVec &aligns,
    const bool leftToRight) const {
    const int n = static_cast<int>(graph.numVertices());
    if (n == 0) {
        return {};
    }
    vector<bool> visited(n), hasShifts(n);
    vector<double> positions(n), shifts(n);
    vector<int> sinks(n);
    for (int u = 0; u < n; ++u) {
        sinks[u] = u;
    }
    std::function<void(int)> placeBlock = [&] (int u) {
        if (visited[u]) {
            return;
        }
        visited[u] = true;
        const int root = u;
        do {
            const int layerIndex = layering.idToLayer[u];
            const auto &orders = layering.orders[layerIndex];
            const int posU = layering.positions[layerIndex][u];
            if (leftToRight ? posU > 0 : posU + 1 < orders.size()) {
                const int posV = leftToRight ? posU - 1 : posU + 1;
                const int v = roots[orders[posV]];
                placeBlock(v);
                if (sinks[u] == u) {
                    sinks[u] = sinks[v];
                }
                const double vertexMargin = _vertexMargin + (vertexSizeAt(u) + vertexSizeAt(orders[posV])) / 2.0;
                if (sinks[u] != sinks[v]) {
                    const double margin = positions[u] - positions[v] - vertexMargin;
                    if (!hasShifts[sinks[v]] ||
                        (leftToRight ? margin < shifts[sinks[v]] : -margin > shifts[sinks[v]])) {
                        hasShifts[sinks[v]] = true;
                        shifts[sinks[v]] = margin;
                    }
                } else {
                    const double newPos = leftToRight ? positions[v] + vertexMargin : positions[v] - vertexMargin;
                    if (leftToRight ? newPos > positions[u] : newPos < positions[u]) {
                        positions[u] = newPos;
                    }
                }
            }
            u = aligns[u];
        } while (u != root);
    };
    for (int u = 0; u < n; ++u) {
        if (roots[u] == u) {
            placeBlock(u);
        }
    }
    for (int u = 0; u < n; ++u) {
        positions[u] = positions[roots[u]];
        if (hasShifts[sinks[roots[u]]]) {
            positions[u] += shifts[sinks[roots[u]]];
        }
    }
    const double minPosition = ranges::min(positions);
    for (auto &position : positions) {
        position -= minPosition;
    }
    return positions;
}

std::vector<std::pair<double, double>> VertexPositioning::assignCoordinates(SPDirectedGraph &graph, SPLayering &layering) const {
    switch (_method) {
        case VertexPositioningMethod::BRANDES_KOPF:
            break;
    }
    return {};
}

double VertexPositioning::vertexSizeAt(const int index) const {
    if (index >= _vertexSizes.size()) {
        return _vertexSize;
    }
    return _vertexSizes[index];
}

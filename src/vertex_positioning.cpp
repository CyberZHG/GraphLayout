#include "vertex_positioning.h"
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

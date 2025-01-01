#include "graph_layout.h"

#include <format>
#include <fstream>

#ifdef GRAPH_LAYOUT_ENABLE_SVG
#include "common/draw_svg.h"
#endif
using namespace std;
using namespace graph_layout;

DirectedGraphHierarchicalLayout::DirectedGraphHierarchicalLayout() = default;

void DirectedGraphHierarchicalLayout::setGraph(const std::shared_ptr<SPDirectedGraph>& graph) {
    _initialNumVertices = static_cast<int>(graph->numVertices());
    _graph = graph;
}

std::shared_ptr<SPDirectedGraph> DirectedGraphHierarchicalLayout::graph() const {
    return _graph;
}

GraphAttributes& DirectedGraphHierarchicalLayout::graphAttributes() {
    return _graphAttributes;
}

VertexAttributes& DirectedGraphHierarchicalLayout::vertexAttributes() {
    return _vertexGlobalAttributes;
}

VertexAttributes DirectedGraphHierarchicalLayout::vertexAttributes(const int u) const {
    if (const auto it = _vertexAttributes.find(u); it != _vertexAttributes.end()) {
        return VertexAttributes::stringMappingToAttributes(it->second, _vertexGlobalAttributes);
    }
    return _vertexGlobalAttributes;
}

void DirectedGraphHierarchicalLayout::setVertexAttributes(const int u, const std::string &key, const std::string &value) {
    _vertexAttributes[u][key] = value;
}

EdgeAttributes& DirectedGraphHierarchicalLayout::edgeAttributes() {
    return _edgeGlobalAttributes;
}

EdgeAttributes DirectedGraphHierarchicalLayout::edgeAttributes(const int u) const {
    if (const auto it = _edgeAttributes.find(u); it != _edgeAttributes.end()) {
        return EdgeAttributes::stringMappingToAttributes(it->second, _edgeGlobalAttributes);
    }
    return _edgeGlobalAttributes;
}

void DirectedGraphHierarchicalLayout::setEdgeAttributes(const int u, const std::string &key, const std::string &value) {
    _edgeAttributes[u][key] = value;
}

void DirectedGraphHierarchicalLayout::setEdgeAttributes(const int u, const unordered_map<string, string>& mapping) {
    _edgeAttributes[u] = mapping;
}

void DirectedGraphHierarchicalLayout::setFeedbackArcsMethod(const FeedbackArcsMethod method) {
    _feedbackArcsFinder.setMethod(method);
}

void DirectedGraphHierarchicalLayout::setLayerAssignmentMethod(const LayerAssignmentMethod method) {
    _layerAssignment.setMethod(method);
}

void DirectedGraphHierarchicalLayout::setCrossMinimizationMethod(const CrossMinimizationMethod method) {
    _crossMinimization.setMethod(method);
}

void DirectedGraphHierarchicalLayout::setVertexPositioningMethod(const VertexPositioningMethod method) {
    _vertexPositioning.setMethod(method);
}

std::pair<std::vector<double>, std::vector<double>> DirectedGraphHierarchicalLayout::layoutGraph() {
    const size_t n = _graph->numVertices();
    int newVertexIndex = static_cast<int>(n);
    int edgeIndex = CrossMinimization::VIRTUAL_EDGE_ID_OFFSET;
    _graph->disableSelfCycleEdges();
    GraphComponentSplitter splitter;
    const auto feedbackArcs = _feedbackArcsFinder.findFeedbackArcs(*_graph);
    _graph->reverseEdges(feedbackArcs);
    _xs.resize(n);
    _ys.resize(n);
    double subGraphShift = 0.0;
    auto subGraphs = splitter.splitGraph(*_graph);
    for (int groupIndex = 0; groupIndex < subGraphs.size(); ++groupIndex) {
        auto& subGraph = subGraphs[groupIndex];
        const size_t subN = subGraph.numVertices();
        auto ranks = _layerAssignment.rankVertices(subGraph);
        auto [layering, virtualEdges] = _crossMinimization.reduceNumCross(subGraph, ranks);
        auto [subXs, subYs] = _vertexPositioning.assignCoordinates(subGraph, layering);
        double maxLeftVertexSize = 0.0, maxRightVertexSize = 0.0, maxX = 0.0;
        for (int u = 0; u < subN; ++u) {
            if (subXs[u] == 0.0) {
                maxLeftVertexSize = max(maxLeftVertexSize, _vertexPositioning.vertexSizeAt(u));
            }
            if (subXs[u] >= maxX) {
                maxX = subXs[u];
                maxRightVertexSize = max(maxRightVertexSize, _vertexPositioning.vertexSizeAt(u));
            }
        }
        if (groupIndex > 0) {
            subGraphShift += maxLeftVertexSize * 0.5;
        }
        for (const auto& virtualEdge : virtualEdges) {
            const auto& originalEdge = virtualEdge.originalEdge;
            const auto& edgeIds = virtualEdge.virtualEdgeIds;
            bool isReversed = _graph->isReverseEdge(originalEdge.id);
            bool removeOriginalEdge = false;
            int lastVertex = originalEdge.u;
            vector<int> newEdgeIds;
            for (int i = 0; i + 1 < edgeIds.size(); ++i) {
                const auto& inEdge = subGraph.getEdge(edgeIds[i]);
                const auto& outEdge = subGraph.getEdge(edgeIds[i + 1]);
                if (abs(subXs[inEdge.u] - subXs[inEdge.v]) > 1e-8 || abs(subXs[outEdge.u] - subXs[outEdge.v]) > 1e-8) {
                    removeOriginalEdge = true;
                    _graph->updateNumVertices(newVertexIndex + 1);
                    newEdgeIds.push_back(edgeIndex);
                    if (!isReversed) {
                        _graph->addEdge({edgeIndex++, lastVertex, newVertexIndex});
                    } else {
                        _graph->addEdge({edgeIndex++, newVertexIndex, lastVertex});
                    }
                    _xs.push_back(subXs[inEdge.v] + subGraphShift);
                    _ys.push_back(subYs[inEdge.v] + subGraphShift);
                    lastVertex = newVertexIndex++;
                }
            }
            if (removeOriginalEdge) {
                _graph->removeEdge(originalEdge.id);
                newEdgeIds.push_back(edgeIndex);
                if (!isReversed) {
                    _graph->addEdge({edgeIndex++, lastVertex, originalEdge.v});
                } else {
                    _graph->addEdge({edgeIndex++, originalEdge.v, lastVertex});
                }
                const int midId = newEdgeIds[newEdgeIds.size() / 2];
                for (const auto& edgeId : newEdgeIds) {
                    if (edgeId == midId) {
                        if (_edgeAttributes.contains(originalEdge.id)) {
                            _edgeAttributes[midId] = _edgeAttributes[originalEdge.id];
                        }
                    } else {
                        setEdgeAttributes(edgeId, ATTRIBUTE_KEY_LABEL, "");
                    }
                }
            }
        }
        for (int u = 0; u < subN; ++u) {
            _xs[splitter.originalVertexId(groupIndex, u)] = subXs[u] + subGraphShift;
            _ys[splitter.originalVertexId(groupIndex, u)] = subYs[u] + subGraphShift;
        }
        subGraphShift += maxX + maxRightVertexSize * 0.5;
    }
    _graph->reverseEdgesBack();
    _graph->enableSelfCycleEdges();
    adjustCoordinatesByGraphRank();
    return {_xs, _ys};
}

#ifdef GRAPH_LAYOUT_ENABLE_SVG
void DirectedGraphHierarchicalLayout::drawSVG(const std::string& outputFilePath) const {
    const int n = static_cast<int>(_graph->numVertices());
    constexpr double scale = 30.0;
    constexpr double margin = 1.0;
    double minX, maxX, minY, maxY;
    for (int u = 0; u < n; ++u) {
        minX = min(minX, _xs[u] - _vertexPositioning.vertexSizeAt(u));
        maxX = max(maxX, _xs[u] + _vertexPositioning.vertexSizeAt(u));
        minY = min(minY, _ys[u] - _vertexPositioning.vertexSizeAt(u));
        maxY = max(maxY, _ys[u] + _vertexPositioning.vertexSizeAt(u));
    }
    const double width = (maxX - minX + margin * 2) * scale;
    const double height = (maxY - minY + margin * 2) * scale;
    const double shiftX = (margin + abs(minX)) * scale;
    const double shiftY = (margin + abs(minY)) * scale;
    const auto svg = DrawSVG(outputFilePath, width, height);
    if (!_graphAttributes.bgcolor.isNone()) {
        auto [red, green, blue] = _graphAttributes.bgcolor.toRGB();
        svg.drawBackground(red, green, blue);
    }
    for (int u = 0; u < _initialNumVertices; ++u) {
        const VertexAttributes attributes = vertexAttributes(u);
        const double x = _xs[u] * scale + shiftX;
        const double y = _ys[u] * scale + shiftY;
        const double r = _vertexPositioning.vertexSizeAt(u) * 0.5 * scale;
        if (attributes.shape == VertexAttributes::Shape::Circle) {
            svg.drawCircle(x, y, r);
        } else if (attributes.shape == VertexAttributes::Shape::DoubleCircle) {
            svg.drawCircle(x, y, r);
            svg.drawCircle(x, y, r * 0.8);
        }
        if (!attributes.label.empty()) {
            svg.drawText(x, y, attributes.label);
        }
    }

    for (const auto& edge : _graph->edges()) {
        const EdgeAttributes attributes = edgeAttributes(edge.id);
        const double edgeLen = sqrt((_xs[edge.u] - _xs[edge.v]) * (_xs[edge.u] - _xs[edge.v]) + (_ys[edge.u] - _ys[edge.v]) * (_ys[edge.u] - _ys[edge.v]));
        double x1, y1, x2, y2;
        if (isVirtualVertex(edge.u)) {
            x1 = _xs[edge.u];
            y1 = _ys[edge.u];
        } else {
            x1 = _xs[edge.u] + (_xs[edge.v] - _xs[edge.u]) * _vertexPositioning.vertexSizeAt(edge.u) * 0.5 / edgeLen;
            y1 = _ys[edge.u] + (_ys[edge.v] - _ys[edge.u]) * _vertexPositioning.vertexSizeAt(edge.u) * 0.5 / edgeLen;
        }
        if (isVirtualVertex(edge.v)) {
            x2 = _xs[edge.v];
            y2 = _ys[edge.v];
        } else {
            x2 = _xs[edge.v] + (_xs[edge.u] - _xs[edge.v]) * _vertexPositioning.vertexSizeAt(edge.v) * 0.5 / edgeLen;
            y2 = _ys[edge.v] + (_ys[edge.u] - _ys[edge.v]) * _vertexPositioning.vertexSizeAt(edge.v) * 0.5 / edgeLen;
        }
        x1 = x1 * scale + shiftX;
        y1 = y1 * scale + shiftY;
        x2 = x2 * scale + shiftX;
        y2 = y2 * scale + shiftY;
        svg.drawLine(x1, y1, x2, y2, !isVirtualVertex(edge.v));
        if (!attributes.label.empty()) {
            const double dx = x2 - x1;
            const double dy = y2 - y1;
            const double len = sqrt(dx * dx + dy * dy);
            const double nx = -dy / len;
            const double ny = dx / len;
            const double midX = (x1 + x2) / 2;
            const double midY = (y1 + y2) / 2;
            const double x = midX + nx * 10.0;
            const double y = midY + ny * 10.0;
            svg.drawText(x, y, attributes.label);
        }
    }
}
#endif

void DirectedGraphHierarchicalLayout::initializeVertexLabelsWithNumericalValues(const int start) {
    const int n = _initialNumVertices;
    for (int i = 0; i < n; ++i) {
        setVertexAttributes(i, ATTRIBUTE_KEY_LABEL, format("{}", start + i));
    }
}

void DirectedGraphHierarchicalLayout::setVertexLabels(const vector<string> &vertexLabels) {
    for (int i = 0; i < vertexLabels.size(); ++i) {
        setVertexAttributes(i, ATTRIBUTE_KEY_LABEL, vertexLabels[i]);
    }
}

/** A vertex is virtual if the vertex ID is greater than the maximum vertex ID in the beginning graph.
 *
 * @param u A vertex ID.
 * @return Whether the vertex is virtual.
 */
bool DirectedGraphHierarchicalLayout::isVirtualVertex(const int u) const {
    return u >= _initialNumVertices;
}

/** Adjust the coordinates by `_graphAttributes.rank`.
 * The default rank is top to bottom.
 */
void DirectedGraphHierarchicalLayout::adjustCoordinatesByGraphRank() {
    if (_graphAttributes.rank == GraphAttributes::Rank::TopToBottom) {
        return;
    }
    if (_graphAttributes.rank == GraphAttributes::Rank::BottomToTop || _graphAttributes.rank == GraphAttributes::Rank::RightToLeft) {
        const auto yMin = ranges::min(_ys);
        const auto yMax = ranges::max(_ys);
        for (auto& y : _ys) {
            y = yMax - y + yMin;
        }
    }
    if (_graphAttributes.rank == GraphAttributes::Rank::LeftToRight || _graphAttributes.rank == GraphAttributes::Rank::RightToLeft) {
        swap_ranges(_xs.begin(), _xs.end(), _ys.begin());
    }
}

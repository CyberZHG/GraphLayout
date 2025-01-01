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

Attributes & DirectedGraphHierarchicalLayout::attributes() {
    return _attributes;
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

void DirectedGraphHierarchicalLayout::setLayerMargin(const double margin) {
    _vertexPositioning.setLayerMargin(margin);
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
                        _attributes.transferEdgeAttributes(originalEdge.id, midId);
                    } else {
                        _attributes.setEdgeAttributes(edgeId, ATTRIBUTE_KEY_LABEL, "");
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
    constexpr double margin = 30.0;
    double minX, maxX, minY, maxY;
    for (int u = 0; u < n; ++u) {
        minX = min(minX, _xs[u] - _vertexPositioning.vertexSizeAt(u));
        maxX = max(maxX, _xs[u] + _vertexPositioning.vertexSizeAt(u));
        minY = min(minY, _ys[u] - _vertexPositioning.vertexSizeAt(u));
        maxY = max(maxY, _ys[u] + _vertexPositioning.vertexSizeAt(u));
    }
    unordered_map<int, unordered_set<int>> outEdges;
    const auto rankDir = _attributes.rankDir();
    for (const auto& [id, u, v] : _graph->edges()) {
        outEdges[u].insert(v);
        if (u == v) {
            if (rankDir == AttributeRankDir::TOP_TO_BOTTOM) {
                minX = min(minX, _xs[u] - _vertexPositioning.vertexSizeAt(u) * 2.5);
            } else if (rankDir == AttributeRankDir::BOTTOM_TO_TOP) {
                maxX = max(maxX, _xs[u] + _vertexPositioning.vertexSizeAt(u) * 2.5);
            } else if (rankDir == AttributeRankDir::LEFT_TO_RIGHT) {
                maxY = max(maxY, _ys[u] + _vertexPositioning.vertexSizeAt(u) * 2.5);
            } else {
                minY = min(minY, _ys[u] - _vertexPositioning.vertexSizeAt(u) * 2.5);
            }
        }
    }
    const double width = maxX - minX + margin * 2;
    const double height = maxY - minY + margin * 2;
    const double shiftX = margin + abs(minX);
    const double shiftY = margin + abs(minY);
    const auto svg = DrawSVG(outputFilePath, width, height);
    const auto backgroundColor = _attributes.graphAttributes(ATTRIBUTE_KEY_BG_COLOR);
    if (!backgroundColor.empty()) {
        auto [red, green, blue] = AttributeColor::toRGB(backgroundColor);
        svg.drawBackground(red, green, blue);
    }
    for (int u = 0; u < _initialNumVertices; ++u) {
        const double x = _xs[u] + shiftX;
        const double y = _ys[u] + shiftY;
        const double r = _vertexPositioning.vertexSizeAt(u) * 0.5;
        const auto shape = _attributes.vertexAttributes(u, ATTRIBUTE_KEY_SHAPE);
        if (shape == AttributeShape::CIRCLE) {
            svg.drawCircle(x, y, r);
        } else if (shape == AttributeShape::DOUBLE_CIRCLE) {
            svg.drawCircle(x, y, r);
            svg.drawCircle(x, y, r * 0.8);
        }
        const auto label = _attributes.vertexAttributes(u, ATTRIBUTE_KEY_LABEL);
        svg.drawText(x, y, label);
    }

    for (const auto& edge : _graph->edges()) {
        const auto label = _attributes.edgeAttributes(edge.id, ATTRIBUTE_KEY_LABEL);
        if (edge.u != edge.v) {
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
            x1 = x1 + shiftX;
            y1 = y1 + shiftY;
            x2 = x2 + shiftX;
            y2 = y2 + shiftY;
            if (outEdges[edge.v].contains(edge.u)) {  // There is a reverse edge
                const double dx = x2 - x1;
                const double dy = y2 - y1;
                const double len = sqrt(dx * dx + dy * dy);
                const double nx = -dy / len;
                const double ny = dx / len;
                const double midX = (x1 + x2) / 2;
                const double midY = (y1 + y2) / 2;
                const double x = midX + nx * 15.0;
                const double y = midY + ny * 15.0;
                x1 = _xs[edge.u] + shiftX;
                y1 = _ys[edge.u] + shiftY;
                x2 = _xs[edge.v] + shiftX;
                y2 = _ys[edge.v] + shiftY;
                const double radius1 = _vertexPositioning.vertexSizeAt(edge.u) * 0.5;
                const double radius2 = _vertexPositioning.vertexSizeAt(edge.v) * 0.5;
                const double edgeLen1 = sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));
                const double edgeLen2 = sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));
                x1 += (x - x1) * radius1 / edgeLen1;
                y1 += (y - y1) * radius1 / edgeLen1;
                x2 += (x - x2) * radius2 / edgeLen2;
                y2 += (y - y2) * radius2 / edgeLen2;
                svg.drawLine(x1, y1, x, y);
                svg.drawLine(x, y, x2, y2, !isVirtualVertex(edge.v));
                if (!label.empty()) {
                    const double tx = midX + nx * 30.0;
                    const double ty = midY + ny * 30.0;
                    svg.drawText(tx, ty, label);
                }
            } else {
                svg.drawLine(x1, y1, x2, y2, !isVirtualVertex(edge.v));
                if (!label.empty()) {
                    const double dx = x2 - x1;
                    const double dy = y2 - y1;
                    const double len = sqrt(dx * dx + dy * dy);
                    const double nx = -dy / len;
                    const double ny = dx / len;
                    const double midX = (x1 + x2) / 2;
                    const double midY = (y1 + y2) / 2;
                    const double x = midX + nx * 15.0;
                    const double y = midY + ny * 15.0;
                    svg.drawText(x, y, label);
                }
            }
        } else {
            double dx = 0, dy = 0;
            if (rankDir == AttributeRankDir::TOP_TO_BOTTOM) {
                dy = 1.0;
            } else if (rankDir == AttributeRankDir::BOTTOM_TO_TOP) {
                dy = -1.0;
            } else if (rankDir == AttributeRankDir::LEFT_TO_RIGHT) {
                dx = 1.0;
            } else {
                dx = -1.0;
            }
            const double nx = -dy, ny = dx;
            const double x = _xs[edge.u] + shiftX, y = _ys[edge.u] + shiftY;
            const double dir3 = atan2(ny, nx);
            constexpr double rotate = 3.14 / 6.0;
            const double dir12 = dir3 + rotate;
            const double dir45 = dir3 - rotate;
            const double radius = _vertexPositioning.vertexSizeAt(edge.u) * 0.5;
            const double x1 = x + cos(dir12) * radius;
            const double y1 = y + sin(dir12) * radius;
            const double x2 = x + cos(dir12) * radius * 2.5;
            const double y2 = y + sin(dir12) * radius * 2.5;
            const double x3 = x + cos(dir3) * radius * 3;
            const double y3 = y + sin(dir3) * radius * 3;
            const double x4 = x + cos(dir45) * radius * 2.5;
            const double y4 = y + sin(dir45) * radius * 2.5;
            const double x5 = x + cos(dir45) * radius;
            const double y5 = y + sin(dir45) * radius;
            svg.drawLine(x1, y1, x2, y2);
            svg.drawLine(x2, y2, x3, y3);
            svg.drawLine(x3, y3, x4, y4);
            svg.drawLine(x4, y4, x5, y5, true);
            const double xt = x + cos(dir3) * (radius * 3 + 15.0);
            const double yt = y + sin(dir3) * (radius * 3 + 15.0);
            svg.drawText(xt, yt, label);
        }
    }
}
#endif

void DirectedGraphHierarchicalLayout::initializeVertexLabelsWithNumericalValues(const int start) {
    const int n = _initialNumVertices;
    for (int i = 0; i < n; ++i) {
        _attributes.setVertexAttributes(i, ATTRIBUTE_KEY_LABEL, format("{}", start + i));
    }
}

void DirectedGraphHierarchicalLayout::setVertexLabels(const vector<string> &vertexLabels) {
    for (int i = 0; i < vertexLabels.size(); ++i) {
        _attributes.setVertexAttributes(i, ATTRIBUTE_KEY_LABEL, vertexLabels[i]);
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
    const auto rankDir = _attributes.rankDir();
    if (rankDir == AttributeRankDir::TOP_TO_BOTTOM) {
        return;
    }
    if (rankDir == AttributeRankDir::BOTTOM_TO_TOP || rankDir == AttributeRankDir::RIGHT_TO_LEFT) {
        const auto yMin = ranges::min(_ys);
        const auto yMax = ranges::max(_ys);
        for (auto& y : _ys) {
            y = yMax - y + yMin;
        }
    }
    if (rankDir == AttributeRankDir::LEFT_TO_RIGHT || rankDir == AttributeRankDir::RIGHT_TO_LEFT) {
        swap_ranges(_xs.begin(), _xs.end(), _ys.begin());
    }
}

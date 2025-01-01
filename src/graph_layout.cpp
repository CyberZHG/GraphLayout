#include "graph_layout.h"

#include <format>
#include <fstream>

#include <cairo.h>
#include <cairo-svg.h>
#include <iostream>
#include <pango/pangocairo.h>
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
            for (int i = 0; i + 1 < edgeIds.size(); ++i) {
                const auto& inEdge = subGraph.getEdge(edgeIds[i]);
                const auto& outEdge = subGraph.getEdge(edgeIds[i + 1]);
                if (abs(subXs[inEdge.u] - subXs[inEdge.v]) > 1e-8 || abs(subXs[outEdge.u] - subXs[outEdge.v]) > 1e-8) {
                    removeOriginalEdge = true;
                    _graph->updateNumVertices(newVertexIndex + 1);
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
                if (!isReversed) {
                    _graph->addEdge({edgeIndex++, lastVertex, originalEdge.v});
                } else {
                    _graph->addEdge({edgeIndex++, originalEdge.v, lastVertex});
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
    return {_xs, _ys};
}

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
    cairo_surface_t* surface = cairo_svg_surface_create(outputFilePath.c_str(), width, height);
    cairo_t* cr = cairo_create(surface);

    for (int u = 0; u < _initialNumVertices; ++u) {
        const double x = _xs[u] * scale + shiftX;
        const double y = _ys[u] * scale + shiftY;
        const double r = _vertexPositioning.vertexSizeAt(u) * 0.5 * scale;
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 1);
        cairo_arc(cr, x, y, r, 0, 2 * 3.1415926);
        cairo_stroke(cr);
        cairo_new_path(cr);

        PangoLayout* layout = pango_cairo_create_layout(cr);
        pango_layout_set_text(layout, _vertexLabels[u].c_str(), -1);
        PangoFontDescription* font_desc = pango_font_description_from_string("Serif 16");
        pango_layout_set_font_description(layout, font_desc);
        pango_font_description_free(font_desc);
        PangoRectangle ink_rect, logical_rect;
        pango_layout_get_extents(layout, &ink_rect, &logical_rect);
        const double iw = ink_rect.width / 1024.0;
        const double ih = ink_rect.height / 1024.0;
        const double textX = x - iw / 2.0 - ink_rect.x / 1024.0;
        const double textY = y - ih / 2.0 - ink_rect.y / 1024.0;
        cairo_move_to(cr, textX, textY);
        pango_cairo_show_layout(cr, layout);
        g_object_unref(layout);
        cairo_new_path(cr);
    }

    for (const auto& edge : _graph->edges()) {
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

        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 1);
        cairo_move_to(cr, x1, y1);
        cairo_line_to(cr, x2, y2);
        cairo_stroke(cr);
        if (!isVirtualVertex(edge.v)) {
            const double angle = atan2(y2 - y1, x2 - x1);
            const double x3 = x2 - 10 * cos(angle - M_PI/8);
            const double y3 = y2 - 10 * sin(angle - M_PI/8);
            const double x4 = x2 - 10 * cos(angle + M_PI/8);
            const double y4 = y2 - 10 * sin(angle + M_PI/8);
            cairo_move_to(cr, x2, y2);
            cairo_line_to(cr, x3, y3);
            cairo_line_to(cr, x4, y4);
            cairo_line_to(cr, x2, y2);
            cairo_fill(cr);
        }
    }

    cairo_destroy(cr);
    cairo_surface_finish(surface);
    cairo_surface_destroy(surface);
}

void DirectedGraphHierarchicalLayout::initializeVertexLabelsWithNumericalValues(const int start) {
    const int n = _initialNumVertices;
    _vertexLabels.resize(n);
    for (int i = 0; i < n; ++i) {
        _vertexLabels[i] = format("{}", start + i);
    }
}

void DirectedGraphHierarchicalLayout::setVertexLabels(const vector<string> &vertexLabels) {
    if (_initialNumVertices != vertexLabels.size()) {
        throw runtime_error(format("The length of input vertex labels {} "
                                   "does not match the number of vertices {}.", vertexLabels.size(), _initialNumVertices));
    }
    _vertexLabels = vertexLabels;
}

/** A vertex is virtual if the vertex ID is greater than the maximum vertex ID in the beginning graph.
 *
 * @param u A vertex ID.
 * @return Whether the vertex is virtual.
 */
bool DirectedGraphHierarchicalLayout::isVirtualVertex(const int u) const {
    return u >= _initialNumVertices;
}

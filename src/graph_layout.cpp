#include "graph_layout.h"

#include <format>
#include <fstream>
using namespace std;
using namespace graph_layout;

DirectedGraphHierarchicalLayout::DirectedGraphHierarchicalLayout() = default;

void DirectedGraphHierarchicalLayout::setGraph(const std::shared_ptr<SPDirectedGraph> &graph) {
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
    ofstream file(outputFilePath);
    file << R"(<?xml version="1.0" encoding="UTF-8"?>)" << endl;

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
    double shiftX = (margin + abs(minX)) * scale;
    double shiftY = (margin + abs(minY)) * scale;
    file << format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="{}" height="{}" viewBox="0 0 {} {}">)", width, height, width, height) << endl;

    file << R"(  <defs>)" << endl;
    file << R"(    <marker id="arrowhead" markerWidth="10" markerHeight="7" refX="10" refY="3.5" orient="auto" fill="black">)" << endl;
    file << R"(      <polygon points="0 0, 10 3.5, 0 7" />)" << endl;
    file << R"(    </marker>)" << endl;
    file << R"(  </defs>)" << endl;

    for (int u = 0; u < _initialNumVertices; ++u) {
        const double x = _xs[u] * scale + shiftX;
        const double y = _ys[u] * scale + shiftY;
        const double r = _vertexPositioning.vertexSizeAt(u) * 0.5 * scale;
        file << format(R"(  <circle cx="{}" cy="{}" r="{}" fill="none" stroke="black" stroke-width="1"/>)", x, y, r) << endl;
        file << format(R"(  <text x="{}" y="{}" text-anchor="middle" dominant-baseline="middle" font-size="20" fill="black">)", x, y);
        file << _vertexLabels[u];
        file << "</text>" << endl;
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

        file << format(R"(  <line x1="{}" y1="{}" x2="{}" y2="{}" stroke="black" stroke-width="1")", x1, y1, x2, y2);
        if (!isVirtualVertex(edge.v)) {
            file << " marker-end=\"url(#arrowhead)\"";
        }
        file << " />" << endl;
    }

    file << "</svg>" << endl;
}

void DirectedGraphHierarchicalLayout::initializeVertexLabelsWithNumericalValues(const int start) {
    const int n = _initialNumVertices;
    _vertexLabels.resize(n);
    for (int i = 0; i < n; ++i) {
        _vertexLabels[i] = format("{}", start + i);
    }
}

bool DirectedGraphHierarchicalLayout::isVirtualVertex(const int u) const {
    return u >= _initialNumVertices;
}

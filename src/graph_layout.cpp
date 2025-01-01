#include "graph_layout.h"
using namespace std;
using namespace graph_layout;

DirectedGraphHierarchicalLayout::DirectedGraphHierarchicalLayout() = default;

std::pair<std::vector<double>, std::vector<double>> DirectedGraphHierarchicalLayout::layoutGraph(SPDirectedGraph &graph) const {
    const size_t n = graph.numVertices();
    int newVertexIndex = static_cast<int>(n);
    int edgeIndex = CrossMinimization::VIRTUAL_EDGE_ID_OFFSET;
    graph.disableSelfCycleEdges();
    GraphComponentSplitter splitter;
    const auto feedbackArcs = _feedbackArcsFinder.findFeedbackArcs(graph);
    graph.reverseEdges(feedbackArcs);
    vector<double> xs(n), ys(n);
    double subGraphShift = 0.0;
    auto subGraphs = splitter.splitGraph(graph);
    for (int groupIndex = 0; groupIndex < subGraphs.size(); ++groupIndex) {
        auto &subGraph = subGraphs[groupIndex];
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
        for (const auto &virtualEdge : virtualEdges) {
            const auto &originalEdge = virtualEdge.originalEdge;
            const auto &edgeIds = virtualEdge.virtualEdgeIds;
            bool isReversed = graph.isReverseEdge(originalEdge.id);
            bool removeOriginalEdge = false;
            int lastVertex = originalEdge.u;
            for (int i = 0; i + 1 < edgeIds.size(); ++i) {
                const auto &inEdge = subGraph.getEdge(edgeIds[i]);
                const auto &outEdge = subGraph.getEdge(edgeIds[i + 1]);
                if (abs(subXs[inEdge.u] - subXs[inEdge.v]) > 1e-8 || abs(subXs[outEdge.u] - subXs[outEdge.v]) > 1e-8) {
                    removeOriginalEdge = true;
                    graph.updateNumVertices(newVertexIndex + 1);
                    if (!isReversed) {
                        graph.addEdge({edgeIndex++, lastVertex, newVertexIndex});
                    } else {
                        graph.addEdge({edgeIndex++, newVertexIndex, lastVertex});
                    }
                    xs.push_back(subXs[inEdge.v] + subGraphShift);
                    ys.push_back(subYs[inEdge.v] + subGraphShift);
                    lastVertex = newVertexIndex++;
                }
            }
            if (removeOriginalEdge) {
                graph.removeEdge(originalEdge.id);
                if (!isReversed) {
                    graph.addEdge({edgeIndex++, lastVertex, originalEdge.v});
                } else {
                    graph.addEdge({edgeIndex++, originalEdge.v, lastVertex});
                }
            }
        }
        for (int u = 0; u < subN; ++u) {
            xs[splitter.originalVertexId(groupIndex, u)] = subXs[u] + subGraphShift;
            ys[splitter.originalVertexId(groupIndex, u)] = subYs[u] + subGraphShift;
        }
        subGraphShift += maxX + maxRightVertexSize * 0.5;
    }
    graph.reverseEdgesBack();
    graph.enableSelfCycleEdges();
    return {xs, ys};
}
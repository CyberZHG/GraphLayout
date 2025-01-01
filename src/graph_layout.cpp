#include "graph_layout.h"
using namespace std;
using namespace graph_layout;

DirectedGraphHierarchicalLayout::DirectedGraphHierarchicalLayout() = default;

std::pair<std::vector<double>, std::vector<double>> DirectedGraphHierarchicalLayout::layoutGraph(SPDirectedGraph &graph) const {
    const size_t n = graph.numVertices();
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
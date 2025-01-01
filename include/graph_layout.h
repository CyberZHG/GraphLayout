#ifndef GRAPHLAYOUT_GRAPH_LAYOUT_H
#define GRAPHLAYOUT_GRAPH_LAYOUT_H

#include <vector>
#include "graph_def.h"
#include "feedback_arcs.h"
#include "layer_assignment.h"
#include "cross_minimization.h"
#include "vertex_positioning.h"

namespace graph_layout {

    /** A Sugiyama style directed graph layout. */
    class DirectedGraphHierarchicalLayout {
    public:
        DirectedGraphHierarchicalLayout();
        ~DirectedGraphHierarchicalLayout() = default;

        void setGraph(const std::shared_ptr<SPDirectedGraph>& graph);
        [[nodiscard]] std::shared_ptr<SPDirectedGraph> graph() const;

        std::pair<std::vector<double>, std::vector<double>> layoutGraph();
        void drawSVG(const std::string& outputFilePath) const;

        /** Add numeric vertex labels.
         *
         * @param start The start index.
         */
        void initializeVertexLabelsWithNumericalValues(int start = 1);

    private:
        std::shared_ptr<SPDirectedGraph> _graph = nullptr;

        FeedbackArcsFinder _feedbackArcsFinder;
        LayerAssignment _layerAssignment;
        CrossMinimization _crossMinimization;
        VertexPositioning _vertexPositioning;

        int _initialNumVertices = 0;
        std::vector<double> _xs, _ys;

        std::vector<std::string> _vertexLabels;

        [[nodiscard]] bool isVirtualVertex(int u) const;
    };

}

#endif //GRAPHLAYOUT_GRAPH_LAYOUT_H
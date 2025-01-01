#ifndef GRAPHLAYOUT_GRAPH_LAYOUT_H
#define GRAPHLAYOUT_GRAPH_LAYOUT_H

#include <vector>
#include "common/graph_def.h"
#include "common/graph_attributes.h"
#include "directed/feedback_arcs.h"
#include "directed/layer_assignment.h"
#include "directed/cross_minimization.h"
#include "directed/vertex_positioning.h"

namespace graph_layout {

    /** A Sugiyama style directed graph layout. */
    class DirectedGraphHierarchicalLayout {
    public:
        DirectedGraphHierarchicalLayout();
        ~DirectedGraphHierarchicalLayout() = default;

        void setGraph(const std::shared_ptr<SPDirectedGraph>& graph);
        [[nodiscard]] std::shared_ptr<SPDirectedGraph> graph() const;

        Attributes& attributes();

        void setFeedbackArcsMethod(FeedbackArcsMethod method);
        void setLayerAssignmentMethod(LayerAssignmentMethod method);
        void setCrossMinimizationMethod(CrossMinimizationMethod method);
        void setVertexPositioningMethod(VertexPositioningMethod method);
        void setLayerMargin(double margin);

        std::pair<std::vector<double>, std::vector<double>> layoutGraph();
        /** Add numeric vertex labels.
         *
         * @param start The start index.
         */
        void initializeVertexLabelsWithNumericalValues(int start = 1);
        void setVertexLabels(const std::vector<std::string> &vertexLabels);

#ifdef GRAPH_LAYOUT_ENABLE_SVG
        void drawSVG(const std::string& outputFilePath) const;
#endif

    private:
        std::shared_ptr<SPDirectedGraph> _graph = nullptr;

        Attributes _attributes;

        FeedbackArcsFinder _feedbackArcsFinder;
        LayerAssignment _layerAssignment;
        CrossMinimization _crossMinimization;
        VertexPositioning _vertexPositioning;

        int _initialNumVertices = 0;
        std::vector<double> _xs, _ys;

        [[nodiscard]] bool isVirtualVertex(int u) const;
        void adjustCoordinatesByGraphRank();

#ifdef GRAPH_LAYOUT_ENABLE_SVG
        void computeVertexSizes();
#endif
    };

}

#endif //GRAPHLAYOUT_GRAPH_LAYOUT_H
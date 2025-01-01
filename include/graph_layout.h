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

        GraphAttributes& graphAttributes();

        VertexAttributes& vertexAttributes();
        [[nodiscard]] VertexAttributes vertexAttributes(int u) const;
        void setVertexAttributes(int u, const std::string& key, const std::string& value);

        EdgeAttributes& edgeAttributes();
        [[nodiscard]] EdgeAttributes edgeAttributes(int u) const;
        void setEdgeAttributes(int u, const std::string& key, const std::string& value);

        void setFeedbackArcsMethod(FeedbackArcsMethod method);
        void setLayerAssignmentMethod(LayerAssignmentMethod method);
        void setCrossMinimizationMethod(CrossMinimizationMethod method);
        void setVertexPositioningMethod(VertexPositioningMethod method);

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

        GraphAttributes _graphAttributes;
        VertexAttributes _vertexGlobalAttributes;
        std::vector<std::unordered_map<std::string, std::string>> _vertexAttributes;
        EdgeAttributes _edgeGlobalAttributes;
        std::vector<std::unordered_map<std::string, std::string>> _edgeAttributes;

        FeedbackArcsFinder _feedbackArcsFinder;
        LayerAssignment _layerAssignment;
        CrossMinimization _crossMinimization;
        VertexPositioning _vertexPositioning;

        int _initialNumVertices = 0;
        std::vector<double> _xs, _ys;

        [[nodiscard]] bool isVirtualVertex(int u) const;
        void adjustCoordinatesByGraphRank();
    };

}

#endif //GRAPHLAYOUT_GRAPH_LAYOUT_H
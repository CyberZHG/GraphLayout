#ifndef GRAPHLAYOUT_GRAPH_LAYOUT_H
#define GRAPHLAYOUT_GRAPH_LAYOUT_H

#include <vector>
#include "graph_def.h"
#include "feedback_arcs.h"
#include "layer_assignment.h"
#include "cross_minimization.h"
#include "vertex_positioning.h"

namespace graph_layout {

    class DirectedGraphHierarchicalLayout {
    public:
        DirectedGraphHierarchicalLayout();
        ~DirectedGraphHierarchicalLayout() = default;

        std::pair<std::vector<double>, std::vector<double>> layoutGraph(SPDirectedGraph &) const;

    private:
        FeedbackArcsFinder _feedbackArcsFinder;
        LayerAssignment _layerAssignment;
        CrossMinimization _crossMinimization;
        VertexPositioning _vertexPositioning;
    };

}

#endif //GRAPHLAYOUT_GRAPH_LAYOUT_H
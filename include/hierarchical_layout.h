#ifndef GRAPHLAYOUT_HIERARCHICAL_LAYOUT_H
#define GRAPHLAYOUT_HIERARCHICAL_LAYOUT_H

#include "feedback_arcs.h"
#include "layer_assignment.h"

namespace graph_layout {

    struct HierarchicalLayoutConfig {
        FeedbackArcsMethod feedbackArcsMethod = FeedbackArcsMethod::EADES_93;
        LayerAssignmentMethod layerAssignmentMethod = LayerAssignmentMethod::GANSNER_93;
    };
}

#endif //GRAPHLAYOUT_HIERARCHICAL_LAYOUT_H
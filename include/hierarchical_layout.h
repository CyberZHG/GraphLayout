#ifndef GRAPHLAYOUT_HIERARCHICAL_LAYOUT_H
#define GRAPHLAYOUT_HIERARCHICAL_LAYOUT_H

#include "feedback_arcs.h"

namespace graph_layout {

    struct HierarchicalLayoutConfig {
        FeedbackArcsMethod cycle_break_method = FeedbackArcsMethod::Eades_93;
    };
}

#endif //GRAPHLAYOUT_HIERARCHICAL_LAYOUT_H
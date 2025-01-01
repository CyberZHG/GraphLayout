#ifndef GRAPHLAYOUT_FEEDBACK_ARCS_H
#define GRAPHLAYOUT_FEEDBACK_ARCS_H

#include <unordered_set>
#include "graph_def.h"

namespace graph_layout {
    enum class FeedbackArcsMethod {
        // Eades, P., Lin, X., & Smyth, W. F. (1993).
        // A fast and effective heuristic for the feedback arc set problem.
        // Information processing letters, 47(6), 319-323.
        EADES_93,
    };

    class FeedbackArcsFinder {
    public:
        explicit FeedbackArcsFinder(FeedbackArcsMethod method = FeedbackArcsMethod::EADES_93);
        ~FeedbackArcsFinder() = default;

        [[nodiscard]] std::unordered_set<int> findFeedbackArcs(SimpleDirectedGraph &) const;
    private:
        FeedbackArcsMethod _method;

        static std::unordered_set<int> findFeedbackArcsEades93(SimpleDirectedGraph &) ;
    };
}

#endif //GRAPHLAYOUT_FEEDBACK_ARCS_H
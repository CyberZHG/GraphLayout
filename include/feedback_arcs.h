#ifndef GRAPHLAYOUT_CYCLE_BREAK_H
#define GRAPHLAYOUT_CYCLE_BREAK_H

#include <vector>
#include "graph_def.h"

namespace graph_layout {
    enum class FeedbackArcsMethod {
        // Eades, P., Lin, X., & Smyth, W. F. (1993).
        // A fast and effective heuristic for the feedback arc set problem.
        // Information processing letters, 47(6), 319-323.
        Eades_93,
    };

    class FeedbackArcsFinder {
    public:
        explicit FeedbackArcsFinder(FeedbackArcsMethod method = FeedbackArcsMethod::Eades_93);
        ~FeedbackArcsFinder() = default;

        [[nodiscard]] std::vector<int> findFeedbackArcs(SimpleGraph &) const;
    private:
        FeedbackArcsMethod _method;

        static std::vector<int> findFeedbackArcsEades93(SimpleGraph &) ;
    };
}

#endif //GRAPHLAYOUT_CYCLE_BREAK_H
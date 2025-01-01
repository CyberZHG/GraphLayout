#ifndef GRAPHLAYOUT_CROSS_MINIMIZATION_H
#define GRAPHLAYOUT_CROSS_MINIMIZATION_H

#include "graph_def.h"

namespace graph_layout {
    enum class CrossMinimizationMethod {
        BARYCENTER_HEURISTIC,
        MEDIAN_HEURISTIC,
        GREEDY_SWITCH_HEURISTIC,
    };

    class CrossMinimization {
    public:
        explicit CrossMinimization(CrossMinimizationMethod method = CrossMinimizationMethod::BARYCENTER_HEURISTIC);
        ~CrossMinimization() = default;

        static constexpr int VIRTUAL_EDGE_ID_OFFSET = 1000000000;

        static std::vector<SPVirtualEdge> addVirtualEdges(SPDirectedGraph& graph, std::vector<int>& ranks);
    private:
        CrossMinimizationMethod _method;
    };
}

#endif //GRAPHLAYOUT_CROSS_MINIMIZATION_H
#ifndef GRAPHLAYOUT_CROSS_MINIMIZATION_H
#define GRAPHLAYOUT_CROSS_MINIMIZATION_H

#include "graph_def.h"
#include "binary_indexed_tree.h"

namespace graph_layout {
    enum class CrossMinimizationMethod {
        BARYCENTER_HEURISTIC,
        MEDIAN_HEURISTIC,
        GREEDY_SWITCH_HEURISTIC,
    };

    struct SPLayeredOrder {
        std::vector<int> layerRanks;
        std::vector<std::vector<int>> orders;
    };

    class CrossMinimization {
    public:
        explicit CrossMinimization(CrossMinimizationMethod method = CrossMinimizationMethod::BARYCENTER_HEURISTIC);
        ~CrossMinimization() = default;

        static constexpr int VIRTUAL_EDGE_ID_OFFSET = 1000000000;

        static std::pair<SPLayeredOrder, std::vector<SPVirtualEdge>> addVirtualEdges(SPDirectedGraph& graph, std::vector<int>& ranks);
        static long long calcNumCross(SPDirectedGraph& graph, const SPLayeredOrder& layeredOrder);

        // void reduceNumCross(SPDirectedGraph &, std::vector<int>& ranks) const;

    private:
        CrossMinimizationMethod _method;

        static long long calcNumCross(
            SPDirectedGraph &graph,
            BinaryIndexedTree &bit,
            const std::vector<int> &order1,
            const std::vector<int> &order2,
            bool forward = true);
    };
}

#endif //GRAPHLAYOUT_CROSS_MINIMIZATION_H
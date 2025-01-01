#ifndef GRAPHLAYOUT_CROSS_MINIMIZATION_H
#define GRAPHLAYOUT_CROSS_MINIMIZATION_H

#include "graph_def.h"
#include "binary_indexed_tree.h"

namespace graph_layout {
    enum class CrossMinimizationMethod {
        BARYCENTER,
        MEDIAN,
        PAIRWISE_SWITCH,
    };

    struct SPLayeredOrder {
        std::vector<int> layerRanks;
        std::vector<std::vector<int>> orders;
        size_t width;
    };

    class CrossMinimization {
    public:
        explicit CrossMinimization(CrossMinimizationMethod method = CrossMinimizationMethod::BARYCENTER);
        ~CrossMinimization() = default;

        static constexpr int VIRTUAL_EDGE_ID_OFFSET = 1000000000;

        std::pair<SPLayeredOrder, std::vector<SPVirtualEdge>> reduceNumCross(SPDirectedGraph &, std::vector<int>& ranks) const;

        static std::pair<SPLayeredOrder, std::vector<SPVirtualEdge>> addVirtualEdges(SPDirectedGraph& graph, std::vector<int>& ranks);
        static long long calcNumCross(SPDirectedGraph& graph, const SPLayeredOrder& layeredOrder);

    private:
        CrossMinimizationMethod _method;

        static long long calcNumCross(
            SPDirectedGraph &graph,
            BinaryIndexedTree &bit,
            const std::vector<int> &order1,
            const std::vector<int> &order2,
            bool forward = true);

        static void reduceNumCrossWithBaryCenterHeuristic(SPDirectedGraph& graph, SPLayeredOrder& layeredOrder) ;
    };
}

#endif //GRAPHLAYOUT_CROSS_MINIMIZATION_H
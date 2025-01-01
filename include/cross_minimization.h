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

    struct SPLayering {
        std::vector<int> layerRanks;
        std::vector<std::vector<int>> orders;
        std::vector<std::unordered_map<int, int>> positions;
        std::unordered_map<int, int> idToLayer;
        size_t width;

        void initMappings();
    };

    class CrossMinimization {
    public:
        explicit CrossMinimization(CrossMinimizationMethod method = CrossMinimizationMethod::BARYCENTER);
        ~CrossMinimization() = default;

        static constexpr int VIRTUAL_EDGE_ID_OFFSET = 1000000000;

        void setMethod(CrossMinimizationMethod method);

        std::pair<SPLayering, std::vector<SPVirtualEdge>> reduceNumCross(SPDirectedGraph &, std::vector<int>& ranks) const;

        static std::pair<SPLayering, std::vector<SPVirtualEdge>> addVirtualEdges(SPDirectedGraph& graph, std::vector<int>& ranks);
        static long long computeNumCross(SPDirectedGraph& graph, const SPLayering& layering);

    private:
        CrossMinimizationMethod _method;

        static long long computeNumCross(
            SPDirectedGraph &graph,
            BinaryIndexedTree &bit,
            const std::vector<int> &order1,
            const std::vector<int> &order2,
            bool forward = true);
        static long long computeNumCross(SPDirectedGraph& graph,
            const std::vector<std::unordered_map<int, int>> &positions,
            std::vector<int> &adjPositionsU,
            std::vector<int> &adjPositionsV,
            int layerIndex, int u, int v, bool forward);
        static long long computeNumCross(SPDirectedGraph& graph,
            const std::vector<std::unordered_map<int, int>> &positions,
            std::vector<int> &adjPositionsU,
            std::vector<int> &adjPositionsV,
            int layerIndex, int u, int v);

        static void reduceNumCrossWithWeightingHeuristic(SPDirectedGraph& graph, SPLayering& layering,
            const std::function<double(SPDirectedGraph&, const std::unordered_map<int, int>&, int, bool)> &weighting);
        static void reduceNumCrossWithBaryCenterHeuristic(SPDirectedGraph& graph, SPLayering& layering);
        static void reduceNumCrossWithMedianHeuristic(SPDirectedGraph& graph, SPLayering& layering);
        static void reduceNumCrossWithPairwiseSwitchHeuristic(SPDirectedGraph& graph, SPLayering& layering);
    };
}

#endif //GRAPHLAYOUT_CROSS_MINIMIZATION_H
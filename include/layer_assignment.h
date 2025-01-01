#ifndef GRAPHLAYOUT_LAYER_ASSIGNMENT_H
#define GRAPHLAYOUT_LAYER_ASSIGNMENT_H

#include <vector>
#include "graph_def.h"

namespace graph_layout {
    enum class LayerAssignmentMethod {
        TOPOLOGICAL,
        MIN_NUM_OF_LAYERS,
        // Gansner, E. R., Koutsofios, E., North, S. C., & Vo, K. P. (2003, May).
        // A Technique for Drawing Directed Graphs.
        GANSNER_93,
        MIN_TOTAL_EDGE_LENGTH,
    };

    class LayerAssignment {
    public:
        explicit LayerAssignment(LayerAssignmentMethod method = LayerAssignmentMethod::MIN_TOTAL_EDGE_LENGTH);
        ~LayerAssignment() = default;

        void setMethod(LayerAssignmentMethod method);

        void setMinEdgeLengths(std::unordered_map<int, int> &&);
        void clearMinEdgeLengths();
        [[nodiscard]] int minEdgeLength(int) const;

        // Input graph must be a DAG.
        [[nodiscard]] std::vector<int> rankVertices(SimpleDirectedGraph &) const;

        static long long calcTotalEdgeLength(const SimpleDirectedGraph &, const std::vector<int> &);
    private:
        LayerAssignmentMethod _method;
        std::unordered_map<int, int> _minEdgeLens;

        std::vector<int> rankVerticesTopological(SimpleDirectedGraph &) const;
        std::vector<int> rankVerticesNetworkSimplex(SimpleDirectedGraph &) const;

    protected:
        static constexpr int NO_PARENT = -1;

        std::pair<int, std::vector<int>> gansner93InitFeasibleTree(SimpleDirectedGraph &, std::vector<int> &ranks) const;
        std::pair<int, std::vector<int>> gansner93ComputeCutValues(
            SimpleDirectedGraph &,
            SimpleDirectedGraph &,
            int root,
            const std::vector<int> &parents) const;
    };
}

#endif //GRAPHLAYOUT_LAYER_ASSIGNMENT_H
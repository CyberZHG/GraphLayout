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
        NETWORK_SIMPLEX,
        MIN_TOTAL_EDGE_LENGTH,
    };

    class LayerAssignment {
    public:
        explicit LayerAssignment(LayerAssignmentMethod method = LayerAssignmentMethod::TOPOLOGICAL);
        ~LayerAssignment() = default;

        void setMinimumEdgeLengths(std::unordered_map<int, int> &&);
        void cleanMinimumEdgeLengths();
        [[nodiscard]] int minEdgeLength(int) const;

        // Input graph must be a DAG.
        [[nodiscard]] std::vector<int> rankVertices(SimpleDirectedGraph &) const;

        static long long calcTotalEdgeLength(const SimpleDirectedGraph &, const std::vector<int> &);
    private:
        LayerAssignmentMethod _method;
        std::unordered_map<int, int> _minEdgeLens;

        std::vector<int> rankVerticesTopological(SimpleDirectedGraph &) const;
        std::vector<int> rankVerticesNetworkSimplex(SimpleDirectedGraph &) const;
    };
}

#endif //GRAPHLAYOUT_LAYER_ASSIGNMENT_H
#ifndef GRAPHLAYOUT_LAYER_ASSIGNMENT_H
#define GRAPHLAYOUT_LAYER_ASSIGNMENT_H

#include <vector>
#include "graph_def.h"

namespace graph_layout {
    enum class LayerAssignmentMethod {
        TOPOLOGICAL,
    };

    class LayerAssignment {
    public:
        explicit LayerAssignment(LayerAssignmentMethod method = LayerAssignmentMethod::TOPOLOGICAL);
        ~LayerAssignment() = default;

        // Input graph must be a DAG.
        [[nodiscard]] std::vector<int> rankVertices(SimpleDirectedGraph &) const;
    private:
        LayerAssignmentMethod _method;

        static std::vector<int> rankVerticesTopological(SimpleDirectedGraph &);
    };
}

#endif //GRAPHLAYOUT_LAYER_ASSIGNMENT_H
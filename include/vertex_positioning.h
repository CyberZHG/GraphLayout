#ifndef GRAPHLAYOUT_VERTEX_POSITIONING_H
#define GRAPHLAYOUT_VERTEX_POSITIONING_H

#include "graph_def.h"
#include "cross_minimization.h"

namespace graph_layout {

    enum class VertexPositioningMethod {
        // Brandes, U., & Köpf, B. (2001, September).
        // Fast and simple horizontal coordinate assignment.
        // In International Symposium on Graph Drawing (pp. 31-44). Berlin, Heidelberg: Springer Berlin Heidelberg.
        BRANDES_KOPF,
    };

    class VertexPositioning {
    public:
        explicit VertexPositioning(VertexPositioningMethod method = VertexPositioningMethod::BRANDES_KOPF);
        ~VertexPositioning() = default;

        using RootVec = std::vector<int>;
        using AlignVec = std::vector<int>;

        static constexpr double DEFAULT_VERTEX_MARGIN = 1.0;
        static constexpr double DEFAULT_LAYER_MARGIN = 1.0;
        static constexpr double DEFAULT_VERTEX_SIZE = 1.0;

        void setVertexMargin(double margin);
        void setLayerMargin(double margin);
        void setVertexSizes(double size);
        void setVertexSizes(std::vector<double> &&sizes);

        [[nodiscard]] std::vector<std::pair<double, double>> assignCoordinates(SPDirectedGraph &, SPLayering &) const;

    protected:
        static void sortIncidentEdges(SPDirectedGraph &, SPLayering &);
        static std::pair<RootVec, AlignVec> verticalAlignment(SPDirectedGraph &, SPLayering &, bool forward, bool leftToRight);
        // std::vector<double> horizontalCompaction(SPDirectedGraph &, RootVec &, AlignVec &, bool forward, bool leftToRight);

    private:
        VertexPositioningMethod _method;

        double _vertexMargin = DEFAULT_VERTEX_MARGIN;
        double _layerMargin = DEFAULT_LAYER_MARGIN;
        double _vertexSize = DEFAULT_VERTEX_SIZE;
        std::vector<double> _vertexSizes;

        [[nodiscard]] double vertexSizeAt(int index) const;
    };
}

#endif //GRAPHLAYOUT_VERTEX_POSITIONING_H
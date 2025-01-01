#ifndef GRAPHLAYOUT_GRAPH_DEF_H
#define GRAPHLAYOUT_GRAPH_DEF_H

#include <vector>
#include <unordered_set>

namespace graph_layout {
    struct SimpleEdge {
        int id, u, v;
    };

    class SimpleDirectedGraph {
    public:
        explicit SimpleDirectedGraph(size_t num_vertices);
        ~SimpleDirectedGraph() = default;

        [[nodiscard]] size_t numVertices() const { return _numVertices; }
        [[nodiscard]] size_t numEdges() const { return _edges.size(); }
        [[nodiscard]] const std::vector<SimpleEdge> &edges() const { return _edges; }

        void addEdge(const SimpleEdge &edge);
        void addEdge(int u, int v);
        void addOutEdges(int u, const std::vector<int> &vertices);

        void disableSelfCycleEdges();
        void enableSelfCycleEdges();

        void reverseEdges(const std::unordered_set<int> &ids);
        void reverseEdgesBack();

        const std::unordered_map<int, size_t> &getEdgeIdToIndexMap();
        const std::vector<int> &getInDegrees();
        const std::vector<int> &getOutDegrees();
        const std::vector<std::vector<int>> &getInVertices();
        const std::vector<std::vector<int>> &getOutVertices();
        const std::vector<std::vector<int>> &getInEdges();
        const std::vector<std::vector<int>> &getOutEdges();
        bool hasCycle();

    private:
        size_t _numVertices{};
        std::vector<SimpleEdge> _edges;

        bool _edgeIdToIndexMapInitialized = false;
        std::unordered_map<int, size_t> _edgeIdToIndexMap;

        bool _degreesInitialized = false;
        std::vector<int> _inDegrees;
        std::vector<int> _outDegrees;

        bool _inOutVerticesInitialized = false;
        std::vector<std::vector<int>> _inVertices;
        std::vector<std::vector<int>> _outVertices;

        bool _inOutEdgesInitialized = false;
        std::vector<std::vector<int>> _inEdges;
        std::vector<std::vector<int>> _outEdges;

        bool _hasCycleInitialized = false;
        bool _hasCycle = false;

        std::vector<SimpleEdge> _selfCycleEdges;

        std::unordered_set<int> _reverseIds;

        void resetInitialization();
        void initDegrees();
        void initInOutVertices();
        void initInOutEdges();
        void initHasCycle();
    };
}

#endif //GRAPHLAYOUT_GRAPH_DEF_H
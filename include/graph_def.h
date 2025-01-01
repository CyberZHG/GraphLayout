#ifndef GRAPHLAYOUT_GRAPH_DEF_H
#define GRAPHLAYOUT_GRAPH_DEF_H

#include <vector>
#include <unordered_set>

namespace graph_layout {
    class EdgeIterationWithIDs;

    struct SimpleEdge {
        int id, u, v;

        bool operator==(const SimpleEdge &) const;
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
        SimpleEdge &getEdge(int id);

        bool operator==(const SimpleDirectedGraph &other) const;

        void disableSelfCycleEdges();
        void enableSelfCycleEdges();

        void reverseEdges(const std::unordered_set<int> &ids);
        void reverseEdgesBack();

        void sortEdgesById();
        const std::unordered_map<int, size_t> &getEdgeIdToIndexMap();
        const std::vector<int> &getInDegrees();
        const std::vector<int> &getOutDegrees();
        const std::vector<std::vector<int>> &getInVertices();
        const std::vector<std::vector<int>> &getOutVertices();
        const std::vector<std::vector<int>> &getInEdgeIds();
        const std::vector<std::vector<int>> &getOutEdgeIds();
        EdgeIterationWithIDs getInEdges(int v);
        EdgeIterationWithIDs getOutEdges(int u);
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

    class EdgeIterationWithIDs {
    public:
        EdgeIterationWithIDs(SimpleDirectedGraph &graph, const std::vector<int>& ids) : _graph(graph), _ids(ids) {}

        class iterator {
        public:
            explicit iterator(SimpleDirectedGraph &graph, const std::vector<int>& ids, const int index) : _graph(graph), _ids(ids), _index(index) {}
            const SimpleEdge &operator*() const;
            iterator& operator++() { ++_index; return *this; }
            bool operator!=(const iterator& other) const { return _index != other._index; }
        private:
            SimpleDirectedGraph &_graph;
            const std::vector<int> &_ids;
            int _index;
        };

        [[nodiscard]] iterator begin() const { return iterator(_graph, _ids, 0); }
        [[nodiscard]] iterator end() const { return iterator(_graph, _ids, static_cast<int>(_ids.size())); }
    private:
        SimpleDirectedGraph &_graph;
        const std::vector<int> &_ids;
    };

    class RandomSimpleDirectedGraphGenerator {
    public:
        explicit RandomSimpleDirectedGraphGenerator(const int maxNumVertices) : _minNumVertices(1), _maxNumVertices(maxNumVertices), _allowSelfCycle(false) {}

        [[nodiscard]] SimpleDirectedGraph generateGraph() const;
    private:
        int _minNumVertices;
        int _maxNumVertices;
        bool _allowSelfCycle;
    };

    class GraphComponentSplitter {
    public:
        std::vector<SimpleDirectedGraph> &splitGraph(const SimpleDirectedGraph &graph);
        [[nodiscard]] SimpleDirectedGraph mergeBack() const;
    private:
        std::vector<SimpleDirectedGraph> _graphs;
        std::vector<std::vector<int>> _groups;
    };
}

#endif //GRAPHLAYOUT_GRAPH_DEF_H
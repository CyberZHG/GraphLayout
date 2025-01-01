#include <random>
#include "include/graph_def_utils.h"
using namespace std;
using namespace graph_layout;

SimpleDirectedGraph RandomGraphGenerator::generateGraph() const {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> numVerticesDist(_minNumVertices, _maxNumVertices);
    const size_t n = numVerticesDist(gen);
    SimpleDirectedGraph graph(n);
    uniform_int_distribution<> numEdgesDist(0, static_cast<int>(n * n));
    uniform_int_distribution<> verticeIndexDist(0, static_cast<int>(n - 1));
    const size_t m = numEdgesDist(gen);
    for (size_t edgeIndex = 0; edgeIndex < m; ++edgeIndex) {
        const int &u = verticeIndexDist(gen);
        const int &v = verticeIndexDist(gen);
        if (!_allowSelfCycle && u == v) {
            continue;
        }
        graph.addEdge(u, v);
    }
    return graph;
}
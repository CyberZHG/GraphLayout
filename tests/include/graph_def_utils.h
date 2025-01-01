#ifndef GRAPHLAYOUT_GRAPH_DEF_UTILS_H
#define GRAPHLAYOUT_GRAPH_DEF_UTILS_H
#include "graph_def.h"

class RandomGraphGenerator {
public:
    explicit RandomGraphGenerator(const int maxNumVertices) : _minNumVertices(2), _maxNumVertices(maxNumVertices), _allowSelfCycle(false) {}

    [[nodiscard]] graph_layout::SimpleDirectedGraph generateGraph() const;
private:
    int _minNumVertices;
    int _maxNumVertices;
    bool _allowSelfCycle;
};


#endif //GRAPHLAYOUT_GRAPH_DEF_UTILS_H

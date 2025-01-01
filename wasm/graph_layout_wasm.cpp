#include "graph_layout.h"
#include <memory>
#include <emscripten/bind.h>
using namespace std;
using namespace emscripten;
using namespace graph_layout;


shared_ptr<SPDirectedGraph> createDirectedGraph(size_t numVertices) {
    return make_shared<SPDirectedGraph>(numVertices);
}

EMSCRIPTEN_BINDINGS(GraphLayoutWASM) {
    emscripten::function("createDirectedGraph", &createDirectedGraph);
    class_<SPDirectedGraph>("SPDirectedGraph")
        .constructor<size_t>()
        .smart_ptr<shared_ptr<SPDirectedGraph>>("SPDirectedGraph")
        .function("addEdge", select_overload<int(int, int)>(&SPDirectedGraph::addEdge))
        ;
    class_<DirectedGraphHierarchicalLayout>("DirectedGraphHierarchicalLayout")
        .constructor<>()
        .function("setGraph", &DirectedGraphHierarchicalLayout::setGraph)
        .function("layoutGraph", &DirectedGraphHierarchicalLayout::layoutGraph)
        .function("render", select_overload<string()const>(&DirectedGraphHierarchicalLayout::render))
        ;
}

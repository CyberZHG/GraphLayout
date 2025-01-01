#include "graph_layout.h"
using namespace graph_layout;

int main() {
    const auto graph = std::make_shared<SPDirectedGraph>(8);
    graph->addEdge(0, 1);
    graph->addEdge(1, 2); graph->addEdge(1, 3);
    graph->addEdge(2, 5); graph->addEdge(2, 6);
    graph->addEdge(3, 4);
    graph->addEdge(4, 5); graph->addEdge(4, 6);
    graph->addEdge(5, 7); graph->addEdge(6, 7);
    graph->addEdge(7, 0);
    DirectedGraphHierarchicalLayout layout;
    layout.setGraph(graph);
    layout.layoutGraph();
    layout.initVertexLabelsWithNumericalValues(0);
    layout.drawSVG("demo.svg");
    return 0;
}
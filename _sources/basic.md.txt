Basic Concepts
==============

The hierarchical layout for directed graphs attempts to minimize edge lengths and crossings. For each layout, a graph must first be created, with the number of vertices specified at creation time. Vertex indices start from 0, and each edge connects two vertices. You must call the layout function before generating the SVG.

`````{tab-set}
````{tab-item} Python
```python
from sp_graph_layout import DirectedGraphHierarchicalLayout

layout = DirectedGraphHierarchicalLayout()
graph = layout.create_graph(8)
# fmt: off
graph.add_edges([
    [0, 1], [1, 2], [1, 3], [2, 5], [2, 6],
    [3, 4], [4, 5], [4, 6], [5, 7], [6, 7],
    [7, 0],
])
# fmt: on
layout.init_vertex_labels_with_numerical_values(0)
layout.layout_graph()
svg = layout.render()
```
````
````{tab-item} JavaScript
```javascript
import { DirectedGraphHierarchicalLayout } from 'sp-graph-layout';

const layout = new DirectedGraphHierarchicalLayout();
const graph = layout.createGraph(8);
graph.addEdges([
    [0, 1], [1, 2], [1, 3], [2, 5], [2, 6],
    [3, 4], [4, 5], [4, 6], [5, 7], [6, 7],
    [7, 0],
]);
layout.initVertexLabelsWithNumericalValues(0);
layout.layoutGraph();
const svg = layout.render();
````

````{tab-item} C++
```c++
#include "graph_layout.h"
using namespace graph_layout;

int main() {
    DirectedGraphHierarchicalLayout layout;
    const auto graph = layout.createGraph(8);
    graph->addEdges({
        {0, 1}, {1, 2}, {1, 3}, {2, 5}, {2, 6},
        {3, 4}, {4, 5}, {4, 6}, {5, 7}, {6, 7},
        {7, 0},
    });
    layout.initVertexLabelsWithNumericalValues(0);
    layout.layoutGraph();
    layout.render("directed.svg");
    return 0;
}
````
`````

The generated SVG:

![](_static/images/basic/directed.svg)

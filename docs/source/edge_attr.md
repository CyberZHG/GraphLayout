Edge Attributes
===============

## Label

`````{tab-set}
````{tab-item} Python
```python
from sp_graph_layout import (
    AttributeRankDir,
    AttributeShape,
    DirectedGraphHierarchicalLayout,
)

layout = DirectedGraphHierarchicalLayout()
graph = layout.create_graph(3)
graph.add_edges([[0, 1], [1, 2], [2, 0]])
layout.set_vertex_labels(["Eat", "Sleep", "Play"])
for u in range(3):
    layout.attributes().set_vertex_shape(u, AttributeShape.ELLIPSE)
layout.attributes().set_rank_dir(AttributeRankDir.LEFT_TO_RIGHT)
layout.layout_graph()
svg = layout.render()
```
````
````{tab-item} JavaScript
```javascript
import {DirectedGraphHierarchicalLayout, AttributeRankDir} from 'sp-graph-layout';

const layout = new DirectedGraphHierarchicalLayout();
const graph = layout.createGraph(3);
graph.addEdges([[0, 1], [1, 2], [2, 2]]);
layout.initVertexLabelsWithNumericalValues(0);
layout.setEdgeLabel(0, "A");
layout.setEdgeLabel(1, "B");
layout.setEdgeLabel(2, "C");
layout.attributes().setRankDir(AttributeRankDir.LEFT_TO_RIGHT);
layout.layoutGraph();
const svg = layout.render();
````

````{tab-item} C++
```c++
#include "graph_layout.h"
using namespace graph_layout;

int main() {
    DirectedGraphHierarchicalLayout layout;
    const auto graph = layout.createGraph(3);
    graph->addEdges({{0, 1}, {1, 2}, {2, 2}});
    layout.initVertexLabelsWithNumericalValues(0);
    layout.setEdgeLabel(0, "A");
    layout.setEdgeLabel(1, "B");
    layout.setEdgeLabel(2, "C");
    layout.attributes().setRankDir(AttributeRankDir::LEFT_TO_RIGHT);
    layout.layoutGraph();
    layout.render("label.svg");
    return 0;
}
````
`````

The generated SVG:

![](_static/images/edge_attr/label.svg)

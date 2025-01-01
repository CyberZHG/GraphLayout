Vertex Attributes
=================

## Shape

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
layout.attributes().set_vertex_shape(0, AttributeShape.DOUBLE_CIRCLE)
layout.attributes().set_vertex_shape(1, AttributeShape.RECT)
layout.attributes().set_vertex_shape(2, AttributeShape.ELLIPSE)
layout.attributes().set_rank_dir(AttributeRankDir.LEFT_TO_RIGHT)
layout.init_vertex_labels_with_numerical_values(0)
layout.layout_graph()
svg = layout.render()
```
````
````{tab-item} JavaScript
```javascript
import {DirectedGraphHierarchicalLayout, AttributeRankDir, AttributeShape, buildVectorString} from 'sp-graph-layout';

const layout = new DirectedGraphHierarchicalLayout();
const graph = layout.createGraph(3);
graph.addEdges([[0, 1], [1, 2], [2, 0]]);
layout.attributes().setVertexShape(0, AttributeShape.DOUBLE_CIRCLE);
layout.attributes().setVertexShape(1, AttributeShape.RECT);
layout.attributes().setVertexShape(2, AttributeShape.ELLIPSE);
layout.attributes().setRankDir(AttributeRankDir.LEFT_TO_RIGHT);
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

![](_static/images/vertex_attr/shape.svg)

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
import {DirectedGraphHierarchicalLayout, AttributeRankDir, AttributeShape, buildVectorString} from 'sp-graph-layout';

const layout = new DirectedGraphHierarchicalLayout();
const graph = layout.createGraph(3);
graph.addEdges([[0, 1], [1, 2], [2, 0]]);
layout.setVertexLabels(buildVectorString(["Eat", "Sleep", "Play"]));
for (let u = 0; u < 3; ++u) {
    layout.attributes().setVertexShape(u, AttributeShape.ELLIPSE);
}
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
    graph->addEdges({{0, 1}, {1, 2}, {2, 0}});
    layout.setVertexLabels({"Eat", "Sleep", "Play"});
    for (int u = 0; u < 3; ++u) {
        layout.attributes().setVertexShape(u, AttributeShape::ELLIPSE);
    }
    layout.attributes().setRankDir(AttributeRankDir::LEFT_TO_RIGHT);
    layout.layoutGraph();
    layout.render("label.svg");
    return 0;
}
````
`````

The generated SVG:

![](_static/images/vertex_attr/label.svg)

Vertex Attributes
=================

## Shape

`````{tab-set}
````{tab-item} Python
```python
from sp_graph_layout import DirectedGraphHierarchicalLayout


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
from sp_graph_layout import DirectedGraphHierarchicalLayout


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

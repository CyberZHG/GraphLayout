Graph Attributes
================

## Rank Direction

The default direction of the graph layout is top to bottom. The direction can be changed by setting `rankDir`.

`````{tab-set}
````{tab-item} Python
```python
from sp_graph_layout import AttributeRankDir, DirectedGraphHierarchicalLayout

rank_dirs = [
    AttributeRankDir.TOP_TO_BOTTOM,
    AttributeRankDir.BOTTOM_TO_TOP,
    AttributeRankDir.LEFT_TO_RIGHT,
    AttributeRankDir.RIGHT_TO_LEFT,
]
for rank_dir in rank_dirs:
    layout = DirectedGraphHierarchicalLayout()
    graph = layout.create_graph(13)
    # fmt: off
    graph.add_edges([
        [0, 1], [1, 2], [1, 3], [2, 5], [2, 6],
        [3, 4], [4, 5], [4, 6], [5, 7], [6, 7],
        [7, 0], [1, 0],
        [8, 8], [8, 9], [8, 10], [9, 12], [10, 12],
        [11, 12]
    ])
    # fmt: on
    layout.init_vertex_labels_with_numerical_values(0)
    layout.attributes().set_rank_dir(rank_dir)
    layout.layout_graph()
    svg = layout.render()
```
````
````{tab-item} JavaScript
```javascript
import { DirectedGraphHierarchicalLayout, AttributeRankDir } from 'sp-graph-layout';

const rankDirs = [
    AttributeRankDir.TOP_TO_BOTTOM,
    AttributeRankDir.BOTTOM_TO_TOP,
    AttributeRankDir.LEFT_TO_RIGHT,
    AttributeRankDir.RIGHT_TO_LEFT,
];
for (const rankDir of rankDirs) {
    const layout = new DirectedGraphHierarchicalLayout();
    const graph = layout.createGraph(13);
    graph.addEdges([
        [0, 1], [1, 2], [1, 3], [2, 5], [2, 6],
        [3, 4], [4, 5], [4, 6], [5, 7], [6, 7],
        [7, 0], [1, 0],
        [8, 8], [8, 9], [8, 10], [9, 12], [10, 12],
        [11, 12]
    ]);
    layout.initVertexLabelsWithNumericalValues(0);
    layout.attributes().setRankDir(rankDir);
    layout.layoutGraph();
    const svg = layout.render();
}
````

````{tab-item} C++
```c++
#include "graph_layout.h"
using namespace graph_layout;

int main() {
    const auto rankDirs = {
        AttributeRankDir::TOP_TO_BOTTOM,
        AttributeRankDir::BOTTOM_TO_TOP,
        AttributeRankDir::LEFT_TO_RIGHT,
        AttributeRankDir::RIGHT_TO_LEFT,
    };
    for (const auto& rankDir : rankDirs) {
        DirectedGraphHierarchicalLayout layout;
        const auto graph = layout.createGraph(13);
        graph->addEdges({
            {0, 1}, {1, 2}, {1, 3}, {2, 5}, {2, 6},
            {3, 4}, {4, 5}, {4, 6}, {5, 7}, {6, 7},
            {7, 0}, {1, 0},
            {8, 8}, {8, 9}, {8, 10}, {9, 12}, {10, 12},
            {11, 12}
        });
        layout.initVertexLabelsWithNumericalValues(0);
        layout.attributes().setRankDir(rankDir);
        layout.layoutGraph();
        layout.render(OUTPUT_DIR + std::format("rank_dir_{}.svg", rankDir));
    }
    return 0;
}
````
`````

The generated SVGs:

|                 Top to Bottom                  |                 Bottom to Top                  |
|:----------------------------------------------:|:----------------------------------------------:|
| ![](_static/images/graph_attr/rank_dir_tb.svg) | ![](_static/images/graph_attr/rank_dir_bt.svg) |

|                 Left to Right                  |                 Right to Left                  |
|:----------------------------------------------:|:----------------------------------------------:|
| ![](_static/images/graph_attr/rank_dir_lr.svg) | ![](_static/images/graph_attr/rank_dir_rl.svg) |

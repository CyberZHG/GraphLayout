from sp_graph_layout import DirectedGraphHierarchicalLayout

from .utils import compare_svg


def test_directed():
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
    compare_svg("basic", "directed", svg)

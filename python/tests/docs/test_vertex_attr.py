from sp_graph_layout import (
    AttributeRankDir,
    AttributeShape,
    DirectedGraphHierarchicalLayout,
)

from .utils import compare_svg


def test_shape():
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
    compare_svg("vertex_attr", "shape", svg)


def test_label():
    layout = DirectedGraphHierarchicalLayout()
    graph = layout.create_graph(3)
    graph.add_edges([[0, 1], [1, 2], [2, 0]])
    layout.set_vertex_labels(["Eat", "Sleep", "Play"])
    for u in range(3):
        layout.attributes().set_vertex_shape(u, AttributeShape.ELLIPSE)
    layout.attributes().set_rank_dir(AttributeRankDir.LEFT_TO_RIGHT)
    layout.layout_graph()
    svg = layout.render()
    compare_svg("vertex_attr", "label", svg)

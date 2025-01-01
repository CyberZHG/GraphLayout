from sp_graph_layout import AttributeRankDir, DirectedGraphHierarchicalLayout

from .utils import compare_svg


def test_label():
    layout = DirectedGraphHierarchicalLayout()
    graph = layout.create_graph(3)
    graph.add_edges([[0, 1], [1, 2], [2, 2]])
    layout.init_vertex_labels_with_numerical_values(0)
    layout.set_edge_label(0, "A")
    layout.set_edge_label(1, "B")
    layout.set_edge_label(2, "C")
    layout.attributes().set_rank_dir(AttributeRankDir.LEFT_TO_RIGHT)
    layout.layout_graph()
    svg = layout.render()
    compare_svg("edge_attr", "label", svg)

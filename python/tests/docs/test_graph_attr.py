from sp_graph_layout import AttributeRankDir, DirectedGraphHierarchicalLayout

from .utils import compare_svg


def test_rank_dir():
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
        compare_svg("graph_attr", f"rank_dir_{rank_dir}", svg)

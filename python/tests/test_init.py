from sp_graph_layout import DirectedGraphHierarchicalLayout, SPDirectedGraph


def test_init():
    layout = DirectedGraphHierarchicalLayout()
    graph = layout.create_graph(2)
    graph.add_edge(0, 1)
    layout.set_graph(graph)
    layout.layout_graph()
    layout.to_svg("init.svg")

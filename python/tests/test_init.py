from sp_graph_layout import DirectedGraphHierarchicalLayout, SPDirectedGraph


def test_init():
    graph = SPDirectedGraph(2)
    graph.add_edge(0, 1)
    layout = DirectedGraphHierarchicalLayout()
    layout.set_graph(graph)
    layout.layout_graph()
    layout.to_svg("init.svg")

#include <pybind11/pybind11.h>
#include <string>
#include <memory>
#include "graph_layout.h"
using namespace std;
using namespace graph_layout;

namespace py = pybind11;

PYBIND11_MODULE(_core, m, py::mod_gil_not_used()) {
    py::class_<SPDirectedGraph, shared_ptr<SPDirectedGraph>>(m, "SPDirectedGraph")
        .def(py::init<size_t>(), py::arg("num_vertices"))
        .def("add_edge", py::overload_cast<int, int>(&SPDirectedGraph::addEdge), py::arg("x_margin"), py::arg("y_margin"))
    ;
    py::class_<DirectedGraphHierarchicalLayout>(m, "DirectedGraphHierarchicalLayout")
        .def(py::init<>())
        .def("set_graph", &DirectedGraphHierarchicalLayout::setGraph, py::arg("graph"))
        .def("layout_graph", &DirectedGraphHierarchicalLayout::layoutGraph)
        .def("render", py::overload_cast<>(&DirectedGraphHierarchicalLayout::render, py::const_))
        .def("to_svg", py::overload_cast<const string&>(&DirectedGraphHierarchicalLayout::render, py::const_), py::arg("file_path"))
    ;
}

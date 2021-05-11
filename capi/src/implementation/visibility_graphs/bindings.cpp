#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"
#pragma ide diagnostic ignored "misc-redundant-expression"

//
// Created by James.Balajan on 30/03/2021.
//

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "datastructures/graph/graph.hpp"
#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "visgraph/visgraph_generator.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(_vis_graph, m) {
    m.doc() = "CAPI visibility graphs";

    py::class_<Coordinate>(m, "VisGraphCoord")
        .def(py::init<double, double>())
        .def_property_readonly("latitude", &Coordinate::get_latitude)
        .def_property_readonly("longitude", &Coordinate::get_longitude)
        .def("__repr__", &Coordinate::to_string_representation)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(-py::self)
        .def(py::self - py::self)
        .def(py::self + py::self)
        .def(py::self * double())
        .def(py::self / double())
        .def("dot_product", &Coordinate::dot_product)
        .def("cross_product_magnitude", &Coordinate::cross_product_magnitude)
        .def("magnitude_squared", &Coordinate::magnitude_squared)
        .def("magnitude", &Coordinate::magnitude);

    py::class_<Polygon>(m, "VisGraphPolygon")
        .def(py::init<const std::vector<Coordinate> &>())
        .def_property_readonly("vertices", &Polygon::get_vertices)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__repr__", &Polygon::to_string_representation);

    py::class_<Graph>(m, "VisGraph")
        .def(py::init<const std::vector<Polygon> &>())
        .def("__repr__", &Graph::to_string_representation)
        .def("has_edge", &Graph::has_edge)
        .def("add_edge", &Graph::add_edge)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("shortest_path", &Graph::shortest_path)
        .def_property_readonly("vertices", &Graph::get_vertices)
        .def_property_readonly("polygons", &Graph::get_polygons)
        .def("get_neighbors", &Graph::get_neighbors)
        .def("serialize_to_file", &Graph::serialize_to_file);

    m.def("generate_visgraph", &VisgraphGenerator::generate, "Generates a visgraph from the supplied polygons");

    m.def("load_graph_from_file", &Graph::load_from_file, "Loads serialized graph from file");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
#pragma clang diagnostic pop
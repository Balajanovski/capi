#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"
#pragma ide diagnostic ignored "misc-redundant-expression"

//
// Created by James.Balajan on 30/03/2021.
//

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <string>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "datastructures/graph/graph.hpp"
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
        .def(py::init<const std::vector<Coordinate>&>())
        .def_property_readonly_static("vertices", &Polygon::get_vertices)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__repr__", &Polygon::to_string_representation);

    py::class_<Graph>(m, "VisGraph")
        .def(py::init<>())
        .def("__repr__", &Graph::to_string_representation)
        .def("has_edge", &Graph::has_edge);

    m.def("generate_visgraph", &VisgraphGenerator::generate, "Generates a visgraph from the supplied polygons");


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
#pragma clang diagnostic pop
//
// Created by James.Balajan on 30/03/2021.
//

#include <pybind11/pybind11.h>
#include <string>

#include "types/coordinate.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;


PYBIND11_MODULE(_vis_graph, m) {
    m.doc() = "CAPI visibility graphs";

    py::class_<Coordinate>(m, "VisGraphCoord")
        .def(py::init<double, double>())
        .def("get_latitude", &Coordinate::get_latitude)
        .def("get_longitude", &Coordinate::get_longitude)
        .def("__repr__", [](const Coordinate& coord) {
                return std::string("Coordinate (latitude: ") +
                       coord.get_latitude() +
                       " longitude: " +
                       coord.get_longitude() +
                       ")";
            });

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
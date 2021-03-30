//
// Created by James.Balajan on 30/03/2021.
//

#include <pybind11/pybind11.h>
#include "src/math.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(_vis_graph, m) {
    m.doc() = "CAPI visibility graphs";
    m.def("add", &add);
    m.def("subtract", &subtract);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
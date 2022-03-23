#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"
#pragma ide diagnostic ignored "misc-redundant-expression"

//
// Created by James.Balajan on 30/03/2021.
//

#include <cmath>
#include <memory>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "datastructures/graph/graph.hpp"
#include "serialization/graph_serializer.hpp"
#include "shortest_path/shortest_path_computer.hpp"
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

    py::class_<Graph, std::shared_ptr<Graph>>(m, "VisGraph")
        .def(py::init<const std::vector<Polygon> &>())
        .def("__repr__", &Graph::to_string_representation)
        .def("has_edge", &Graph::has_edge)
        .def("has_vertex", &Graph::has_vertex)
        .def("add_edge", &Graph::add_edge)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def_property_readonly("vertices", &Graph::get_vertices)
        .def_property_readonly("polygons", &Graph::get_polygons)
        .def("get_neighbors", &Graph::get_neighbors);

    py::class_<ShortestPathComputer>(m, "VisGraphShortestPathComputer")
        .def(py::init<const std::shared_ptr<Graph> &>())
        .def(
            "shortest_path",
            [](ShortestPathComputer &self, const Coordinate &source, const Coordinate &destination,
               double maximum_distance_to_search_from_source, bool correct_vertices_on_land,
               double a_star_greediness_weighting) {
                return self.shortest_path(source, destination, maximum_distance_to_search_from_source,
                                          correct_vertices_on_land, a_star_greediness_weighting);
            },
            py::arg("source"), py::arg("destination"), py::arg("maximum_distance_to_search_from_source") = INFINITY,
            py::arg("correct_vertices_on_land") = false, py::arg("a_star_greediness_weighting") = 1.0)
        .def(
            "shortest_paths",
            [](ShortestPathComputer &self, const std::vector<std::pair<Coordinate, Coordinate>> &source_dest_pairs,
               double maximum_distance_to_search_from_source, bool correct_vertices_on_land,
               double a_star_greediness_weighting) {
                return self.shortest_paths(source_dest_pairs, maximum_distance_to_search_from_source,
                                           correct_vertices_on_land, a_star_greediness_weighting);
            },
            py::arg("source_dest_pairs"), py::arg("maximum_distance_to_search_from_source") = INFINITY,
            py::arg("correct_vertices_on_land") = false, py::arg("a_star_greediness_weighting") = 1.0);

    py::class_<BatchInterpolateResult>(m, "VisGraphBatchInterpolateResult")
        .def_readwrite("path", &BatchInterpolateResult::path)
        .def_readwrite("error_msg", &BatchInterpolateResult::error_msg);

    m.def("generate_visgraph", &VisgraphGenerator::generate, "Generates a visgraph from the supplied polygons");
    m.def("generate_visgraph_with_shuffled_range", &VisgraphGenerator::generate_with_shuffled_range,
          "Generates a visgraph from the supplied polygons using only a certain range of vertices (after shuffling)");

    m.def("load_graph_from_file", &GraphSerializer::deserialize_from_file, "Loads serialized graph from file");
    m.def("save_graph_to_file", &GraphSerializer::serialize_to_file, "Serializes graph to file");
    m.def("merge_graphs", &merge_graphs, "Merges graphs into one");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
#pragma clang diagnostic pop
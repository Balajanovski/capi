//
// Created by James.Balajan on 31/03/2021.
//

#include <algorithm>
#include <random>
#include <stdexcept>

#include "coordinate_periodicity/coordinate_periodicity.hpp"
#include "visgraph_generator.hpp"
#include "vistree_generator.hpp"

VisgraphGenerator::VisgraphGenerator() = default;

std::shared_ptr<Graph> VisgraphGenerator::generate(const std::vector<Polygon> &polygons) {
    auto polygon_vertices = VisgraphGenerator::polygon_vertices(polygons);
    auto visgraph = std::make_shared<Graph>(polygons);
    auto vistree_gen = VistreeGenerator(make_polygons_periodic(polygons));

#pragma omp parallel for shared(visgraph, polygon_vertices, vistree_gen) default(none) schedule(dynamic)
    for (size_t i = 0; i < polygon_vertices.size(); ++i) { // NOLINT
        const auto visible_vertices = vistree_gen.get_visible_vertices(polygon_vertices[i], true);

        for (const auto &visible_vertex : visible_vertices) {
            visgraph->add_edge(polygon_vertices[i], visible_vertex.coord, visible_vertex.is_visible_across_meridian);
        }
    }

    return visgraph;
}

std::shared_ptr<Graph> VisgraphGenerator::generate_with_shuffled_range(const std::vector<Polygon> &polygons, size_t range_start,
                                                      size_t range_end, unsigned int seed) {
    auto polygon_vertices = VisgraphGenerator::polygon_vertices(polygons);
    auto visgraph = std::make_shared<Graph>(polygons);
    if (polygons.empty()) {
        return visgraph;
    }
    if (range_start < 0 || range_end > polygon_vertices.size() || range_start > range_end) {
        throw std::runtime_error("Improper range for visgraph generation");
    }
    auto vistree_gen = VistreeGenerator(make_polygons_periodic(polygons));

    std::mt19937 gen(seed);
    std::shuffle(polygon_vertices.begin(), polygon_vertices.end(), gen);

#pragma omp parallel for shared(visgraph, vistree_gen, polygon_vertices, range_start, range_end) default(none) schedule(dynamic)
    for (size_t i = range_start; i < range_end; ++i) { // NOLINT
        const auto visible_vertices = vistree_gen.get_visible_vertices(polygon_vertices[i], true);

        for (const auto &visible_vertex : visible_vertices) {
            visgraph->add_edge(polygon_vertices[i], visible_vertex.coord, visible_vertex.is_visible_across_meridian);
        }
    }

    return visgraph;
}

std::vector<Coordinate> VisgraphGenerator::polygon_vertices(const std::vector<Polygon> &polygons) {
    auto vertices = std::vector<Coordinate>();

    for (const auto &polygon : polygons) {
        vertices.reserve(polygon.get_vertices().size());
        vertices.insert(vertices.end(), polygon.get_vertices().begin(), polygon.get_vertices().end());
    }

    return vertices;
}

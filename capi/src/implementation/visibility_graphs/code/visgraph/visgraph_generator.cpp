//
// Created by James.Balajan on 31/03/2021.
//

#include <algorithm>
#include <random>
#include <stdexcept>
#include <indicators/progress_bar.hpp>
#include <omp.h>
#include <fmt/core.h>
#include <sstream>

#include "coordinate_periodicity/coordinate_periodicity.hpp"
#include "visgraph_generator.hpp"
#include "vistree_generator.hpp"

VisgraphGenerator::VisgraphGenerator() = default;

std::shared_ptr<Graph> VisgraphGenerator::generate(const std::vector<Polygon> &polygons) {
    auto polygon_vertices = VisgraphGenerator::polygon_vertices(polygons);
    auto visgraph = std::make_shared<Graph>(polygons);

    const auto periodic_polygons = make_polygons_periodic(polygons);
    auto vistree_gen = VistreeGenerator(periodic_polygons);

    size_t num_vertices = polygon_vertices.size();
    indicators::ProgressBar bar {
        indicators::option::BarWidth{50},
        indicators::option::Start{"["},
        indicators::option::Fill{"="},
        indicators::option::Lead{">"},
        indicators::option::Remainder{" "},
        indicators::option::End{"]"},
        indicators::option::PostfixText{"Generating Visibility Graph"},
        indicators::option::ForegroundColor{indicators::Color::green},
        indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}},
        indicators::option::ShowElapsedTime{true},
        indicators::option::ShowRemainingTime{true},
        indicators::option::Stream{std::cout},
        indicators::option::MaxProgress{num_vertices},
    };

#pragma omp parallel shared(visgraph, polygon_vertices, vistree_gen, num_vertices, bar) default(none)
    {
        size_t num_threads = omp_get_num_threads();

        if (omp_get_thread_num() == 0) {
            bar.set_option(indicators::option::MaxProgress{num_vertices / num_threads});
            bar.set_option(indicators::option::PostfixText(fmt::format("Generating VisGraph ({} threads)", num_threads)));
        }

#pragma omp for schedule(dynamic)
        for (size_t i = 0; i < num_vertices; ++i) { // NOLINT
            const auto visible_vertices = vistree_gen.get_visible_vertices(polygon_vertices[i], true);

            for (const auto &visible_vertex : visible_vertices) {
                visgraph->add_edge(polygon_vertices[i], visible_vertex.coord, visible_vertex.is_visible_across_meridian);
            }

            if (omp_get_thread_num() == 0) {
                bar.tick();
            }
        }
    }

    bar.mark_as_completed();

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

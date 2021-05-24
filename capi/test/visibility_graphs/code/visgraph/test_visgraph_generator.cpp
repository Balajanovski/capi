//
// Created by James.Balajan on 12/04/2021.
//

#include <catch.hpp>
#include <vector>

#include "types/coordinate/coordinate.hpp"
#include "types/polygon/polygon.hpp"
#include "types/visible_vertex/visible_vertex.hpp"
#include "visgraph/visgraph_generator.hpp"

void add_edges(const Coordinate &source, const std::vector<VisibleVertex> &neighbors, Graph &g);

TEST_CASE("Visgraph Generator Normal Case") {
    const auto poly1 = Polygon({
        Coordinate(1, 0),
        Coordinate(0, 1),
        Coordinate(-1, 0),
    });

    const auto poly2 = Polygon({
        Coordinate(5, 0),
        Coordinate(3, 0),
        Coordinate(4, 2),
    });

    const auto visgraph = VisgraphGenerator::generate(std::vector<Polygon>{poly1, poly2});
    const auto full_range_visgraph = VisgraphGenerator::generate_with_shuffled_range(
        std::vector<Polygon>{poly1, poly2}, 0, poly1.get_vertices().size() + poly2.get_vertices().size(), 42);
    const auto not_full_range_visgraph =
        VisgraphGenerator::generate_with_shuffled_range(std::vector<Polygon>{poly1, poly2}, 0, 1, 42);

    auto expected_vis_graph = Graph({poly1, poly2});
    add_edges(Coordinate(5, 0),
              {
                  VisibleVertex{.coord = Coordinate(1, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(3, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(0, 1), .is_visible_across_meridian = true},
                  VisibleVertex{.coord = Coordinate(4, 2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1, 0), .is_visible_across_meridian = false},
              },
              expected_vis_graph);
    add_edges(Coordinate(-1, 0),
              {
                  VisibleVertex{.coord = Coordinate(1, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(3, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(5, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(0, 1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(4, 2), .is_visible_across_meridian = true},
              },
              expected_vis_graph);
    add_edges(Coordinate(3, 0),
              {
                  VisibleVertex{.coord = Coordinate(1, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(5, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(0, 1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(4, 2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1, 0), .is_visible_across_meridian = false},
              },
              expected_vis_graph);
    add_edges(Coordinate(4, 2),
              {
                  VisibleVertex{.coord = Coordinate(1, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(3, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(5, 0), .is_visible_across_meridian = -false},
                  VisibleVertex{.coord = Coordinate(0, 1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1, 0), .is_visible_across_meridian = true},
              },
              expected_vis_graph);
    add_edges(Coordinate(0, 1),
              {
                  VisibleVertex{.coord = Coordinate(1, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(3, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(5, 0), .is_visible_across_meridian = true},
                  VisibleVertex{.coord = Coordinate(4, 2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1, 0), .is_visible_across_meridian = false},
              },
              expected_vis_graph);
    add_edges(Coordinate(1, 0),
              {
                  VisibleVertex{.coord = Coordinate(3, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(5, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(0, 1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(4, 2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1, 0), .is_visible_across_meridian = false},
              },
              expected_vis_graph);

    REQUIRE(visgraph == expected_vis_graph);
    REQUIRE(full_range_visgraph == expected_vis_graph);
    REQUIRE(not_full_range_visgraph != expected_vis_graph);
}

TEST_CASE("Visgraph Generator Surrounded Case") {
    const auto polygon = Polygon({
        Coordinate(1.5, 1),
        Coordinate(-1.5, 1),
        Coordinate(-1.5, -2),
        Coordinate(-1, -1),
        Coordinate(0, 0),
        Coordinate(1, -1),
        Coordinate(1.5, -2),
    });

    const auto visgraph = VisgraphGenerator::generate(std::vector<Polygon>{polygon});

    auto expectedVisGraph = Graph({polygon});
    add_edges(Coordinate(1.5, -2),
              {
                  VisibleVertex{.coord = Coordinate(-1, -1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(0, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(1, -1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1.5, -2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(1.5, 1), .is_visible_across_meridian = false},
              },
              expectedVisGraph);
    add_edges(Coordinate(-1.5, -2),
              {
                  VisibleVertex{.coord = Coordinate(-1.5, 1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1, -1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(0, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(1.5, -2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(1, -1), .is_visible_across_meridian = false},
              },
              expectedVisGraph);
    add_edges(Coordinate(0, 0),
              {
                  VisibleVertex{.coord = Coordinate(1.5, -2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(1, -1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1, -1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1.5, -2), .is_visible_across_meridian = false},
              },
              expectedVisGraph);
    add_edges(Coordinate(-1, -1),
              {
                  VisibleVertex{.coord = Coordinate(1.5, -2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1.5, -2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(1, -1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(0, 0), .is_visible_across_meridian = false},
              },
              expectedVisGraph);
    add_edges(Coordinate(1, -1),
              {
                  VisibleVertex{.coord = Coordinate(1.5, -2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(0, 0), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1, -1), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1.5, -2), .is_visible_across_meridian = false},
              },
              expectedVisGraph);
    add_edges(Coordinate(-1.5, 1),
              {
                  VisibleVertex{.coord = Coordinate(-1.5, -2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(1.5, 1), .is_visible_across_meridian = false},
              },
              expectedVisGraph);
    add_edges(Coordinate(1.5, 1),
              {
                  VisibleVertex{.coord = Coordinate(1.5, -2), .is_visible_across_meridian = false},
                  VisibleVertex{.coord = Coordinate(-1.5, 1), .is_visible_across_meridian = false},
              },
              expectedVisGraph);
}

void add_edges(const Coordinate &source, const std::vector<VisibleVertex> &neighbors, Graph &g) {
    for (const auto &neighbor : neighbors) {
        g.add_edge(source, neighbor.coord, neighbor.is_visible_across_meridian);
    }
}

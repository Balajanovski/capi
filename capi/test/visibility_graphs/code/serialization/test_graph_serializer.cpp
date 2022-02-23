//
// Created by James.Balajan on 12/05/2021.
//

#include <catch.hpp>

#include "datastructures/graph/graph.hpp"
#include "serialization/graph_serializer.hpp"
#include "visgraph/visgraph_generator.hpp"

TEST_CASE("Graph serialize") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(2., 1.);
    const auto coord3 = Coordinate(1., 1.);
    const auto coord4 = Coordinate(10., 2.);

    auto graph = std::make_shared<Graph>(std::vector<Polygon>{Polygon({coord1, coord2}), Polygon({coord3, coord4})});

    graph->add_edge(coord1, coord2, false);
    graph->add_edge(coord1, coord3, false);
    graph->add_edge(coord3, coord4, true);

    char tmp_name[L_tmpnam];
    tmpnam(tmp_name);

    GraphSerializer::serialize_to_file(graph, tmp_name);
    const auto deserialized_graph = GraphSerializer::deserialize_from_file(tmp_name);

    remove(tmp_name);

    REQUIRE(*graph == *deserialized_graph);
}

TEST_CASE("Graph serialize 2") {
    const auto poly1 = Polygon({
        Coordinate(1., 0.),
        Coordinate(0., 1.),
        Coordinate(-1., 0.),
    });

    const auto poly2 = Polygon({
        Coordinate(4., 0.),
        Coordinate(3., 1.),
        Coordinate(2., 0.),
    });

    const auto graph = VisgraphGenerator::generate({poly1, poly2});

    char tmp_name[L_tmpnam];
    tmpnam(tmp_name);

    GraphSerializer::serialize_to_file(graph, tmp_name);
    const auto deserialized_graph = GraphSerializer::deserialize_from_file(tmp_name);

    remove(tmp_name);

    REQUIRE(*graph == *deserialized_graph);
}

TEST_CASE("Graph serialize 3") {
    const auto coord1 = Coordinate(1., 2.);
    const auto coord2 = Coordinate(2., 1.);
    const auto coord3 = Coordinate(2., -1.);
    const auto coord4 = Coordinate(1., -2.);
    const auto coord5 = Coordinate(-1., -2.);
    const auto coord6 = Coordinate(-2., -1.);
    const auto coord7 = Coordinate(-2., 1.);
    const auto coord8 = Coordinate(-1., 2.);

    auto graph = std::make_shared<Graph>(std::vector<Polygon>{Polygon({
        coord1,
        coord2,
        coord3,
        coord4,
        coord5,
        coord6,
        coord7,
        coord8,
    })});

    graph->add_edge(coord1, coord2, false);
    graph->add_edge(coord1, coord3, false);
    graph->add_edge(coord3, coord4, true);

    char tmp_name[L_tmpnam];
    tmpnam(tmp_name);

    GraphSerializer::serialize_to_file(graph, tmp_name);
    const auto deserialized_graph = GraphSerializer::deserialize_from_file(tmp_name);

    remove(tmp_name);

    REQUIRE(*graph == *deserialized_graph);
}

//
// Created by James.Balajan on 8/06/2021.
//

#include <iostream>
#include <cmath>

#include "types/coordinate/coordinate.hpp"
#include "serialization/graph_serializer.hpp"
#include "shortest_path/shortest_path_computer.hpp"

int main() {
    const auto graph = GraphSerializer::deserialize_from_file("/mnt/d/DATVM/vessel_eta_prediction/resources/shapefiles/graph/default");
    const auto computer = ShortestPathComputer(graph);

    const auto source = Coordinate(104., 1.);
    const auto dest = Coordinate(13., 55.);
    const auto path = computer.shortest_path(source, dest, INFINITY, false, 1.0);

    for (const auto &coord : path) {
        std::cout << coord << "\n";
    }
    std::cout << std::flush;
}

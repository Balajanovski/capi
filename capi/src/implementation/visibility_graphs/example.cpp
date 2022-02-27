//
// Created by James.Balajan on 8/06/2021.
//

#include <iostream>
#include <cmath>
#include <ctime>

#include "types/coordinate/coordinate.hpp"
#include "serialization/graph_serializer.hpp"
#include "shortest_path/shortest_path_computer.hpp"

int main() {
    const auto graph = GraphSerializer::deserialize_from_file("/mnt/d/DATVM/vessel_eta_prediction/resources/shapefiles/graph/default");
    const auto computer = ShortestPathComputer(graph);

    const auto start = std::time(nullptr);
    const auto paths = computer.shortest_paths(
            std::vector<std::pair<Coordinate, Coordinate>> {
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                    std::make_pair(Coordinate(104, 1), Coordinate(13, 55)),
                },
            INFINITY,
            false,
            1.0);
    std::cout << "Took " << std::time(nullptr) - start << " seconds" << std::endl;
}
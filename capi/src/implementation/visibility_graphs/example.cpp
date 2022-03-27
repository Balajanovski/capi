//
// Created by James.Balajan on 8/06/2021.
//

#include <iostream>
#include <ctime>

#include "types/coordinate/coordinate.hpp"
#include "serialization/graph_serializer.hpp"
#include "shortest_path/shortest_path_computer.hpp"

int main() {
    const auto graph = GraphSerializer::deserialize_from_file("/mnt/d/DATVM/vessel_eta_prediction/resources/shapefiles/graph/default");
    const auto computer = ShortestPathComputer(graph);
    std::cout << "Loaded graph" << std::endl;

    const auto num_trials = 1;
    float total_time = 0.0;
    float conjoined_time = 0.0;

    for (int i = 0; i < num_trials; ++i) {
        const auto source = Coordinate(-129.127, -40.776082);
        const auto source2 = Coordinate(36.7637, 46.046);
        const auto dest = Coordinate(36.7537, 46.04);

        auto source_dest_pairs1 = std::vector<std::pair<Coordinate, Coordinate>>();
        auto source_dest_pairs2 = std::vector<std::pair<Coordinate, Coordinate>>();
        for (int i = 0; i < 50; ++i) {
            source_dest_pairs1.push_back(std::make_pair(source, dest));
        }
        for (int i = 0; i < 100; ++i) {
            source_dest_pairs2.push_back(std::make_pair(source2, dest));
        }
        auto source_dest_pairs = std::vector<std::pair<Coordinate, Coordinate>>();
        source_dest_pairs.reserve(source_dest_pairs1.size() + source_dest_pairs2.size());
        source_dest_pairs.insert(source_dest_pairs.end(), source_dest_pairs1.begin(), source_dest_pairs1.end());
        source_dest_pairs.insert(source_dest_pairs.end(), source_dest_pairs2.begin(), source_dest_pairs2.end());

        auto start = std::time(nullptr);
        const auto path1 = computer.shortest_paths(source_dest_pairs1);
        const auto path2 = computer.shortest_paths(source_dest_pairs2);
        total_time += (std::time(nullptr) - start);

        start = std::time(nullptr);
        const auto path = computer.shortest_paths(source_dest_pairs);
        conjoined_time += (std::time(nullptr) - start);
    }
    std::cout << "Separate Took " << total_time / num_trials << " seconds" << std::endl;
    std::cout << "Together Took " << conjoined_time / num_trials << " seconds" << std::endl;
}
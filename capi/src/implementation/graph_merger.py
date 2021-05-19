import os
from typing import Sequence, cast

from capi.src.implementation.datastructures.graph_file_paths import GraphFilePaths
from capi.src.implementation.visibility_graphs import load_graph_from_file, merge_graphs, save_graph_to_file
from capi.src.interfaces.graph_merger import IGraphMerger


class GraphMerger(IGraphMerger):
    def merge(self, input_graph_files: Sequence[str], output_graph_file: str) -> None:
        os.mkdir(output_graph_file)

        input_graph_file_paths = [GraphFilePaths(graph_file) for graph_file in input_graph_files]
        default_graphs = list(
            map(
                lambda graph_file_path: cast(object, load_graph_from_file(graph_file_path.default_graph_path)),
                input_graph_file_paths,
            )
        )

        merged_default_graph = merge_graphs(default_graphs)

        output_graph_file_paths = GraphFilePaths(output_graph_file)
        save_graph_to_file(merged_default_graph, output_graph_file_paths.default_graph_path)

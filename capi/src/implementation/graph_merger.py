from capi.src.interfaces.graph_merger import IGraphMerger
from capi.src.implementation.visibility_graphs import merge_graphs, load_graph_from_file, save_graph_to_file
from typing import Sequence
from multiprocessing.pool import ThreadPool


class GraphMerger(IGraphMerger):
    def __init__(self, num_load_threads: int = 10):
        self._num_load_threads = num_load_threads

    def merge(self, input_graph_files: Sequence[str], output_graph_file: str) -> None:
        with ThreadPool(self._num_load_threads) as pool:
            graphs = list(
                pool.map(load_graph_from_file, input_graph_files)
            )

        merged_graph = merge_graphs(graphs)
        save_graph_to_file(merged_graph, output_graph_file)

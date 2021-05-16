from capi.src.interfaces.graph_merger import IGraphMerger
from typing import Sequence


class GraphMerger(IGraphMerger):
    def merge(self, input_graph_files: Sequence[str], output_graph_file: str) -> None:
        pass

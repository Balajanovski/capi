import abc
from typing import Sequence


class IGraphMerger(abc.ABC):
    @abc.abstractmethod
    def merge(self, input_graph_files: Sequence[str], output_graph_file: str) -> None:
        pass

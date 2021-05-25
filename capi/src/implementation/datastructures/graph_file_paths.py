import os

from capi.src.interfaces.datastructures.graph_file_paths import IGraphFilePaths


class GraphFilePaths(IGraphFilePaths):
    def __init__(self, folder_path: str):
        self._folder_path = folder_path

    @property
    def default_graph_path(self) -> str:
        return os.path.join(self._folder_path, "default")

    @property
    def folder_path(self) -> str:
        return self._folder_path

import abc


class IGraphFilePaths(abc.ABC):
    @property
    @abc.abstractmethod
    def default_graph_path(self) -> str:
        pass

    @property
    @abc.abstractmethod
    def folder_path(self) -> str:
        pass

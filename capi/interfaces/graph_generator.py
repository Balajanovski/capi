import abc


class IGraphGenerator(abc.ABC):
    @abc.abstractmethod
    def generate(self, shape_file_path: str, output_path: str, meridian_crossing: bool = False) -> None:
        pass

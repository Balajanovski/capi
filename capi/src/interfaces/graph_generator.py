import abc


class IGraphGenerator(abc.ABC):
    @abc.abstractmethod
    def generate(self, shape_file_path: str, output_path: str) -> None:
        pass

    @abc.abstractmethod
    def generate_for_vertex_range(
        self, shape_file_path: str, output_path: str, current_split_num: int, num_splits: int, seed: int
    ) -> None:
        pass

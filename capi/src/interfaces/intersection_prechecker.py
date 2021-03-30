import abc

from capi.src.implementation.dtos.coordinate import Coordinate


class IIntersectionPrechecker(abc.ABC):
    @abc.abstractmethod
    def does_line_cross_polygons(self, coord_1: Coordinate, coord_2: Coordinate) -> bool:
        pass

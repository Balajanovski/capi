import abc

from capi.src.dtos.coordinate import Coordinate


class IIntersectionPrechecker(abc.ABC):
    def does_line_cross_polygons(self, coord_1: Coordinate, coord_2: Coordinate) -> bool:
        pass

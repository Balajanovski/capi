from capi.interfaces.intersection_prechecker import IIntersectionPrechecker
from capi.src.dtos.coordinate import Coordinate


class IntersectionPrechecker(IIntersectionPrechecker):
    def __init__(self, shapefile_file_path: str):
        self._shapefile_file_path = shapefile_file_path

    def does_line_cross_polygons(self, coord_1: Coordinate, coord_2: Coordinate) -> bool:
        pass

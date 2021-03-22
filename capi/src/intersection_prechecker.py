import typing

from capi.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.interfaces.intersection_prechecker import IIntersectionPrechecker
from capi.interfaces.shapefiles.shapefile_reader import IShapefileReader
from capi.src.datastructures.axis_aligned_box import AxisAlignedBox
from capi.src.dtos.coordinate import Coordinate, construct_coord_in_lat_lon_range


class IntersectionPrechecker(IIntersectionPrechecker):
    def __init__(self, shapefile_file_path: str, shapefile_reader: IShapefileReader, clip_box_margin: float = 0.01802):
        self._polygons = shapefile_reader.read(shapefile_file_path)
        self._clip_box_margin = clip_box_margin

    def does_line_cross_polygons(self, coord_1: Coordinate, coord_2: Coordinate) -> bool:
        clip_box = self._create_clip_box(coord_1, coord_2)

    def _create_clip_box(self, coord_1: Coordinate, coord_2: Coordinate) -> IAxisAlignedBox:
        min_lon = min(coord_1.longitude, coord_2.longitude) - self._clip_box_margin
        max_lon = max(coord_1.longitude, coord_2.longitude) + self._clip_box_margin
        min_lat = min(coord_1.latitude, coord_2.latitude) - self._clip_box_margin
        max_lat = max(coord_1.latitude, coord_2.latitude) + self._clip_box_margin

        top_left = construct_coord_in_lat_lon_range(
            longitude=min_lon,
            latitude=max_lat,
        )
        bottom_right = construct_coord_in_lat_lon_range(
            longitude=max_lon,
            latitude=min_lat,
        )

        return AxisAlignedBox(
            top_left=top_left,
            bottom_right=bottom_right,
        )

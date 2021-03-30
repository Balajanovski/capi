import typing

from capi.src.implementation.datastructures.axis_aligned_box import AxisAlignedBox
from capi.src.implementation.datastructures.polygon_spatial_index_factory import PolygonSpatialIndexFactory
from capi.src.implementation.dtos.coordinate import Coordinate, construct_coord_in_lat_lon_range
from capi.src.implementation.shapefiles.shapefile_reader import ShapefileReader
from capi.src.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.src.interfaces.datastructures.polygon_spatial_index_factory import IPolygonSpatialIndexFactory
from capi.src.interfaces.intersection_prechecker import IIntersectionPrechecker
from capi.src.interfaces.shapefiles.shapefile_reader import IShapefileReader


class IntersectionPrechecker(IIntersectionPrechecker):
    def __init__(
        self,
        shapefile_file_path: str,
        shapefile_reader: typing.Optional[IShapefileReader] = None,
        spatial_index_factory: typing.Optional[IPolygonSpatialIndexFactory] = None,
    ):
        _shapefile_reader = ShapefileReader() if shapefile_reader is None else shapefile_reader

        _polygons = _shapefile_reader.read(shapefile_file_path)
        _spatial_index_factory = (
            PolygonSpatialIndexFactory() if spatial_index_factory is None else spatial_index_factory
        )
        self._spatial_index = _spatial_index_factory.create(_polygons)

    def does_line_cross_polygons(self, coord_1: Coordinate, coord_2: Coordinate) -> bool:
        index_clip_box = self._create_clip_box(coord_1, coord_2, lat_lon_wrap=False, margin=0)

        candidate_polygons = self._spatial_index.intersecting_polygons_with_box(index_clip_box)

        for polygon in candidate_polygons:
            if polygon.line_intersects(coord_1, coord_2):
                return True

        return False

    @staticmethod
    def _create_clip_box(
        coord_1: Coordinate, coord_2: Coordinate, lat_lon_wrap: bool, margin: float
    ) -> IAxisAlignedBox:
        min_lon = min(coord_1.longitude, coord_2.longitude) - margin
        max_lon = max(coord_1.longitude, coord_2.longitude) + margin
        min_lat = min(coord_1.latitude, coord_2.latitude) - margin
        max_lat = max(coord_1.latitude, coord_2.latitude) + margin

        if not lat_lon_wrap:
            top_left = Coordinate(
                longitude=min_lon,
                latitude=max_lat,
            )
            bottom_right = Coordinate(
                longitude=max_lon,
                latitude=min_lat,
            )
        else:
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

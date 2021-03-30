from capi.src.implementation.datastructures.axis_aligned_box import AxisAlignedBox
from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.src.interfaces.datastructures.polygon import IPolygon
from capi.src.interfaces.geometry.polygon_bounding_box_computer import IPolygonBoundingBoxComputer


class PolygonBoundingBoxComputer(IPolygonBoundingBoxComputer):
    def compute(self, polygon: IPolygon) -> IAxisAlignedBox:
        latitudes = [vertex.latitude for vertex in polygon.vertices]
        longitudes = [vertex.longitude for vertex in polygon.vertices]

        min_lat, max_lat = min(latitudes), max(latitudes)
        min_lon, max_lon = min(longitudes), max(longitudes)

        return AxisAlignedBox(
            top_left=Coordinate(longitude=min_lon, latitude=max_lat),
            bottom_right=Coordinate(longitude=max_lon, latitude=min_lat),
        )

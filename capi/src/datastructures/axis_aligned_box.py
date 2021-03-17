import typing

from capi.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.interfaces.datastructures.polygon import IPolygon
from capi.interfaces.meridian_handling.meridian_wrap_coord_transformer import IMeridianWrapCoordTransformer
from capi.src.dtos.coordinate import Coordinate
from capi.src.meridian_handling.meridian_wrap_coord_transformer import MeridianWrapCoordTransformer


class AxisAlignedBox(IAxisAlignedBox):
    def __init__(
        self,
        top_left: Coordinate,
        bottom_right: Coordinate,
        meridian_wrap_transformer: typing.Optional[IMeridianWrapCoordTransformer] = None,
    ):
        max_lat = top_left.latitude
        min_lon = top_left.longitude

        min_lat = bottom_right.latitude
        max_lon = bottom_right.longitude

        self._upper_right = Coordinate(latitude=max_lat, longitude=max_lon)
        self._lower_right = bottom_right.copy()
        self._upper_left = top_left.copy()
        self._lower_left = Coordinate(latitude=min_lat, longitude=min_lon)

        self._meridian_wrap_transformer = (
            MeridianWrapCoordTransformer() if meridian_wrap_transformer is None else meridian_wrap_transformer
        )

    @property
    def upper_right(self) -> Coordinate:
        return self._upper_right

    @property
    def upper_left(self) -> Coordinate:
        return self._upper_left

    @property
    def lower_right(self) -> Coordinate:
        return self._lower_right

    @property
    def lower_left(self) -> Coordinate:
        return self._lower_left

    @property
    def vertices(self) -> typing.Sequence[Coordinate]:
        return [self._upper_left, self._upper_right, self._lower_right, self._lower_left]

    def is_point_inside(self, point: Coordinate) -> bool:
        transformed_coords = self._meridian_wrap_transformer.wrap_coordinates(
            [self._upper_left, self._lower_right] + [point]
        )
        transformed_vertices = transformed_coords[:-1]
        transformed_point = transformed_coords[-1]

        max_lat = transformed_vertices[0].latitude
        min_lon = transformed_vertices[0].longitude

        min_lat = transformed_vertices[1].latitude
        max_lon = transformed_vertices[1].longitude

        return (min_lon < transformed_point.longitude < max_lon) and (min_lat < transformed_point.latitude < max_lat)

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, IPolygon):
            return False

        return self.vertices == other.vertices

    def __repr__(self) -> str:
        return f"AxisAlignedBox (vertices: {self.vertices})"

import typing

import shapely.geometry as geom  # type: ignore

from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.implementation.meridian_handling.meridian_wrap_coord_transformer import MeridianWrapCoordTransformer
from capi.src.interfaces.datastructures.polygon import IPolygon
from capi.src.interfaces.meridian_handling.meridian_wrap_coord_transformer import IMeridianWrapCoordTransformer


class Polygon(IPolygon):
    def __init__(
        self,
        vertices: typing.Sequence[Coordinate],
        meridian_wrap_transformer: typing.Optional[IMeridianWrapCoordTransformer] = None,
    ):
        if len(vertices) < 3:
            raise ValueError(f"{Polygon.__name__} was initialised with less than 3 vertices. Vertex dump: {vertices}")

        self._vertices = list(vertices)
        self._meridian_wrap_transformer = (
            MeridianWrapCoordTransformer() if meridian_wrap_transformer is None else meridian_wrap_transformer
        )

    @classmethod
    def from_shapely_polygon(
        cls,
        shapely_polygon: geom.Polygon,
        meridian_wrap_transformer: typing.Optional[IMeridianWrapCoordTransformer] = None,
    ) -> IPolygon:
        return cls(
            [
                Coordinate(latitude=latitude, longitude=longitude)
                for longitude, latitude in zip(shapely_polygon.exterior.xy[0][:-1], shapely_polygon.exterior.xy[1][:-1])
            ],
            meridian_wrap_transformer=meridian_wrap_transformer,
        )

    @property
    def vertices(self) -> typing.Sequence[Coordinate]:
        return self._vertices

    def is_point_inside(self, point: Coordinate) -> bool:
        if len(self._vertices) <= 2:
            return False

        meridian_wrapped_coords = list(self._meridian_wrap_transformer.wrap_coordinates(self._vertices + [point]))
        meridian_wrapped_poly_coords = meridian_wrapped_coords[:-1]
        meridian_wrapped_query_coord = meridian_wrapped_coords[-1]
        shapely_polygon = self._make_shapely_polygon(meridian_wrapped_poly_coords)
        shapely_point = geom.Point(meridian_wrapped_query_coord.longitude, meridian_wrapped_query_coord.latitude)

        return typing.cast(bool, shapely_point.within(shapely_polygon))

    def line_intersects(self, point_1: Coordinate, point_2: Coordinate) -> bool:
        meridian_wrapped_coords = list(
            self._meridian_wrap_transformer.wrap_coordinates(self._vertices + [point_1, point_2])
        )
        meridian_wrapped_poly_coords = meridian_wrapped_coords[:-2]
        meridian_wrapped_point_1 = meridian_wrapped_coords[-2]
        meridian_wrapped_point_2 = meridian_wrapped_coords[-1]

        shapely_poly = self._make_shapely_polygon(meridian_wrapped_poly_coords)
        return typing.cast(
            bool,
            shapely_poly.intersects(
                geom.LineString(
                    coordinates=[
                        (meridian_wrapped_point_1.longitude, meridian_wrapped_point_1.latitude),
                        (meridian_wrapped_point_2.longitude, meridian_wrapped_point_2.latitude),
                    ]
                )
            ),
        )

    def to_shapely_polygon(self) -> geom.Polygon:
        return self._make_shapely_polygon(self._vertices)

    @staticmethod
    def _make_shapely_polygon(vertices: typing.Iterable[Coordinate]) -> geom.Polygon:
        return geom.Polygon([(vertex.longitude, vertex.latitude) for vertex in vertices])

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, IPolygon):
            return False

        return self.vertices == other.vertices

    def __repr__(self) -> str:
        return f"Polygon (vertices: {self.vertices})"

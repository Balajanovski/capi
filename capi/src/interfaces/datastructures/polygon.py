import abc
import typing

import shapely.geometry as geom  # type: ignore

from capi.src.implementation.dtos.coordinate import Coordinate


class IPolygon(abc.ABC):
    @property
    @abc.abstractmethod
    def vertices(self) -> typing.Sequence[Coordinate]:
        pass

    @abc.abstractmethod
    def is_point_inside(self, point: Coordinate) -> bool:
        pass

    @abc.abstractmethod
    def line_intersects(self, point_1: Coordinate, point_2: Coordinate) -> bool:
        pass

    @abc.abstractmethod
    def __eq__(self, other: object) -> bool:
        pass

    @abc.abstractmethod
    def __repr__(self) -> str:
        pass

    @abc.abstractmethod
    def to_shapely_polygon(self) -> geom.Polygon:
        pass

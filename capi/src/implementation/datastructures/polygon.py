import typing

from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.interfaces.datastructures.polygon import IPolygon


class Polygon(IPolygon):
    def __init__(
        self,
        vertices: typing.Sequence[Coordinate],
    ):
        if len(vertices) < 3:
            raise ValueError(f"{Polygon.__name__} was initialised with less than 3 vertices. Vertex dump: {vertices}")

        self._vertices = list(vertices)

    @property
    def vertices(self) -> typing.Sequence[Coordinate]:
        return self._vertices

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, IPolygon):
            return False

        return self.vertices == other.vertices

    def __repr__(self) -> str:
        return f"Polygon (vertices: {self.vertices})"

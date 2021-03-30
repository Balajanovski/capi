import abc
import typing

from capi.src.interfaces.datastructures.polygon import IPolygon
from capi.src.interfaces.datastructures.polygon_spatial_index import IPolygonSpatialIndex


class IPolygonSpatialIndexFactory(abc.ABC):
    @abc.abstractmethod
    def create(self, polygons: typing.Sequence[IPolygon]) -> IPolygonSpatialIndex:
        pass

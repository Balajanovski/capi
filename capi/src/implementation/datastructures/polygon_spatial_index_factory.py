import typing

from capi.src.implementation.datastructures.polygon_spatial_index import PolygonSpatialIndex
from capi.src.interfaces.datastructures.polygon import IPolygon
from capi.src.interfaces.datastructures.polygon_spatial_index import IPolygonSpatialIndex
from capi.src.interfaces.datastructures.polygon_spatial_index_factory import IPolygonSpatialIndexFactory


class PolygonSpatialIndexFactory(IPolygonSpatialIndexFactory):
    def create(self, polygons: typing.Sequence[IPolygon]) -> IPolygonSpatialIndex:
        return PolygonSpatialIndex(polygons=polygons)

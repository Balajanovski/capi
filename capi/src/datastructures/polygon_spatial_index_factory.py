import typing

from capi.interfaces.datastructures.polygon import IPolygon
from capi.interfaces.datastructures.polygon_spatial_index import IPolygonSpatialIndex
from capi.interfaces.datastructures.polygon_spatial_index_factory import IPolygonSpatialIndexFactory
from capi.src.datastructures.polygon_spatial_index import PolygonSpatialIndex


class PolygonSpatialIndexFactory(IPolygonSpatialIndexFactory):
    def create(self, polygons: typing.Sequence[IPolygon]) -> IPolygonSpatialIndex:
        return PolygonSpatialIndex(polygons=polygons)

import typing

from shapely.strtree import STRtree  # type: ignore

from capi.src.implementation.datastructures.polygon import Polygon
from capi.src.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.src.interfaces.datastructures.polygon import IPolygon
from capi.src.interfaces.datastructures.polygon_spatial_index import IPolygonSpatialIndex


class PolygonSpatialIndex(IPolygonSpatialIndex):
    def __init__(
        self,
        polygons: typing.Sequence[IPolygon],
    ):
        self._index = STRtree([polygon.to_shapely_polygon() for polygon in polygons])

    def intersecting_polygons_with_box(self, box: IAxisAlignedBox) -> typing.Sequence[IPolygon]:
        return [
            Polygon.from_shapely_polygon(shapely_poly) for shapely_poly in self._index.query(box.to_shapely_polygon())
        ]

import typing

from rtree import index  # type: ignore

from capi.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.interfaces.datastructures.polygon import IPolygon
from capi.interfaces.datastructures.polygon_spatial_index import IPolygonSpatialIndex
from capi.interfaces.geometry.polygon_bounding_box_computer import IPolygonBoundingBoxComputer
from capi.src.geometry.polygon_bounding_box_computer import PolygonBoundingBoxComputer


class PolygonSpatialIndex(IPolygonSpatialIndex):
    def __init__(
        self,
        polygons: typing.Sequence[IPolygon],
        bounding_box_computer: typing.Optional[IPolygonBoundingBoxComputer] = None,
    ):
        self._bounding_box_computer = (
            PolygonBoundingBoxComputer() if bounding_box_computer is None else bounding_box_computer
        )

        self._index = index.Index()
        for i, polygon in enumerate(polygons):
            bounding_box = self._bounding_box_computer.compute(polygon)
            bottom_left = bounding_box.lower_left
            top_right = bounding_box.upper_right

            self._index.insert(
                i, (bottom_left.longitude, bottom_left.latitude, top_right.longitude, top_right.latitude)
            )

        self._polygons = polygons

    def intersecting_polygons_with_box(self, box: IAxisAlignedBox) -> typing.Sequence[IPolygon]:
        bottom_left = box.lower_left
        top_right = box.upper_right

        return [
            self._polygons[poly_index]
            for poly_index in self._index.intersection(
                (bottom_left.longitude, bottom_left.latitude, top_right.longitude, top_right.latitude)
            )
        ]

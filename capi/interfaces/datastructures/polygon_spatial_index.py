import abc
import typing

from capi.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.interfaces.datastructures.polygon import IPolygon


class IPolygonSpatialIndex(abc.ABC):
    @abc.abstractmethod
    def intersecting_polygons_with_box(self, box: IAxisAlignedBox) -> typing.Sequence[IPolygon]:
        pass

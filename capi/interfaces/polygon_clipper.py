import abc
import typing

from capi.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.interfaces.datastructures.polygon import IPolygon


class IPolygonClipper(abc.ABC):
    @abc.abstractmethod
    def clip_single_polygon(self, polygon: IPolygon, clipping_box: IAxisAlignedBox) -> IPolygon:
        pass

    @abc.abstractmethod
    def clip_polygons(
        self, polygons: typing.Sequence[IPolygon], clipping_box: IAxisAlignedBox
    ) -> typing.Sequence[IPolygon]:
        pass

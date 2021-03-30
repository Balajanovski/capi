import abc
import typing

from capi.src.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.src.interfaces.datastructures.polygon import IPolygon


class IPolygonClipper(abc.ABC):
    @abc.abstractmethod
    def clip_single_polygon(self, polygon: IPolygon, clipping_box: IAxisAlignedBox) -> typing.Optional[IPolygon]:
        pass

    @abc.abstractmethod
    def clip_polygons(
        self, polygons: typing.Sequence[IPolygon], clipping_box: IAxisAlignedBox
    ) -> typing.Sequence[IPolygon]:
        pass

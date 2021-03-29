import abc

from capi.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.interfaces.datastructures.polygon import IPolygon


class IPolygonBoundingBoxComputer(abc.ABC):
    @abc.abstractmethod
    def compute(self, polygon: IPolygon) -> IAxisAlignedBox:
        pass

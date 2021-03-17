import abc

from capi.interfaces.datastructures.polygon import IPolygon
from capi.src.dtos.coordinate import Coordinate


class IAxisAlignedBox(IPolygon, abc.ABC):
    def upper_right(self) -> Coordinate:
        pass

    def upper_left(self) -> Coordinate:
        pass

    def lower_right(self) -> Coordinate:
        pass

    def lower_left(self) -> Coordinate:
        pass

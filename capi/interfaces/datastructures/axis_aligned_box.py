import abc

from capi.interfaces.datastructures.polygon import IPolygon
from capi.src.dtos.coordinate import Coordinate


class IAxisAlignedBox(IPolygon, abc.ABC):
    @property
    @abc.abstractmethod
    def upper_right(self) -> Coordinate:
        pass

    @property
    @abc.abstractmethod
    def upper_left(self) -> Coordinate:
        pass

    @property
    @abc.abstractmethod
    def lower_right(self) -> Coordinate:
        pass

    @property
    @abc.abstractmethod
    def lower_left(self) -> Coordinate:
        pass

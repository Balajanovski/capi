import abc

from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.interfaces.datastructures.polygon import IPolygon


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

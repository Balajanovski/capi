import abc
import typing

from capi.src.implementation.dtos.coordinate import Coordinate


class IPathInterpolator(abc.ABC):
    @abc.abstractmethod
    def interpolate(self, coord_1: Coordinate, coord_2: Coordinate) -> typing.Sequence[Coordinate]:
        pass

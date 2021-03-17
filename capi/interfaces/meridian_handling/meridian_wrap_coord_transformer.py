import abc
import typing

from capi.src.dtos.coordinate import Coordinate


class IMeridianWrapCoordTransformer(abc.ABC):
    @abc.abstractmethod
    def wrap_coordinates(self, coordinates: typing.Sequence[Coordinate]) -> typing.Sequence[Coordinate]:
        pass

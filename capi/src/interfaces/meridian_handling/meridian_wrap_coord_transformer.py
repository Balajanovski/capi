import abc
import typing

from capi.src.implementation.dtos.coordinate import Coordinate


class IMeridianWrapCoordTransformer(abc.ABC):
    @abc.abstractmethod
    def wrap_coordinates(self, coordinates: typing.Sequence[Coordinate]) -> typing.Sequence[Coordinate]:
        pass

    @abc.abstractmethod
    def reverse_wrap(self, coordinates: typing.Sequence[Coordinate]) -> typing.Sequence[Coordinate]:
        pass

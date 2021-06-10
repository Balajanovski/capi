import abc
import typing

from capi.src.implementation.dtos.coordinate import Coordinate


class IPolygon(abc.ABC):
    @property
    @abc.abstractmethod
    def vertices(self) -> typing.Sequence[Coordinate]:
        pass

    @abc.abstractmethod
    def __eq__(self, other: object) -> bool:
        pass

    @abc.abstractmethod
    def __repr__(self) -> str:
        pass

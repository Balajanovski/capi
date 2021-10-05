import abc
import math
import typing

from capi.src.implementation.dtos.coordinate import Coordinate


class IPathInterpolator(abc.ABC):
    @abc.abstractmethod
    def interpolate(
        self, coord_1: Coordinate, coord_2: Coordinate, search_distance_from_source_limit: float = math.inf
    ) -> typing.Sequence[Coordinate]:
        pass

    @abc.abstractmethod
    def batch_interpolate(
        self,
        coord_pairs: typing.Sequence[typing.Tuple[Coordinate, Coordinate]],
        search_distance_from_source_limit: float = math.inf,
    ) -> typing.Sequence[typing.Optional[typing.Sequence[Coordinate]]]:
        pass

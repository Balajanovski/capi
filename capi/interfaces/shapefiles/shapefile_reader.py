import abc
import typing

from capi.interfaces.datastructures.polygon import IPolygon


class IShapefileReader(abc.ABC):
    @abc.abstractmethod
    def read(self, shape_file_path: str) -> typing.Sequence[IPolygon]:
        pass

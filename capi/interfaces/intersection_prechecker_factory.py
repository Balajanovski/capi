import abc

from capi.interfaces.intersection_prechecker import IIntersectionPrechecker


class IIntersectionPrecheckerFactory(abc.ABC):
    @abc.abstractmethod
    def create(self, shapefile_path: str) -> IIntersectionPrechecker:
        pass

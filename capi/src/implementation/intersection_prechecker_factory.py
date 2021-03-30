from capi.src.implementation.intersection_prechecker import IntersectionPrechecker
from capi.src.interfaces.intersection_prechecker import IIntersectionPrechecker
from capi.src.interfaces.intersection_prechecker_factory import IIntersectionPrecheckerFactory


class IntersectionPrecheckerFactory(IIntersectionPrecheckerFactory):
    def create(self, shapefile_path: str) -> IIntersectionPrechecker:
        return IntersectionPrechecker(shapefile_path)

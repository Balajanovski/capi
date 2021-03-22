from capi.interfaces.intersection_prechecker import IIntersectionPrechecker
from capi.interfaces.intersection_prechecker_factory import IIntersectionPrecheckerFactory
from capi.src.intersection_prechecker import IntersectionPrechecker


class IntersectionPrecheckerFactory(IIntersectionPrecheckerFactory):
    def create(self, shapefile_path: str) -> IIntersectionPrechecker:
        return IntersectionPrechecker(shapefile_path)

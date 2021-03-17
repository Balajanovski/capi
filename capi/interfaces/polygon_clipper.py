import abc


class IPolygonClipper(abc.ABC):
    @abc.abstractmethod
    def clip(self):
        pass

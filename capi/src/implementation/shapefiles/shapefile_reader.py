import typing

import shapefile  # type: ignore

from capi.src.implementation.datastructures.polygon import Polygon
from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.interfaces.datastructures.polygon import IPolygon
from capi.src.interfaces.shapefiles.shapefile_reader import IShapefileReader


class ShapefileReader(IShapefileReader):
    def read(self, shape_file_path: str) -> typing.Sequence[IPolygon]:
        shape_file = shapefile.Reader(shape_file_path)

        return [
            Polygon([Coordinate(longitude=point[0], latitude=point[1]) for point in shape.points[:-1]])
            for shape in shape_file.shapes()
        ]

import typing

import shapefile  # type: ignore

from capi.interfaces.datastructures.polygon import IPolygon
from capi.interfaces.shapefiles.shapefile_reader import IShapefileReader
from capi.src.datastructures.polygon import Polygon
from capi.src.dtos.coordinate import Coordinate


class ShapefileReader(IShapefileReader):
    def read(self, shape_file_path: str) -> typing.Sequence[IPolygon]:
        shape_file = shapefile.Reader(shape_file_path)

        return [
            Polygon([Coordinate(longitude=point[0], latitude=point[1]) for point in shape.points[:-1]])
            for shape in shape_file.shapes()
        ]

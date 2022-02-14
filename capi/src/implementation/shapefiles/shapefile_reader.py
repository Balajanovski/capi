import typing

import shapefile  # type: ignore

from capi.src.implementation.datastructures.polygon import Polygon
from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.interfaces.datastructures.polygon import IPolygon
from capi.src.interfaces.shapefiles.shapefile_reader import IShapefileReader


class ShapefileReader(IShapefileReader):
    def read(self, shape_file_path: str) -> typing.Sequence[IPolygon]:
        with shapefile.Reader(str(shape_file_path)) as reader:
            polygons = []
            for shape in reader.shapes():
                if shape.shapeTypeName != "POLYGON":
                    continue

                current_part = []

                part_i = 1
                for point_i, point in enumerate(shape.points):
                    if len(shape.parts) > part_i and shape.parts[part_i] == point_i:
                        break
                    current_part.append(Coordinate(longitude=point[0], latitude=point[1]))

                # We only create a new polygon if the part has 3 or more vertices
                # (> because shapefiles repeat the first vertex)
                if len(current_part) > 3:
                    polygons.append(
                        Polygon(vertices=current_part[:-1])
                    )

            return polygons

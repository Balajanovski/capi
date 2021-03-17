import typing

from capi.interfaces.meridian_handling.meridian_wrap_coord_transformer import IMeridianWrapCoordTransformer
from capi.src.dtos.coordinate import Coordinate


class MeridianWrapCoordTransformer(IMeridianWrapCoordTransformer):
    def wrap_coordinates(self, coordinates: typing.Sequence[Coordinate]) -> typing.Sequence[Coordinate]:
        longitudes = [coord.longitude for coord in coordinates]
        max_lon = max(longitudes)
        min_lon = min(longitudes)

        if (max_lon - min_lon) < 180:
            return coordinates

        wrapped_coordinates = [coord.copy() for coord in coordinates]
        positive_lon_vertex_indices = []
        negative_lon_vertex_indices = []

        for i, lon in enumerate(longitudes):
            if lon < 0:
                negative_lon_vertex_indices.append(i)
            else:
                positive_lon_vertex_indices.append(i)

        if len(positive_lon_vertex_indices) > 0 and len(negative_lon_vertex_indices) > 0:
            for negative_lon_index in negative_lon_vertex_indices:
                wrapped_coord = Coordinate(
                    latitude=wrapped_coordinates[negative_lon_index].latitude,
                    longitude=wrapped_coordinates[negative_lon_index].longitude + 360,
                )

                wrapped_coordinates[negative_lon_index] = wrapped_coord
        return wrapped_coordinates

import typing

from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.interfaces.meridian_handling.meridian_wrap_coord_transformer import IMeridianWrapCoordTransformer


class MeridianWrapCoordTransformer(IMeridianWrapCoordTransformer):
    def wrap_coordinates(self, coordinates: typing.Sequence[Coordinate]) -> typing.Sequence[Coordinate]:
        if len(coordinates) == 0:
            return coordinates

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

    def reverse_wrap(self, coordinates: typing.Sequence[Coordinate]) -> typing.Sequence[Coordinate]:
        longitudes = [coord.longitude for coord in coordinates]

        wrapped_coordinates = [coord.copy() for coord in coordinates]
        greater_lon_180_indices = []
        less_lon_180_indices = []

        for i, lon in enumerate(longitudes):
            if lon < 180:
                less_lon_180_indices.append(i)
            else:
                greater_lon_180_indices.append(i)

        if len(greater_lon_180_indices) > 0 and len(less_lon_180_indices) > 0:
            for index in greater_lon_180_indices:
                wrapped_coord = Coordinate(
                    latitude=wrapped_coordinates[index].latitude,
                    longitude=wrapped_coordinates[index].longitude - 360,
                )

                wrapped_coordinates[index] = wrapped_coord
        return wrapped_coordinates

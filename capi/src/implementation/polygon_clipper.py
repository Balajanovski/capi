import typing

from capi.src.implementation.datastructures.axis_aligned_box import AxisAlignedBox
from capi.src.implementation.datastructures.polygon import Polygon
from capi.src.implementation.dtos.coordinate import Coordinate
from capi.src.implementation.meridian_handling.meridian_wrap_coord_transformer import MeridianWrapCoordTransformer
from capi.src.interfaces.datastructures.axis_aligned_box import IAxisAlignedBox
from capi.src.interfaces.datastructures.polygon import IPolygon
from capi.src.interfaces.meridian_handling.meridian_wrap_coord_transformer import IMeridianWrapCoordTransformer
from capi.src.interfaces.polygon_clipper import IPolygonClipper


class PolygonClipper(IPolygonClipper):
    _INSIDE = 0
    _LEFT = 1
    _RIGHT = 1 << 1
    _BOTTOM = 1 << 2
    _TOP = 1 << 3

    def __init__(
        self,
        meridian_wrap_transformer: typing.Optional[IMeridianWrapCoordTransformer] = None,
        coord_merge_epsilon_tolerance: float = 0.00000001,
    ):
        self._transformer = (
            MeridianWrapCoordTransformer() if meridian_wrap_transformer is None else meridian_wrap_transformer
        )
        self._epsilon_tolerance = coord_merge_epsilon_tolerance

    def clip_single_polygon(self, polygon: IPolygon, clipping_box: IAxisAlignedBox) -> typing.Optional[IPolygon]:
        polygon_verts = list(polygon.vertices)
        box_verts = [clipping_box.upper_left, clipping_box.lower_right]

        wrapped_verts = list(self._transformer.wrap_coordinates(polygon_verts + box_verts))
        wrapped_poly_verts = wrapped_verts[: len(polygon_verts)]
        wrapped_box_verts = wrapped_verts[-len(box_verts) :]
        wrapped_clipping_box = AxisAlignedBox(wrapped_box_verts[0], wrapped_box_verts[1])

        wrapped_clipped_verts = []
        for i in range(len(wrapped_poly_verts)):
            first_vertex = wrapped_poly_verts[i]
            second_vertex = wrapped_poly_verts[(i + 1) % len(wrapped_poly_verts)]

            clipped_edge = self._cohen_sutherland_line_clipping(first_vertex, second_vertex, wrapped_clipping_box)

            wrapped_clipped_verts.append(clipped_edge[0])
            wrapped_clipped_verts.append(clipped_edge[1])
        wrapped_clipped_verts = list(self._filter_out_redundant_vertices(wrapped_clipped_verts))

        wrapped_clipped_verts_and_box_verts = wrapped_clipped_verts + wrapped_box_verts
        clipped_verts = self._transformer.reverse_wrap(wrapped_clipped_verts_and_box_verts)[
            : len(wrapped_clipped_verts)
        ]

        if len(clipped_verts) < 3:
            return None
        return Polygon(vertices=self._transformer.reverse_wrap(clipped_verts))

    def clip_polygons(
        self, polygons: typing.Sequence[IPolygon], clipping_box: IAxisAlignedBox
    ) -> typing.Sequence[IPolygon]:
        clipped_polygons = []
        for polygon in polygons:
            clipped_polygon = self.clip_single_polygon(polygon, clipping_box)
            if clipped_polygon is not None:
                clipped_polygons.append(clipped_polygon)
        return clipped_polygons

    def _filter_out_redundant_vertices(self, vertices: typing.Sequence[Coordinate]) -> typing.Sequence[Coordinate]:
        dup_filtered = self._filter_out_duplicate_vertices(vertices)
        edge_filtered = self._filter_out_duplicate_grad_vertices(dup_filtered)
        return self._filter_out_duplicate_vertices(edge_filtered)

    def _filter_out_duplicate_vertices(self, vertices: typing.Sequence[Coordinate]) -> typing.Sequence[Coordinate]:
        if len(vertices) <= 1:
            return vertices

        rev_verts = reversed(vertices)

        dup_filtered_vertices = []
        prev_vertex = vertices[0]  # last of reversed list
        for curr_vertex in rev_verts:
            if not self._coords_identical(prev_vertex, curr_vertex, self._epsilon_tolerance):
                dup_filtered_vertices.append(curr_vertex)
                prev_vertex = curr_vertex

        # All vertices must have been identical.
        if len(dup_filtered_vertices) == 1:
            dup_filtered_vertices.append(vertices[0])
        return list(reversed(dup_filtered_vertices))

    def _filter_out_duplicate_grad_vertices(self, vertices: typing.Sequence[Coordinate]) -> typing.Sequence[Coordinate]:
        edge_filtered_vertices = []

        for curr_i in range(len(vertices)):
            prev_i = (curr_i - 1) % len(vertices)
            next_i = (curr_i + 1) % len(vertices)

            if not self._edges_same_grad(
                (vertices[prev_i], vertices[curr_i]), (vertices[curr_i], vertices[next_i]), self._epsilon_tolerance
            ):
                edge_filtered_vertices.append(vertices[curr_i])

        return edge_filtered_vertices

    @staticmethod
    def _cohen_sutherland_line_clipping(
        coordinate_1: Coordinate, coordinate_2: Coordinate, clipping_box: IAxisAlignedBox
    ) -> typing.Tuple[Coordinate, Coordinate]:
        out_code_1 = PolygonClipper._compute_out_code(coordinate_1, clipping_box)
        out_code_2 = PolygonClipper._compute_out_code(coordinate_2, clipping_box)

        curr_coord_1: Coordinate = coordinate_1.copy()
        curr_coord_2: Coordinate = coordinate_2.copy()

        x_max = clipping_box.upper_right.longitude
        x_min = clipping_box.lower_left.longitude
        y_max = clipping_box.upper_right.latitude
        y_min = clipping_box.lower_left.latitude

        while True:
            if out_code_1 == PolygonClipper._INSIDE and out_code_2 == PolygonClipper._INSIDE:
                break
            elif out_code_1 & out_code_2:
                return (
                    Coordinate(
                        longitude=PolygonClipper._clamp(curr_coord_1.longitude, x_min, x_max),
                        latitude=PolygonClipper._clamp(curr_coord_1.latitude, y_min, y_max),
                    ),
                    Coordinate(
                        longitude=PolygonClipper._clamp(curr_coord_2.longitude, x_min, x_max),
                        latitude=PolygonClipper._clamp(curr_coord_2.latitude, y_min, y_max),
                    ),
                )
            else:
                out_of_box_code = out_code_1 if out_code_1 != 0 else out_code_2

                x1 = curr_coord_1.longitude
                x2 = curr_coord_2.longitude
                y1 = curr_coord_1.latitude
                y2 = curr_coord_2.latitude

                if out_of_box_code & PolygonClipper._TOP:
                    x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1)
                    y = y_max
                elif out_of_box_code & PolygonClipper._BOTTOM:
                    x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1)
                    y = y_min
                elif out_of_box_code & PolygonClipper._RIGHT:
                    x = x_max
                    y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1)
                else:
                    x = x_min
                    y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1)

                if out_of_box_code == out_code_1:
                    curr_coord_1 = Coordinate(longitude=x, latitude=y)
                    out_code_1 = PolygonClipper._compute_out_code(curr_coord_1, clipping_box)
                else:
                    curr_coord_2 = Coordinate(longitude=x, latitude=y)
                    out_code_2 = PolygonClipper._compute_out_code(curr_coord_2, clipping_box)

        return curr_coord_1, curr_coord_2

    @staticmethod
    def _compute_out_code(coordinate: Coordinate, clipping_box: IAxisAlignedBox) -> int:
        code = PolygonClipper._INSIDE

        lower_left = clipping_box.lower_left
        upper_right = clipping_box.upper_right

        if coordinate.longitude < lower_left.longitude:
            code |= PolygonClipper._LEFT
        elif coordinate.longitude > upper_right.longitude:
            code |= PolygonClipper._RIGHT

        if coordinate.latitude < lower_left.latitude:
            code |= PolygonClipper._BOTTOM
        elif coordinate.latitude > upper_right.latitude:
            code |= PolygonClipper._TOP

        return code

    @staticmethod
    def _clamp(x: float, lower_bound: float, upper_bound: float) -> float:
        return min(upper_bound, max(lower_bound, x))

    @staticmethod
    def _coords_identical(coord_1: Coordinate, coord_2: Coordinate, epsilon_tolerance: float) -> bool:
        return (
            abs(coord_1.longitude - coord_2.longitude) < epsilon_tolerance
            and abs(coord_1.latitude - coord_2.latitude) < epsilon_tolerance
        )

    @staticmethod
    def _edges_same_grad(
        edge_1: typing.Tuple[Coordinate, Coordinate],
        edge_2: typing.Tuple[Coordinate, Coordinate],
        epsilon_tolerance: float,
    ) -> bool:
        edge_1_undef_grad = abs(edge_1[0].longitude - edge_1[1].longitude) < epsilon_tolerance
        edge_2_undef_grad = abs(edge_2[0].longitude - edge_2[1].longitude) < epsilon_tolerance

        if edge_1_undef_grad and edge_2_undef_grad:
            return True
        if edge_1_undef_grad or edge_2_undef_grad:
            return False

        grad_1 = PolygonClipper._compute_edge_grad(edge_1)
        grad_2 = PolygonClipper._compute_edge_grad(edge_2)

        return abs(grad_1 - grad_2) < epsilon_tolerance

    @staticmethod
    def _compute_edge_grad(edge: typing.Tuple[Coordinate, Coordinate]) -> float:
        return (edge[0].latitude - edge[1].latitude) / (edge[0].longitude - edge[1].longitude)

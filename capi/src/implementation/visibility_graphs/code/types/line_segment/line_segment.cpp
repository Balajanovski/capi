//
// Created by James.Balajan on 31/03/2021.
//

#include <s2/s2point.h>
#include <stdexcept>

#include "line_segment.hpp"

Coordinate LineSegment::get_endpoint_1() const { return _endpoint_1; }

Coordinate LineSegment::get_endpoint_2() const { return _endpoint_2; }

Coordinate LineSegment::get_adjacent_to(const Coordinate &point) const {
    if (point == _endpoint_1) {
        return _endpoint_2;
    } else if (point == _endpoint_2) {
        return _endpoint_1;
    } else {
        throw std::runtime_error("Point queried in get_adjacent_to is not part of edge");
    }
}

std::optional<Coordinate> LineSegment::intersection_with_segment(const LineSegment &line_segment) const {
    /*
     * https://math.stackexchange.com/a/3176648
     */

    const auto a = _endpoint_1;
    const auto b = _endpoint_2;
    const auto c = line_segment._endpoint_1;
    const auto d = line_segment._endpoint_2;

    const auto n = b - a;
    const auto m = c - d;
    const auto p = c - a;

    const auto D = n.get_longitude_microdegrees_long() * m.get_latitude_microdegrees_long() - n.get_latitude_microdegrees_long() * m.get_longitude_microdegrees_long();
    const auto Qx = m.get_latitude_microdegrees_long() * p.get_longitude_microdegrees_long() - m.get_longitude_microdegrees_long() * p.get_latitude_microdegrees_long();
    const auto Qy = n.get_longitude_microdegrees_long() * p.get_latitude_microdegrees_long() - n.get_latitude_microdegrees_long() * p.get_longitude_microdegrees_long();

    if (D == 0) {
        // Parallel

        const auto endpoint_1_matches =
                _endpoint_1 == line_segment._endpoint_1 || _endpoint_1 == line_segment._endpoint_2;
        const auto endpoint_2_matches =
                _endpoint_2 == line_segment._endpoint_1 || _endpoint_2 == line_segment._endpoint_2;

        if (endpoint_1_matches) {
            return _endpoint_1;
        } else if (endpoint_2_matches) {
            return _endpoint_2;
        } else {
            return {};
        }
    } if (!((D >= 0 && Qx >= 0 && Qy >= 0) || (D <= 0 && Qx <= 0 && Qy <= 0)) ||
            std::abs(D) < std::abs(Qx) ||
            std::abs(D) < std::abs(Qy)) {
        // No intersection

        return {};
    }

    const auto t = static_cast<double>(Qx) / static_cast<double>(D);
    return a + (n * t);
}

Coordinate LineSegment::get_tangent_vector() const { return _endpoint_2 - _endpoint_1; }

Orientation LineSegment::orientation_of_point_to_segment(const Coordinate &point) const {
    const auto point_lon = point.get_longitude_microdegrees_long();
    const auto point_lat = point.get_latitude_microdegrees_long();
    const auto point_1_lon = _endpoint_1.get_longitude_microdegrees_long();
    const auto point_1_lat = _endpoint_1.get_latitude_microdegrees_long();
    const auto point_2_lon = _endpoint_2.get_longitude_microdegrees_long();
    const auto point_2_lat = _endpoint_2.get_latitude_microdegrees_long();

    const auto signed_area = (point_2_lon - point_1_lon) * (point_lat - point_1_lat) -
                             (point_2_lat - point_1_lat) * (point_lon - point_1_lon);
    if (signed_area > 0) {
        return Orientation::COUNTER_CLOCKWISE;
    }
    if (signed_area < 0) {
        return Orientation::CLOCKWISE;
    }
    return Orientation::COLLINEAR;
}

bool LineSegment::on_segment(const Coordinate &point) const {
    const auto point_orientation = orientation_of_point_to_segment(point);

    if (point_orientation != Orientation::COLLINEAR) {
        return false;
    }

    return ((_endpoint_1.get_longitude() <= point.get_longitude() &&
             point.get_longitude() <= _endpoint_2.get_longitude()) ||
            (_endpoint_2.get_longitude() <= point.get_longitude() &&
             point.get_longitude() <= _endpoint_1.get_longitude())) &&
           ((_endpoint_1.get_latitude() <= point.get_latitude() &&
             point.get_latitude() <= _endpoint_2.get_latitude()) ||
            (_endpoint_2.get_latitude() <= point.get_latitude() && point.get_latitude() <= _endpoint_1.get_latitude()));
}

Coordinate LineSegment::project(const Coordinate &point) const {
    const auto point_s2 = point.to_s2_point();
    const auto polyline_s2 = this->to_s2_polyline();

    int next_vertex;
    const auto projected_point_s2 = polyline_s2->Project(point_s2, &next_vertex);

    delete polyline_s2;

    return Coordinate(projected_point_s2);
}

bool LineSegment::operator==(const LineSegment &other) const {
    return ((_endpoint_1 == other._endpoint_1) && (_endpoint_2 == other._endpoint_2)) ||
           ((_endpoint_1 == other._endpoint_2) && (_endpoint_2 == other._endpoint_1));
}

bool LineSegment::operator!=(const LineSegment &other) const { return !((*this) == other); }

S2Polyline *LineSegment::to_s2_polyline() const {
    return new S2Polyline(std::vector<S2Point>{_endpoint_1.to_s2_point(), _endpoint_2.to_s2_point()});
}

std::size_t std::hash<LineSegment>::operator()(const LineSegment &segment) const {
    const std::size_t coord_1_hash = hash<Coordinate>()(segment.get_endpoint_1());
    const std::size_t coord_2_hash = hash<Coordinate>()(segment.get_endpoint_2());
    return coord_1_hash ^ (coord_2_hash + 0x9e3779b9 + (coord_1_hash << 6) + (coord_1_hash >> 2));
}

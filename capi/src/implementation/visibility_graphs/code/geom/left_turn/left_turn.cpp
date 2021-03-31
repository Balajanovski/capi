//
// Created by James.Balajan on 31/03/2021.
//

#include "left_turn.hpp"

bool left_turn(Coordinate p, Coordinate q, Coordinate r) {
  const auto d = (r.get_longitude() - p.get_longitude()) *
                     (q.get_latitude() - p.get_latitude()) -
                 (r.get_latitude() - p.get_latitude()) *
                     (q.get_longitude() - p.get_longitude());
  return d < 0;
}

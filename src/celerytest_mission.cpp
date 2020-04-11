#include "../include/celerytest_mission.hpp"
using namespace celerytest;

bool mission_flat::tick() {
  if (has_rendered_before) {
    return false;
  }
  has_rendered_before = true;
  return true;
}

std::vector<static_vert> mission_flat::get_terrain_data() {
  return std::vector<static_vert>{static_vert(0.0f, 0.0f, 0.0f, 0.0f)};
}

size_t mission_flat::get_terrain_pitch() {
  return 1;
}
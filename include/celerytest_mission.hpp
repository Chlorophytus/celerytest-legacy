#pragma once
#include "celerytest.hpp"
#include "celerytest_sim.hpp"

namespace celerytest {
enum class mission_types : U8 {
  // Null mission, no floor
  null,

  // Slate mission, flat plane as floor
  flat,

  // Terrained mission, heightmap as floor
  terrain,

  // Cube mission, no floor but has voxels
  voxel,
};
struct static_vert {
  F32 x, y, z, w;
};
struct mission_null : sim_object {
  virtual const sim_types get_type() const { return sim_types::missionobject; }
  virtual const mission_types get_subtype() const {
    return mission_types::null;
  }
  // re-render scene?
  virtual bool tick() { return false; };

  // Don't run these functions here, you should never run these.
  virtual std::vector<static_vert> get_terrain_data() { assert(false); };
  virtual size_t get_terrain_pitch() { assert(false); };
};
struct mission_flat : mission_null {
  bool has_rendered_before;
  // These return a flat map
  virtual std::vector<static_vert> get_terrain_data();
  // NOTE: Pitch by verts.
  virtual size_t get_terrain_pitch();
  virtual bool tick();
  virtual const mission_types get_subtype() const {
    return mission_types::flat;
  }
};
struct mission_terrain : mission_null {
  virtual const mission_types get_subtype() const {
    return mission_types::terrain;
  }
};
struct mission_voxel : mission_null {
  virtual const mission_types get_subtype() const {
    return mission_types::voxel;
  }
};

} // namespace celerytest

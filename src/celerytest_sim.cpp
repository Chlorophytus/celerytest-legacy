#include "../include/celerytest_sim.hpp"
#include "../include/celerytest_env2d.hpp"
#include "../include/celerytest_env3d.hpp"
#include "../include/celerytest_log.hpp"
#include "../include/celerytest_mission.hpp"
using namespace celerytest;

std::unique_ptr<sim_keys_t> sim_keys{nullptr};
std::unique_ptr<sim_vals_t> sim_vals{nullptr};

// Initialize the sim
void celerytest::sim_init() {
  celerytest::log(severity::warn, {"Sim creation time."});
  sim_keys = std::make_unique<sim_keys_t>();
  sim_vals = std::make_unique<sim_vals_t>();
  celerytest::log(severity::warn, {"Sim creation finished."});
}

// Deinit...deletes everything, hopefully RAII applies
void celerytest::sim_deinit() {
  celerytest::log(severity::warn,
                  {"Sim teardown time, will delete ",
                   std::to_string(sim_vals->size()), " objects..."});
  celerytest::prune_loggers();
  sim_keys = nullptr;
  sim_vals = nullptr;
  celerytest::log(severity::warn, {"Sim teardown finished."});
}

U32 celerytest::sim_create(int Ltype) { return sim_create_hint(Ltype, 0); }

sim_object *celerytest::sim_reference(U32 where) {
  if (sim_keys->test(where)) {
    return sim_vals->at(where);
  }
  return nullptr;
}

void celerytest::sim_delete(U32 where) {
  assert(sim_keys->test(where));
  (*sim_keys)[where] = false;
  delete sim_vals->at(where);
}

U32 celerytest::sim_create_hint(int Ltype, U32 hint) {
  // derp stuff to check where in the universe a free object is
  // if not then oh god we've filled a lot of objects, and you pat yourself on
  // the back
  auto where = hint;
  assert(!sim_keys->all());
  while (sim_keys->test(++where))
    ;
  (*sim_keys)[where] = true;
  switch (Ltype) {
    // UI objects and stuff
  case 1:
    sim_vals->try_emplace(where, new sim_object());
    return where;
  case 2:
    log(severity::error,
        {"You are creating a deprecated object (shaderobject). Stop."});
    assert(false);
    return where;
  case 3:
    log(severity::error,
        {"You are creating a deprecated object (shaderlist). Stop."});
    assert(false);
    return where;
  case 4:
    sim_vals->try_emplace(where, new env2d_uiobject());
    return where;
  case 5:
    sim_vals->try_emplace(where, new env2d_conobject());
    return where;
  case 6:
    sim_vals->try_emplace(where, new env3d_shaderprogram());
    return where;
  case 7:
    sim_vals->try_emplace(where, new env3d_shaderopaque());
    return where;
    // Mission objects
  case 16:
    sim_vals->try_emplace(where, new mission_null());
    return where;
  case 17:
    sim_vals->try_emplace(where, new mission_flat());
    return where;
  case 18:
    sim_vals->try_emplace(where, new mission_terrain());
    return where;
  case 19:
    sim_vals->try_emplace(where, new mission_voxel());
    return where;
  default:
    log(severity::error,
        {"Can't create bogus object type ", std::to_string(Ltype)});
    return 0;
  }
}
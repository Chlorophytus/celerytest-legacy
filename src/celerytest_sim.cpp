#include "../include/celerytest_sim.hpp"
#include "../include/celerytest_log.hpp"
#include "../include/celerytest_env2d.hpp"
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
  sim_keys = nullptr;
  sim_vals = nullptr;
  celerytest::log(severity::warn, {"Sim teardown finished."});
}

U32 celerytest::sim_create(int Ltype) {
  return sim_create_hint(Ltype, 0);
}

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
  case 1:
    sim_vals->try_emplace(where, new sim_object());
    return where;
  case 2:
    sim_vals->try_emplace(where, new sim_shaderobject());
    return where;
  case 3:
    sim_vals->try_emplace(where, new sim_shaderlist());
    return where;
  case 4:
    sim_vals->try_emplace(where, new env2d_uiobject());
    return where;
  default:
    log(severity::error,
        {"Can't create bogus object type ", std::to_string(Ltype)});
    return 0;
  }
}
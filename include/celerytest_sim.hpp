#pragma once
#include "celerytest.hpp"

namespace celerytest {
enum class sim_types : U8 { object, shaderobject, shaderlist };
struct sim_object {
  virtual const sim_types get_type() const { return sim_types::object; }
};
struct sim_shaderobject : sim_object {
  virtual const sim_types get_type() const { return sim_types::shaderobject; }
};
struct sim_shaderlist : sim_object {
  virtual const sim_types get_type() const { return sim_types::shaderlist; }
};
using sim_keys_t = std::bitset<UINT32_MAX>;
using sim_vals_t = std::unordered_map<U32, sim_object *>;
void sim_init();

// Little brother `create` is dangerously slow when cache is full.
U32 sim_create(int);
// Big brother `create_hint` isn't slow until RAM is too pressured.
U32 sim_create_hint(int, U32);

sim_object *sim_reference(U32);
void sim_delete(U32);
void sim_deinit();

} // namespace celerytest

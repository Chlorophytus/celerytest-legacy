#pragma once
#include "celerytest.hpp"

namespace celerytest {
struct lua {
  lua_State *L;
  lua();
  void load(const char *);
  ~lua();
};
} // namespace celerytest

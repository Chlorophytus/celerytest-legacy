#pragma once
#include "celerytest.hpp"

namespace celerytest {
struct lua {
  lua_State *L;
  lua();
  ~lua();
};
} // namespace celerytest

#include "../include/celerytest_log.hpp"
#include "../include/celerytest_lua.hpp"
using namespace celerytest;

const std::filesystem::path root(celerytest_SDIR);

lua::lua() {
  L = luaL_newstate();
  assert(L);
  luaL_openlibs(L);
}

void lua::load(const char *path) {
  celerytest::log(celerytest::severity::warn,
                  {"unimplemented: lua::load(const char *)"});
}

lua::~lua() { lua_close(L); }
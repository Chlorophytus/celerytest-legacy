#pragma once
#include "celerytest.hpp"

namespace celerytest {
enum class severity : U8 {
  info,
  warn,
  error,
};
void log(severity, std::forward_list<std::string_view> &&);
} // namespace celerytest
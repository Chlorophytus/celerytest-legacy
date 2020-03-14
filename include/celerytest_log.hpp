#pragma once
#include "celerytest.hpp"

namespace celerytest {
enum class severity : U8 {
  info,
  warn,
  error,
};
void log(severity, std::forward_list<std::string_view> &&);
using log_fun = void(*)(severity, std::forward_list<std::string_view> &&);
void emplace_logger(log_fun);
void log_printf(severity, std::forward_list<std::string_view> &&);
void log_window(severity, std::forward_list<std::string_view> &&);
} // namespace celerytest
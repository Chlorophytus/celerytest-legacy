#pragma once
// Console logging helpers
#include "celerytest.hpp"

namespace celerytest {
namespace con {
enum class severity : U8 {
  emergency = 0x01,
  alert = 0x02,
  critical = 0x04,
  error = 0x08,
  warning = 0x10,
  notice = 0x20,
  informational = 0x40,
  debug = 0x80
};

struct listener {
#ifdef NDEBUG
  severity current_level = severity::informational;
#else
  severity current_level = severity::debug;
#endif
  virtual void log(const severity &, const char *){};
};
struct file_listener : listener {
  FILE *file{nullptr};
  file_listener(std::filesystem::path &&);
  void log(const severity &, const char *);
  ~file_listener();
};
struct stdout_listener : listener {
  void log(const severity &, const char *);
};

void init();
void log_cpuid();
bool check_sse();
bool check_avx();
size_t attach(con::listener *);
void log_all(severity &&, std::string &&);
void detach(const size_t);
void deinit();
} // namespace con
} // namespace celerytest

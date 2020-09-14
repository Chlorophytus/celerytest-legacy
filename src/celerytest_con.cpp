#include "../include/celerytest_con.hpp"

using namespace celerytest;

static std::unique_ptr<std::vector<con::listener *>> listeners{nullptr};

// === CON CONTAINER ==========================================================
void con::init() {
  listeners = std::make_unique<std::vector<con::listener *>>();
}

size_t con::attach(con::listener *ptr) {
  listeners->emplace_back(ptr);
  return listeners->size();
}

void con::log_all(con::severity &&severity, std::string &&strings) {
  auto format = std::stringstream{};

  // We should set HH:MM:SS.III
  auto t = std::chrono::system_clock::now();
  auto t_time = std::chrono::system_clock::to_time_t(t);
  char t_buffer[64]{0};
  std::strftime(t_buffer, sizeof(char) * 63, "%T", std::localtime(&t_time));
  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                    t.time_since_epoch())
                    .count() %
                1000;

  switch (severity) {
  case con::severity::emergency: {
    format << std::string{"[ EMG ] ["};
    break;
  }
  case con::severity::alert: {
    format << std::string{"[ ALT ] ["};
    break;
  }
  case con::severity::critical: {
    format << std::string{"[ CRT ] ["};
    break;
  }
  case con::severity::error: {
    format << std::string{"[ ERR ] ["};
    break;
  }
  case con::severity::warning: {
    format << std::string{"[ WRN ] ["};
    break;
  }
  case con::severity::notice: {
    format << std::string{"[ NOT ] ["};
    break;
  }
  case con::severity::informational: {
    format << std::string{"[ INF ] ["};
    break;
  }
  case con::severity::debug: {
    format << std::string{"[ DEB ] ["};
    break;
  }
  }
  format << t_buffer;
  format << ".";
  format << std::setw(3) << std::setfill('0') << millis;
  format << std::setw(0) << std::setfill(' ') << "] ";
  for (auto &&string : strings) {
    format << string;
  }
  for (auto &&listener : *listeners) {
    listener->log(severity, format.str().c_str());
  }
}

void con::detach(const size_t where) {
  listeners->erase(listeners->begin() + where);
}

void con::deinit() { listeners = nullptr; }

// === INTRINSICS CHECKS ======================================================
bool con::check_sse() {
#ifdef WIN32
  U32 cpuid_info[4]{0x00000000};
  __cpuid(reinterpret_cast<int *>(cpuid_info), 0x00000001);
  // Windows doesn't have GCC's builtins. Do this by hand.
  // Check for both SSE4.1 and SSE4.2.
  return (cpuid_info[2] & 0x00180000) == 0x00180000;
#else
  return __builtin_cpu_supports("sse4.1") && __builtin_cpu_supports("sse4.2");
#endif
}
bool con::check_avx() {
#ifdef WIN32
  U32 cpuid_info[4]{0x00000000};
  __cpuid(reinterpret_cast<int *>(cpuid_info), 0x00000001);
  // Here it gets a little bit weirder. We have to check the weird %ymm context
  // saving features.
  if ((cpuid_info[2] & 0x18000000) == 0x18000000) {
    U32 feature_mask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
    return (feature_mask & 0x00000006) == 0x00000006;
  }
  return false;
#else
  return __builtin_cpu_supports("avx2");
#endif
}
void con::log_cpuid() {
  U32 cpuid_args[4]{0x00000000};

#ifdef WIN32
  __cpuid(reinterpret_cast<int *>(cpuid_args), 0x80000000);
#else
  __cpuid(0x80000000, cpuid_args[0], cpuid_args[1], cpuid_args[2],
          cpuid_args[3]);
#endif

  if (cpuid_args[0] < 0x80000004) {
    throw std::runtime_error{"Please use a genuine non-legacy Intel/AMD CPU"};
  }
  auto procinfo = std::string{};
  for (auto i = 0x80000002; i < 0x80000005; i++) {
#ifdef WIN32
    __cpuid(reinterpret_cast<int *>(cpuid_args), i);
#else
    __cpuid(i, cpuid_args[0], cpuid_args[1], cpuid_args[2], cpuid_args[3]);
#endif

    for (auto j = 0; j < 4; j++) {
      procinfo += static_cast<U8>((cpuid_args[j] & 0x000000FF) >> 0x00);
      procinfo += static_cast<U8>((cpuid_args[j] & 0x0000FF00) >> 0x08);
      procinfo += static_cast<U8>((cpuid_args[j] & 0x00FF0000) >> 0x10);
      procinfo += static_cast<U8>((cpuid_args[j] & 0xFF000000) >> 0x18);
    }
  }
  con::log_all(con::severity::informational,
               std::string{"x86 ... CPU: "} + procinfo);
  con::log_all(
      con::severity::informational,
      std::string{"x86 ... SSE4.x: "} +
          std::string{con::check_sse() ? "all present" : "NOT all present"});
  con::log_all(con::severity::informational,
               std::string{"x86 ... AVX2: "} +
                   std::string{con::check_avx() ? "present" : "NOT present"});
}

// === FILE LISTENERS =========================================================
con::file_listener::file_listener(std::filesystem::path &&path)
    : file{std::fopen(path.string().c_str(), "w")} {}

void con::file_listener::log(const con::severity &severity,
                             const char *fancy_string) {
  if (static_cast<U8>(current_level) >= static_cast<U8>(severity)) {
    std::fprintf(file, "%s\n", fancy_string);
  }
}

con::file_listener::~file_listener() { std::fclose(file); }

// === STDOUT LISTENERS =======================================================
void con::stdout_listener::log(const con::severity &severity,
                               const char *fancy_string) {
  if (static_cast<U8>(current_level) >= static_cast<U8>(severity)) {
    std::fprintf(stderr, "%s\n", fancy_string);
  }
}

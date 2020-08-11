#include "../include/celerytest_con.hpp"

using namespace celerytest;

static std::unique_ptr<std::vector<con::listener *>> listeners{nullptr};

// === CON CONTAINER ==========================================================
void con::init() {
  listeners = std::make_unique<std::vector<con::listener *>>();
}

const size_t con::attach(con::listener *ptr) {
  listeners->emplace_back(ptr);
  return listeners->size();
}

void con::log_all(con::severity &&severity,
                  std::forward_list<std::string> &&strings) {
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
    format << "[ EMG ] [";
    break;
  }
  case con::severity::alert: {
    format << "[ ALT ] [";
    break;
  }
  case con::severity::critical: {
    format << "[ CRT ] [";
    break;
  }
  case con::severity::error: {
    format << "[ ERR ] [";
    break;
  }
  case con::severity::warning: {
    format << "[ WRN ] [";
    break;
  }
  case con::severity::notice: {
    format << "[ NOT ] [";
    break;
  }
  case con::severity::informational: {
    format << "[ INF ] [";
    break;
  }
  case con::severity::debug: {
    format << "[ DEB ] [";
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

void con::deinit() {
  listeners = nullptr;
}

// === FILE LISTENERS =========================================================
con::file_listener::file_listener(std::filesystem::path &&path)
    : file{std::fopen(path.c_str(), "w")} {}

void con::file_listener::log(const con::severity &severity,
                             const char *fancy_string) {
  if (static_cast<U8>(current_level) <= static_cast<U8>(severity)) {
    std::fprintf(file, "%s\n", fancy_string);
  }
}

con::file_listener::~file_listener() { std::fclose(file); }

// === STDOUT LISTENERS =======================================================
void con::stdout_listener::log(const con::severity &severity,
                               const char *fancy_string) {
  if (static_cast<U8>(current_level) <= static_cast<U8>(severity)) {
    std::fprintf(stderr, "%s\n", fancy_string);
  }
}
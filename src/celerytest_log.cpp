#include "../include/celerytest_log.hpp"
using namespace celerytest;

std::unique_ptr<std::forward_list<log_fun>> funs{nullptr};

void celerytest::emplace_logger(log_fun fun) {
  funs->emplace_front(fun);
}

void celerytest::log(severity s, std::forward_list<std::string_view> &&msg) {
  if (!funs) {
    funs = std::make_unique<std::forward_list<log_fun>>();
    funs->emplace_front(&celerytest::log_printf);
  }
  for (auto &&fun : *funs) {
    fun(s, std::forward_list<std::string_view>{msg});
  }
}

void celerytest::log_printf(severity s,
                            std::forward_list<std::string_view> &&msg) {
  auto msg_buffer = new char[4096]{0};
  auto millis = U16{0};

  auto scount = U16{0};

  for (auto &&msglet : msg) {
    auto md = msglet;
    for (auto &&ch : md) {
      msg_buffer[scount] = ch;
      assert(scount++ < 4096);
    }
  }

  // We should set HH:MM:SS.III
  auto t = std::chrono::system_clock::now();
  auto t_time = std::chrono::system_clock::to_time_t(t);
  char t_buffer[64]{0};
  std::strftime(t_buffer, sizeof(t_buffer), "%T", std::localtime(&t_time));
  millis = std::chrono::duration_cast<std::chrono::milliseconds>(
               t.time_since_epoch())
               .count() %
           1000;

  switch (s) {
  case severity::error:
    std::printf("\x1B[31m[%s.%0.3hu]\x1B[0m %s\n", t_buffer, millis,
                msg_buffer);
    break;
  case severity::warn:
    std::printf("\x1B[90m[%s.%0.3hu]\x1B[0m %s\n", t_buffer, millis,
                msg_buffer);
    break;
  default:
    std::printf("[%s.%0.3hu] %s\n", t_buffer, millis, msg_buffer);
    break;
  }
  delete[] msg_buffer;
}

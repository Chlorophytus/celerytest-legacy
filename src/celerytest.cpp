#include "../include/celerytest.hpp"
#include "../include/celerytest_interwork.hpp"
#include "../include/celerytest_lua.hpp"

int main(int argc, char **argv) {
  std::printf("celerytest %s\n", celerytest_VSTRING_FULL);

  // If you don't specify the Width/Height then I will specify it for you.
  auto w = U16((argc == 2) ? std::atoi(argv[1]) : 0);
  auto h = U16((argc == 2) ? std::atoi(argv[2]) : 0);

  if (w == 0) {
    w = 640;
  }
  if (h == 0) {
    h = 480;
  }

  // SDL will return a non-zero value for failure in initialization.
  assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
  auto inter = std::make_unique<celerytest::interwork>(w, h);

  while (inter->tick())
    SDL_Delay(10);

  // Quit SDL lastly, **LASTLY**.
  SDL_Quit();
  return 0;
}
/*  Copyright (c) 2022 Mitya Selivanov
 */

#include <iostream>
#include <tuple>

#include <SDL.h>
#include <SDL_video.h>

namespace cute::example {
  using std::cout, std::tuple;

  static constexpr int default_window_width  = 1024;
  static constexpr int default_window_height = 768;

  [[nodiscard]] auto default_window_rect() noexcept
      -> tuple<int, int, int, int> {
    int x      = SDL_WINDOWPOS_UNDEFINED;
    int y      = SDL_WINDOWPOS_UNDEFINED;
    int width  = default_window_width;
    int height = default_window_height;

    auto display_mode = SDL_DisplayMode {};
    if (SDL_GetCurrentDisplayMode(0, &display_mode) < 0) {
      cout << "SDL_GetCurrentDisplayMode failed: " << SDL_GetError()
           << '\n';
    } else {
      x = display_mode.w / 2 - width / 2;
      y = display_mode.h / 2 - height / 2;
    }

    return { x, y, width, height };
  }

  [[nodiscard]] auto init() noexcept -> bool {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      cout << "SDL_Init failed: " << SDL_GetError() << '\n';
      return false;
    }

    return true;
  }

  [[nodiscard]] auto create_window() noexcept -> SDL_Window * {
    auto [x, y, width, height] = default_window_rect();

    auto window = SDL_CreateWindow(
        "Cute Example", x, y, width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == nullptr) {
      cout << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
      return nullptr;
    }

    return window;
  }

  void event_loop() noexcept {
    for (bool done = false; !done;) {
      auto event = SDL_Event {};
      while (SDL_PollEvent(&event) == 1)
        if (event.type == SDL_QUIT)
          done = true;
    }
  }

  void run() {
    if (init()) {
      if (auto window = create_window(); window != nullptr) {
        event_loop();
        SDL_DestroyWindow(window);
      }

      SDL_Quit();
    }
  }
}

auto main(int argc, char **argv) -> int {
  cute::example::run();
  return 0;
}

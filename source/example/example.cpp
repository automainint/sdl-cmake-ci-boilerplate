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

  [[nodiscard]] auto init() noexcept -> bool {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      cout << "SDL_Init failed: " << SDL_GetError() << '\n';
      return false;
    }

    return true;
  }

  [[nodiscard]] auto create_window() noexcept -> SDL_Window * {
    auto window = SDL_CreateWindow(
        "Cute Example", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, default_window_width,
        default_window_height, SDL_WINDOW_RESIZABLE);

    if (window == nullptr) {
      cout << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
      return nullptr;
    }

    return window;
  }

  void render(SDL_Renderer *renderer) noexcept {
    SDL_SetRenderDrawColor(renderer, 0x70, 0x78, 0x90, 0xff);
    SDL_RenderClear(renderer);

    auto rect = SDL_Rect {};
    SDL_RenderGetViewport(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0x90, 0x70, 0x78, 0xff);
    rect.x += 40;
    rect.y += 40;
    rect.w -= 80;
    rect.h -= 80;
    SDL_RenderFillRect(renderer, &rect);
  }

  void event_loop(SDL_Renderer *renderer) noexcept {
    for (bool done = false; !done;) {
      auto event = SDL_Event {};
      while (SDL_PollEvent(&event) == 1)
        if (event.type == SDL_QUIT)
          done = true;

      render(renderer);

      SDL_RenderPresent(renderer);
    }
  }

  void run() {
    if (init()) {
      if (auto window = create_window(); window != nullptr) {
        auto renderer = SDL_CreateRenderer(window, -1, 0);

        if (renderer == nullptr)
          cout << "SDL_CreateSoftwareRenderer failed: "
               << SDL_GetError() << '\n';
        else {
          event_loop(renderer);

          SDL_DestroyRenderer(renderer);
        }

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

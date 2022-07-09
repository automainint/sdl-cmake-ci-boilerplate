/*  Copyright (c) 2022 Mitya Selivanov
 */

#include <cute/cute.h>

#include <iostream>

#include <SDL.h>
#include <SDL_video.h>

namespace cute::example {
  static constexpr int default_window_width  = 1024;
  static constexpr int default_window_height = 768;

  void log() noexcept { }

  void log(auto arg, auto... args) noexcept {
    std::cout << arg;
    log(args...);
  }

  [[nodiscard]] auto init() noexcept -> bool {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      log("SDL_Init failed: ", SDL_GetError(), '\n');
      return false;
    }

    return true;
  }

  [[nodiscard]] auto create_window() noexcept -> SDL_Window * {
    auto window = SDL_CreateWindow(
        "Cute Example", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, default_window_width,
        default_window_height, SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
      log("SDL_CreateWindow failed: ", SDL_GetError(), '\n');

    return window;
  }

  [[nodiscard]] auto create_renderer(SDL_Window *window) noexcept
      -> SDL_Renderer * {
    if (window == nullptr)
      return nullptr;

    auto renderer = SDL_CreateRenderer(window, -1,
                                       SDL_RENDERER_ACCELERATED |
                                           SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr)
      log("SDL_CreateSoftwareRenderer failed: ", SDL_GetError(),
          '\n');

    auto info = SDL_RendererInfo {};

    if (SDL_GetRendererInfo(renderer, &info) < 0)
      log("SDL_GetRendererInfo failed: ", SDL_GetError(), '\n');
    else
      log("SDL renderer: ", info.name, '\n');

    return renderer;
  }

  struct render_buffer {
    int          width   = 0;
    int          height  = 0;
    SDL_Texture *texture = nullptr;
  };

  [[nodiscard]] auto update_size(SDL_Renderer *renderer,
                                 render_buffer buffer) noexcept
      -> render_buffer {
    if (renderer == nullptr)
      return buffer;

    auto rect = SDL_Rect {};
    SDL_RenderGetViewport(renderer, &rect);

    if (buffer.texture != nullptr && buffer.width == rect.w &&
        buffer.height == rect.h)
      return buffer;

    if (buffer.texture != nullptr)
      SDL_DestroyTexture(buffer.texture);

    buffer.width  = rect.w;
    buffer.height = rect.h;

    buffer.texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
        buffer.width, buffer.height);

    if (buffer.texture == nullptr)
      log("SDL_CreateTexture failed: ", SDL_GetError(), '\n');

    return buffer;
  }

  struct pixel_type {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  };

  static_assert(sizeof(pixel_type) == 4);

  void render(SDL_Renderer *renderer, render_buffer buffer) noexcept {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (buffer.texture != nullptr) {
      pixel_type *pixels = nullptr;
      int         pitch  = 0;

      if (SDL_LockTexture(buffer.texture, nullptr,
                          reinterpret_cast<void **>(&pixels),
                          &pitch) < 0)
        log("SDL_LockTexture failed: ", SDL_GetError(), '\n');
      else {
        pitch /= sizeof(pixel_type);

        for (ptrdiff_t j = 0; j < buffer.height; j++)
          for (ptrdiff_t i = 0, n = j * pitch; i < buffer.width;
               i++, n++) {
            pixels[n].r = i;
            pixels[n].g = j;
            pixels[n].b = i + j;
            pixels[n].a = 255;
          }

        SDL_UnlockTexture(buffer.texture);
      }

      SDL_RenderCopy(renderer, buffer.texture, nullptr, nullptr);
    }

    SDL_RenderPresent(renderer);
  }

  void event_loop(SDL_Renderer *renderer) noexcept {
    if (renderer == nullptr)
      return;

    auto buffer = update_size(renderer, {});

    for (bool done = false; !done;) {
      auto event = SDL_Event {};
      while (SDL_PollEvent(&event) == 1)
        if (event.type == SDL_QUIT)
          done = true;

      buffer = update_size(renderer, buffer);

      render(renderer, buffer);
    }
  }

  void run() {
    if (!init())
      return;

    auto window   = create_window();
    auto renderer = create_renderer(window);

    event_loop(renderer);

    if (renderer != nullptr)
      SDL_DestroyRenderer(renderer);
    if (window != nullptr)
      SDL_DestroyWindow(window);

    SDL_Quit();
  }
}

auto main(int argc, char **argv) -> int {
  cute::example::run();
  return 0;
}

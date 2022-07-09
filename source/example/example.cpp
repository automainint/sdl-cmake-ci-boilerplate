/*  Copyright (c) 2022 Mitya Selivanov
 */

#include <cute/cute.h>

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>

#include <SDL.h>
#include <SDL_video.h>

namespace cute::example {
  using std::is_same_v, std::decay_t, std::pmr::vector;

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

  [[nodiscard]] auto world() noexcept -> cute::state & {
    static auto world = cute::state {};
    return world;
  }

  [[nodiscard]] auto fragment_stage() noexcept -> cute::state & {
    static auto stage = cute::state {};
    return stage;
  }

  void create() noexcept {
    world() = world().form([](cute::state const &, auto in)
                               -> vector<cute::primitive_type> {
      auto f = [](auto x) -> float {
        return static_cast<float>(x & 255) / 255;
      };

      if constexpr (is_same_v<decay_t<decltype(in)>,
                              cute::fragment_in>) {
        auto v = vector<cute::primitive_type> {
          &cute::memory_resource
        };
        v.reserve(in.area.width * in.area.height);

        for (auto y = in.area.y; y < in.area.y + in.area.height; ++y)
          for (auto x = in.area.x; x < in.area.x + in.area.width; ++x)
            v.emplace_back(
                cute::fragment_out { .position = { .x = x, .y = y },
                                     .color    = { .red   = f(x),
                                                   .green = f(y),
                                                   .blue  = f(x + y),
                                                   .alpha = 1 } });

        return v;
      }

      return {};
    });
  }

  void animation(int64_t time_elapsed) noexcept { }

  void render(int width, int height) noexcept {
    fragment_stage() = world()
                           .put(cute::fragment_in {
                               .area = { 0, 0, width, height } })
                           .cycle();
  }

  [[nodiscard]] auto pixel(int index) noexcept -> pixel_type {
    auto convert = [](float c) -> uint8_t {
      auto b = static_cast<int>(c * 255.f);
      return b < 0 ? 0 : b > 255 ? 255 : static_cast<uint8_t>(b);
    };

    auto color = fragment_stage().fragment(index);

    return { .r = convert(color.red),
             .g = convert(color.green),
             .b = convert(color.blue),
             .a = convert(color.alpha) };
  }

  void frame(SDL_Renderer *renderer, render_buffer buffer,
             int64_t time_elapsed) noexcept {
    animation(time_elapsed);

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

        render(buffer.width, buffer.height);

        for (int j = 0; j < buffer.height; j++)
          for (int i = 0, n = j * buffer.width; i < buffer.width;
               i++, n++)
            pixels[n] = pixel(n);

        SDL_UnlockTexture(buffer.texture);
      }

      SDL_RenderCopy(renderer, buffer.texture, nullptr, nullptr);
    }

    SDL_RenderPresent(renderer);
  }

  void event_loop(SDL_Renderer *renderer) noexcept {
    using clock = std::chrono::steady_clock;
    using std::chrono::duration_cast, std::chrono::milliseconds;

    if (renderer == nullptr)
      return;

    auto buffer = update_size(renderer, {});

    create();

    auto time = clock::now();

    for (bool done = false; !done;) {
      auto event = SDL_Event {};
      while (SDL_PollEvent(&event) == 1)
        if (event.type == SDL_QUIT)
          done = true;

      buffer = update_size(renderer, buffer);

      auto time_elapsed = duration_cast<milliseconds>(clock::now() -
                                                      time);

      frame(renderer, buffer, time_elapsed.count());

      time += time_elapsed;
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

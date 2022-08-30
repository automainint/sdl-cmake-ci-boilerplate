/*  Copyright (c) 2022 Mitya Selivanov
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>
#include <SDL_video.h>

enum { DEFAULT_WINDOW_WIDTH = 1024, DEFAULT_WINDOW_HEIGHT = 768 };

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} pixel_t;

typedef struct {
  int          width;
  int          height;
  SDL_Texture *texture;
} render_buffer_t;

static_assert(sizeof(pixel_t) == 4, "pixel_t should be 4 bytes long");

int init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return -1;
  }

  return 0;
}

SDL_Window *create_window() {
  SDL_Window *window = SDL_CreateWindow(
      "Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
      SDL_WINDOW_RESIZABLE);

  if (window == NULL)
    printf("SDL_CreateWindow failed: %s\n", SDL_GetError());

  return window;
}

SDL_Renderer *create_renderer(SDL_Window *window) {
  if (window == NULL)
    return NULL;

  SDL_Renderer *const renderer = SDL_CreateRenderer(
      window, -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == NULL)
    printf("SDL_CreateSoftwareRenderer failed: %s\n", SDL_GetError());

  SDL_RendererInfo info;
  if (SDL_GetRendererInfo(renderer, &info) < 0)
    printf("SDL_GetRendererInfo failed: %s\n", SDL_GetError());
  else
    printf("SDL renderer: %s\n", info.name);

  return renderer;
}

render_buffer_t update_size(SDL_Renderer   *renderer,
                            render_buffer_t buffer) {
  if (renderer == NULL)
    return buffer;

  SDL_Rect rect;
  SDL_RenderGetViewport(renderer, &rect);

  if (buffer.texture != NULL && buffer.width == rect.w &&
      buffer.height == rect.h)
    return buffer;

  if (buffer.texture != NULL)
    SDL_DestroyTexture(buffer.texture);

  buffer.width  = rect.w;
  buffer.height = rect.h;

  buffer.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     buffer.width, buffer.height);

  if (buffer.texture == NULL)
    printf("SDL_CreateTexture failed: %s\n", SDL_GetError());

  return buffer;
}

void frame(SDL_Renderer *renderer, render_buffer_t buffer,
           int64_t time_elapsed) {
  static int64_t t = 0;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  if (buffer.texture != NULL) {
    pixel_t *pixels = NULL;
    int      pitch  = 0;

    if (SDL_LockTexture(buffer.texture, NULL, (void **) &pixels,
                        &pitch) < 0)
      printf("SDL_LockTexture failed: %s\n", SDL_GetError());
    else {
      pitch /= sizeof(pixel_t);

      t += time_elapsed;
      int64_t k = t / 10;

      for (int j = 0; j < buffer.height; j++)
        for (int i = 0, n = j * buffer.width; i < buffer.width;
             i++, n++) {
          pixels[n].r = k + i;
          pixels[n].g = k + j;
          pixels[n].b = k + i + j;
        }

      SDL_UnlockTexture(buffer.texture);
    }

    SDL_RenderCopy(renderer, buffer.texture, NULL, NULL);
  }

  SDL_RenderPresent(renderer);
}

void event_loop(SDL_Renderer *renderer) {
  if (renderer == NULL)
    return;

  render_buffer_t buffer;
  memset(&buffer, 0, sizeof buffer);

  buffer = update_size(renderer, buffer);

  struct timespec time_0;
  timespec_get(&time_0, TIME_UTC);

  int64_t time_extra = 0;

  for (int done = 0; !done;) {
    SDL_Event event;
    while (SDL_PollEvent(&event) == 1)
      if (event.type == SDL_QUIT)
        done = 1;

    buffer = update_size(renderer, buffer);

    struct timespec time_1;
    timespec_get(&time_1, TIME_UTC);

    int64_t time_elapsed = (time_1.tv_sec - time_0.tv_sec) *
                               1000000000 +
                           (time_1.tv_nsec - time_0.tv_nsec);

    int64_t time_elapsed_ms = (time_elapsed + time_extra) / 1000000;
    time_extra              = (time_elapsed + time_extra) % 1000000;

    frame(renderer, buffer, time_elapsed_ms);

    time_0 = time_1;
  }
}

void run() {
  if (init() != 0)
    return;

  SDL_Window   *window   = create_window();
  SDL_Renderer *renderer = create_renderer(window);

  event_loop(renderer);

  if (renderer != NULL)
    SDL_DestroyRenderer(renderer);
  if (window != NULL)
    SDL_DestroyWindow(window);

  SDL_Quit();
}

int main(int argc, char **argv) {
  run();
  return 0;
}

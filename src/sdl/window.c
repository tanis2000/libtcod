
#include <libtcod_sdl_window.h>

#include <stdlib.h>

#include <SDL.h>

#include <libtcod_sdl_render.h>
#include <libtcod_int.h>

int TCOD_sdl_window_flush(struct TCOD_ManagedSDLWindow *window,
                          struct TCOD_Tileset *tileset,
                          TCOD_console_data_t *console) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(window->renderer);
  TCOD_sdl_render_console(window->renderer, tileset, console, NULL);
  SDL_RenderPresent(window->renderer);
  return 0;
}

struct TCOD_ManagedSDLWindow *TCOD_sdl_window_new(int width, int height) {
  struct TCOD_ManagedSDLWindow *window;
  if (SDL_Init(SDL_INIT_VIDEO)) {
    return NULL;
  }
  window = calloc(1, sizeof(*window));
  if (SDL_CreateWindowAndRenderer(width, height,
                                  SDL_WINDOW_RESIZABLE,
                                  &window->window, &window->renderer)) {
    TCOD_sdl_window_delete(window);
    return NULL;
  }
  return window;
}
/**
 *  \brief Brief
 *
 *  \param window Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
int TCOD_sdl_window_delete(struct TCOD_ManagedSDLWindow *window) {
  if (!window) { return -1; }
  if (window->renderer) { SDL_DestroyRenderer(window->renderer); }
  if (window->window) { SDL_DestroyWindow(window->window); }
  free(window);
  return 0;
}

struct TCOD_Backend_SDL_ {
  struct TCOD_Backend_ callbacks;
  struct SDL_Window *window;
  struct SDL_Renderer *renderer;
  struct TCOD_Tileset *tileset;
};

static int sdl_backend_render(struct TCOD_Backend_SDL_ *backend,
                      TCOD_console_data_t *console) {
  TCOD_sdl_render_console(backend->renderer, backend->tileset, console, NULL);
  return 0;
}

static int sdl_backend_render_and_present(struct TCOD_Backend_SDL_ *backend,
                                          TCOD_console_data_t *console) {
  SDL_SetRenderDrawColor(backend->renderer, 0, 0, 0, 255);
  SDL_RenderClear(backend->renderer);
  sdl_backend_render(backend, console);
  SDL_RenderPresent(backend->renderer);
  return 0;
}

static int sdl_backend_destroy(struct TCOD_Backend_SDL_ *backend) {
  if (backend->tileset) { TCOD_tileset_delete(backend->tileset); }
  return 0;
}

struct TCOD_Backend_* TCOD_get_sdl_backend_(int w, int h, bool fullscreen) {
  struct TCOD_Backend_SDL_ *backend;
  backend = calloc(1, sizeof(*backend));
  if (!backend) { return NULL; }
  backend->callbacks.render = sdl_backend_render;
  backend->callbacks.render_and_present = sdl_backend_render_and_present;
  backend->callbacks.destroy = sdl_backend_destroy;
  backend->window = SDL_CreateWindow(TCOD_ctx.window_title,
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     w * TCOD_ctx.font_width,
                                     h * TCOD_ctx.font_height,
                                     SDL_WINDOW_RESIZABLE);
  backend->renderer = SDL_CreateRenderer(backend->window, -1,
                                         SDL_RENDERER_SOFTWARE);
  backend->tileset = TCOD_tileset_from_tcod_context_();
  /* set the libtcod_int.h global variables */
  window = backend->window;
  renderer = backend->renderer;
  return (struct TCOD_Backend_*)backend;
}
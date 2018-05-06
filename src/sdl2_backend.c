
#include <libtcod_sdl2_backend.h>

#include <stdlib.h>

#include <SDL.h>

#include <console.h>
#include <libtcod_sdl2_render.h>
#include <libtcod_int.h>

int TCOD_sdl_window_flush(struct TCOD_ManagedSDLWindow *window,
                          struct TCOD_Tileset *tileset,
                          TCOD_console_t console) {
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
  struct SDL_Texture *buffer;
};

static int sdl_backend_render(struct TCOD_Backend_SDL_ *backend,
                              TCOD_console_t console) {
  TCOD_sdl_render_console(backend->renderer, backend->tileset, console, NULL);
  return 0;
}

static SDL_Texture* sdl_get_buffer_texture(struct TCOD_Backend_SDL_ *backend,
                                           TCOD_console_t console) {
  int wanted_width =
      backend->tileset->tile_width * TCOD_console_get_width(console);
  int wanted_height =
      backend->tileset->tile_height * TCOD_console_get_height(console);
  if (backend->buffer) {
    int current_width;
    int current_height;
    SDL_QueryTexture(backend->buffer, NULL, NULL,
                     &current_width, &current_height);
    if (current_width != wanted_width || current_height != wanted_height) {
      SDL_DestroyTexture(backend->buffer);
      backend->buffer = NULL;
    }
  }
  if (!backend->buffer) {
    backend->buffer = SDL_CreateTexture(
        backend->renderer, 0, SDL_TEXTUREACCESS_TARGET,
        backend->tileset->tile_width * TCOD_console_get_width(console),
        backend->tileset->tile_height * TCOD_console_get_height(console));
  }
  return backend->buffer;
  }

static int sdl_backend_render_and_present(struct TCOD_Backend_SDL_ *backend,
                                          TCOD_console_t console) {
  /* update back buffer from console */
  sdl_get_buffer_texture(backend, console);
  SDL_SetRenderTarget(backend->renderer, backend->buffer);
  sdl_backend_render(backend, console);
  /* draw buffer to screen */
  SDL_SetRenderTarget(backend->renderer, NULL);
  SDL_SetRenderDrawColor(backend->renderer, 0, 0, 0, 255);
  SDL_RenderClear(backend->renderer);
  SDL_RenderCopy(backend->renderer, backend->buffer, NULL, NULL);
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
                                         SDL_RENDERER_TARGETTEXTURE);
  backend->tileset = TCOD_tileset_from_tcod_context_();
  /* set the libtcod_int.h global variables */
  window = backend->window;
  renderer = backend->renderer;
  return (struct TCOD_Backend_*)backend;
}
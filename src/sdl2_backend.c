
#include <libtcod_sdl2_backend.h>

#include <stdlib.h>

#include <SDL.h>

#include <console.h>
#include <libtcod_sdl2_render.h>
#include <libtcod_int.h>
struct TCOD_Backend_SDL_ {
  struct TCOD_Backend_ *callbacks;
  struct SDL_Window *window;
  struct SDL_Renderer *renderer;
  struct TCOD_Tileset *tileset;
  struct SDL_Texture *buffer;
  struct TCOD_Console *cache_console;
};
/**
 *  Delete the console cache.
 *  This will force a refresh of the back buffer as a side-effect.
 */
static int TCOD_sdl2_backend_free_console_(
    struct TCOD_Backend_SDL_ *backend) {
  if (backend->cache_console) {
    TCOD_console_delete(backend->cache_console);
    backend->cache_console = NULL;
  }
  return 0;
}
/**
 *  Delete the back buffer texture.
 */
static int TCOD_sdl2_backend_free_back_buffer_(
    struct TCOD_Backend_SDL_ *backend) {
  if (backend->buffer) {
    SDL_DestroyTexture(backend->buffer);
    backend->buffer = NULL;
  }
  /* The console cache depends on this buffer and also needs to be freed. */
  return TCOD_sdl2_backend_free_console_(backend);
}
/**
 *  This back-end receives SDL events to this callback during its lifetime.
 */
static int TCOD_sdl2_backend_on_event_(
    struct TCOD_Backend_SDL_ *backend,
    SDL_Event* event) {
  switch (event->type) {
    /* When texture targets are reset a full update of the back buffer is
       needed. */
    case SDL_RENDER_TARGETS_RESET:
      /* Just delete cache_console, it will be reinitialized at the end of the
         next frame. */
      TCOD_sdl2_backend_free_console_(backend);
      break;
#if SDL_VERSION_ATLEAST(2, 0, 4)
    case SDL_RENDER_DEVICE_RESET:
      /* All textures need to be recreated. */
      TCOD_sdl2_backend_free_back_buffer_(backend);
      break;
#endif
    case SDL_APP_LOWMEMORY:
      TCOD_sdl2_backend_free_console_(backend);
      TCOD_sdl2_backend_free_back_buffer_(backend);
    default:
      break;
  }
  return 0;
}

static int sdl_backend_render(struct TCOD_Backend_SDL_ *backend,
                              struct TCOD_Console *console) {
  TCOD_sdl_render_console(backend->renderer, backend->tileset, console,
                          &backend->cache_console);
  return 0;
}

static SDL_Texture* sdl_get_buffer_texture(struct TCOD_Backend_SDL_ *backend,
                                           struct TCOD_Console *console) {
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

static int sdl_backend_present(struct TCOD_Backend_SDL_ *backend,
                               struct TCOD_Console *console) {
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
  TCOD_sdl2_backend_free_console_(backend);
  TCOD_sdl2_backend_free_back_buffer_(backend);
  if (backend->tileset) { TCOD_tileset_delete(backend->tileset); }
  free(backend->callbacks);
  SDL_DelEventWatch(TCOD_sdl2_backend_on_event_, backend);
  return 0;
}

struct TCOD_Backend_* TCOD_get_sdl_backend_(int w, int h, bool fullscreen) {
  struct TCOD_Backend_SDL_ *backend;
  backend = calloc(1, sizeof(*backend));
  if (!backend) { return NULL; }
  backend->callbacks = TCOD_new_custom_backend_();
  if (!backend->callbacks) { sdl_backend_destroy(backend); return NULL; }
  backend->callbacks->userdata = backend;
  backend->callbacks->present = sdl_backend_present;
  backend->callbacks->destroy = sdl_backend_destroy;
  backend->window = SDL_CreateWindow(TCOD_ctx.window_title,
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     w * TCOD_ctx.font_width,
                                     h * TCOD_ctx.font_height,
                                     SDL_WINDOW_RESIZABLE);
  backend->renderer = SDL_CreateRenderer(backend->window, -1,
                                         SDL_RENDERER_TARGETTEXTURE);
  backend->tileset = TCOD_tileset_from_tcod_context_();
  SDL_AddEventWatch(TCOD_sdl2_backend_on_event_, backend);
  /* set the libtcod_int.h global variables */
  window = backend->window;
  renderer = backend->renderer;
  return backend->callbacks;
}
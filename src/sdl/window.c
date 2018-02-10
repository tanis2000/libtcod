
#include <libtcod_sdl_window.h>

#include <stdlib.h>

#include <SDL.h>

#include <libtcod_sdl_render.h>
#include <libtcod_int.h>

int TCOD_sdl_window_flush(struct TCOD_ManagedSDLWindow *window,
                          struct TCOD_Tileset *tileset,
                          const TCOD_console_data_t *console) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(window->renderer);
  TCOD_sdl_render_console(window->renderer, tileset, console);
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

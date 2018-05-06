
#ifndef LIBTCOD_SDL_WINDOW_H_
#define LIBTCOD_SDL_WINDOW_H_

#include "libtcod_portability.h"
#include "libtcod_sdl2_tileset.h"
#include "console.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

struct TCOD_ManagedSDLWindow {
  struct SDL_Window *window;
  struct SDL_Renderer *renderer;
};

TCODLIB_API
struct TCOD_ManagedSDLWindow* TCOD_sdl_window_new(int width, int height);

TCODLIB_API
int TCOD_sdl_window_delete(struct TCOD_ManagedSDLWindow *window);

TCODLIB_API
int TCOD_sdl_window_flush(struct TCOD_ManagedSDLWindow *window,
                          struct TCOD_Tileset *tileset,
                          const TCOD_console_t console);

struct TCOD_Backend_ {
  int (*render)(void*, TCOD_console_t);
  int (*render_and_present)(void*, TCOD_console_t);
  int (*destroy)(void*);
};

struct TCOD_Backend_* TCOD_get_sdl_backend_(int w, int h, bool fullscreen);

#endif /* LIBTCOD_SDL_WINDOW_H_ */

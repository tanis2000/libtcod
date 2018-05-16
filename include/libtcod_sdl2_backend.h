
#ifndef LIBTCOD_SDL_WINDOW_H_
#define LIBTCOD_SDL_WINDOW_H_

#include "libtcod_portability.h"
#include "libtcod_backend.h"
#include "libtcod_sdl2_tileset.h"
#include "console.h"

struct TCOD_Backend_* TCOD_get_sdl_backend_(int w, int h, bool fullscreen);

#endif /* LIBTCOD_SDL_WINDOW_H_ */

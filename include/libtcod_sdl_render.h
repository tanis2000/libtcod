
#ifndef LIBTCOD_SDL_RENDER_H_
#define LIBTCOD_SDL_RENDER_H_

#include "libtcod_portability.h"
#include "libtcod_sdl_tileset.h"
#include "console.h"

struct SDL_Renderer;

TCODLIB_API
int TCOD_sdl_render_console(struct SDL_Renderer *renderer,
                            struct TCOD_Tileset *tileset,
                            const TCOD_console_t console);

#endif /* LIBTCOD_SDL_RENDER_H_ */

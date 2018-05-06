
#ifndef LIBTCOD_SDL_TILESET_H_
#define LIBTCOD_SDL_TILESET_H_

#include "libtcod_portability.h"

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Surface;

struct TCOD_Tileset {
  int magic_version_number;
  int tile_width, tile_height;
  int columns, rows;
  int allocated_tiles, assigned_tiles;
  int character_count;
  int *map_charcode_to_tile;
  struct SDL_Surface *surface;
  struct SDL_Texture *sdl_texture_cache;
};

TCODLIB_API
struct TCOD_Tileset* TCOD_tileset_from_file(const char *filename,
                                            int tile_width,
                                            int tile_height);

TCODLIB_API
int TCOD_tileset_get_tile_for_charcode(const struct TCOD_Tileset *tileset,
                                       int ch);

TCODLIB_API
int TCOD_tileset_delete(struct TCOD_Tileset *tileset);

struct TCOD_Tileset* TCOD_tileset_from_tcod_context_();

struct SDL_Texture* TCOD_tileset_get_sdl_texture_(
    struct TCOD_Tileset *tileset,
    struct SDL_Renderer *renderer);

#endif /* LIBTCOD_SDL_TILESET_H_ */

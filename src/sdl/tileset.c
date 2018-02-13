
#include <libtcod_sdl_tileset.h>

#include <stdlib.h>
#include <string.h>

#include "../png/lodepng.h"
#include <SDL.h>

#include <libtcod_int.h>

/* Returns 1 if the tile in this rect can be converted into an alpha-only
   tile. */
static int CanNormalizeTile(struct SDL_Surface *surface,
                            struct SDL_Rect *rect) {
  int x, y;
  for (y = rect->y; y < rect->y + rect->h; ++y) {
    for (x = rect->x; x < rect->x + rect->w; ++x) {
      /* surface->pixels should be 0xAABBGGRR */
      unsigned int *abgr = ((unsigned int*)surface->pixels);
      int i = y*surface->w+x;
      if ((abgr[i] & 0xff000000) != 0xff000000) { /* test alpha */
        return 0; /* this tile already has alpha values */
      }
      if ((abgr[i] & 0xff) != (abgr[i] & 0xff00 >> 8) ||  /* test r,g,b */
          (abgr[i] & 0xff) != (abgr[i] & 0xff0000 >> 16)) {
        return 0; /* this tile is not monochrome */
      }
    }
  }
  return 1;
};
/* Converts the monochrome tile at rect into an alpha-only tile, this checks
   CanNormalizeTile first before converting. */
static int NormalizeTile(struct SDL_Surface *surface,
                         struct SDL_Rect *rect) {
  int x, y;
  if (!CanNormalizeTile(surface, rect)) { return 0; }
  for (y = rect->y; y < rect->y + rect->h; ++y) {
    for (x = rect->x; x < rect->x + rect->w; ++x) {
      /* surface->pixels should be 0xAABBGGRR */
      unsigned int *abgr = ((unsigned int*)surface->pixels);
      int i = y*surface->w+x;
      /* move grey-scale channel to the alpha channel */
      abgr[i] &= 0x00ffffff;
      abgr[i] |= (abgr[i] & 0xff) << 24;
      /* white out the color space */
      abgr[i] |= 0x00ffffff;
    }
  }
  return 0;
};
/* Converts all monochrome tiles into pure white tiles with an alpha channel */
static int NormalizeAllTiles(struct TCOD_Tileset *tileset) {
  int i;
  for (i = 0; i < tileset->assigned_tiles; ++i) {
    struct SDL_Rect tile_rect = {
        i % tileset->columns * tileset->tile_width,
        i / tileset->columns * tileset->tile_height,
        tileset->tile_width,
        tileset->tile_height};
    NormalizeTile(tileset->surface, &tile_rect);
  }
  return 0;
};
/* Loads and formats an SDL_Surface from a font file.
   For now this only handles PNG files. */
static struct SDL_Surface* LoadFontFile(const char *filename) {
  unsigned char *png_pixels;
  unsigned png_width;
  unsigned png_height;
  struct SDL_Surface *surface;
  if (lodepng_decode32_file(&png_pixels, &png_width, &png_height, filename)) {
    return NULL;
  }
  /* lodepng returns RRGGBBAA bytes, sdl sees this as an 0xAABBGGRR int32 */
  surface = SDL_CreateRGBSurface(0, png_width, png_height, 32,
                                 0xff, 0xff00, 0xff0000, 0xff000000);
  if (!surface) {
    free(png_pixels);
    return NULL;
  }
  memcpy(surface->pixels, png_pixels, png_height * png_width * 4);
  free(png_pixels);
  return surface;
}
/* Allocate the map_charcode_to_tile buffer and return the status. */
static int AllocateDefaultMapping(struct TCOD_Tileset *tileset) {
  int i;
  size_t new_size;
  if (tileset->character_count >= tileset->assigned_tiles ) {
    return 0;
  }
  if (tileset->map_charcode_to_tile) {
    free(tileset->map_charcode_to_tile);
  }
  tileset->character_count = tileset->assigned_tiles;
  new_size = sizeof(*tileset->map_charcode_to_tile) * tileset->character_count;
  tileset->map_charcode_to_tile = malloc(new_size);
  if (tileset->map_charcode_to_tile == NULL) {
    return -1;
  }
  for(i=0; i<tileset->character_count; ++i) {
    tileset->map_charcode_to_tile[i] = i;
  }
  return 0;
}
/**
 *  \brief Brief
 *
 *  \param filename Parameter_Description
 *  \param tile_width Parameter_Description
 *  \param tile_height Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
struct TCOD_Tileset* TCOD_tileset_from_file(const char *filename,
                                            int tile_width,
                                            int tile_height) {
  struct TCOD_Tileset *tileset;
  tileset = calloc(1, sizeof(*tileset));
  tileset->surface = LoadFontFile(filename);
  if (tileset->surface == NULL) { TCOD_tileset_delete(tileset); return NULL; }
  tileset->tile_width = tile_width;
  tileset->tile_height = tile_height;
  /* prevent division by zero */
  if (tile_width == 0 || tile_height == 0 ) {
    TCOD_tileset_delete(tileset);
    return NULL;
  }
  tileset->columns = tileset->surface->w / tile_width;
  tileset->rows = tileset->surface->h / tile_height;
  tileset->assigned_tiles = tileset->columns * tileset->rows;
  tileset->allocated_tiles = tileset->assigned_tiles;
  if (AllocateDefaultMapping(tileset) < 0) {
    TCOD_tileset_delete(tileset);
    return NULL;
  }
  NormalizeAllTiles(tileset);
  return tileset;
};
/**
 *  \brief Brief
 *
 *  \param tileset Parameter_Description
 *  \param ch Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
int TCOD_tileset_get_tile_for_charcode(const struct TCOD_Tileset *tileset,
                                       int ch) {
  if (ch >= tileset->character_count) { return 0; }
  return tileset->map_charcode_to_tile[ch];
};
/**
 *  \brief Brief
 *
 *  \param tileset Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
int TCOD_tileset_delete(struct TCOD_Tileset *tileset) {
  if (tileset->map_charcode_to_tile) { free(tileset->map_charcode_to_tile); }
  if (tileset->surface) { SDL_FreeSurface(tileset->surface); }
  free(tileset);
  return 0;
}
/**
 *  If tileset->map_charcode_to_tile is less than needed_size then it will
 *  be resized to needed_size or larger.  The new array members are zeroed.
 */
static int RaiseMappingSizeIfNeeded(struct TCOD_Tileset *tileset,
                                    int needed_size) {
  int *temp_map;
  int i;
  if (needed_size <= tileset->character_count) { return 0; }
  temp_map = realloc(tileset->map_charcode_to_tile,
                     sizeof(*tileset->map_charcode_to_tile) * needed_size);
  if (!temp_map) { return -1; }
  tileset->map_charcode_to_tile = temp_map;
  for (i = tileset->character_count; i < needed_size; ++i) {
    tileset->map_charcode_to_tile[i] = 0;
  }
  tileset->character_count = needed_size;
  return 0;
}
struct TCOD_Tileset* TCOD_tileset_from_tcod_context_() {
  struct TCOD_Tileset *tileset = TCOD_tileset_from_file(TCOD_ctx.font_file,
                                                        TCOD_ctx.font_width,
                                                        TCOD_ctx.font_height);
  int i;
  if (!tileset) { return NULL; }
  if (RaiseMappingSizeIfNeeded(tileset, TCOD_ctx.max_font_chars) < 0) {
    TCOD_tileset_delete(tileset);
    return NULL;
  }
  for (i = 0; i < TCOD_ctx.max_font_chars; ++i) {
    tileset->map_charcode_to_tile[i] = TCOD_ctx.ascii_to_tcod[i];
  }
  return tileset;
}

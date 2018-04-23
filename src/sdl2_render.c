
#include <libtcod_sdl2_render.h>

#include <SDL.h>

#include <libtcod_int.h>

static SDL_Texture* NewTexture(struct SDL_Renderer *renderer,
                               struct TCOD_Tileset *tileset) {
  return SDL_CreateTextureFromSurface(renderer, tileset->surface);
}


/* Return the renderer drawing region */
static struct SDL_Rect GetDestinationRect(struct TCOD_Tileset *tileset,
                                          int console_x,
                                          int console_y) {
  struct SDL_Rect rect = {
      console_x * tileset->tile_width,
      console_y * tileset->tile_height,
      tileset->tile_width,
      tileset->tile_height
  };
  return rect;
}
/* Return the rect for tile_id from tileset */
static struct SDL_Rect GetTileRect(struct TCOD_Tileset *tileset,
                                   int tile_id) {
  struct SDL_Rect rect = {
      tile_id % tileset->columns * tileset->tile_width,
      tile_id / tileset->columns * tileset->tile_height,
      tileset->tile_width,
      tileset->tile_height
  };
  return rect;
}

static int IsTileDirty(TCOD_console_data_t *console,
                       TCOD_console_data_t *cache,
                       int console_i) {
  int ch = console->ch_array[console_i];
  TCOD_color_t bg = console->bg_array[console_i];
  TCOD_color_t fg = console->fg_array[console_i];
  if (cache) {
    int cache_ch = cache->ch_array[console_i];
    TCOD_color_t cache_bg = cache->bg_array[console_i];
    TCOD_color_t cache_fg = cache->fg_array[console_i];
    if (ch == cache_ch &&
        bg.r == cache_bg.r && bg.g == cache_bg.g && bg.b == cache_bg.b &&
        fg.r == cache_fg.r && fg.g == cache_fg.g && fg.b == cache_fg.b) {
      return 0;
    }
  }
  return 1;
}

static void RenderTile(struct SDL_Renderer *renderer,
                       struct TCOD_Tileset *tileset,
                       TCOD_console_data_t *console,
                       TCOD_console_data_t *cache,
                       struct SDL_Texture *tile_texture,
                       int console_x, int console_y) {
  int console_i = console_y*console->w+console_x;
  int ch = console->ch_array[console_i];
  int tile_id = TCOD_tileset_get_tile_for_charcode(tileset, ch);
  struct SDL_Rect dest_rect = GetDestinationRect(tileset,
                                                 console_x, console_y);
  struct SDL_Rect tex_rect = GetTileRect(tileset, tile_id);
  TCOD_color_t bg = console->bg_array[console_i];
  TCOD_color_t fg = console->fg_array[console_i];
  if (!IsTileDirty(console, cache, console_i)) { return; }
  /* fill in the background color */
  SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 255);
  SDL_RenderFillRect(renderer, &dest_rect);
  /* skip invisible glyphs */
  if (bg.r == fg.r && bg.g == fg.g && bg.b == fg.b) { return; }
  if (ch == 0 || ch == 0x20) { return; }
  /* draw the foreground glyph */
  SDL_SetTextureColorMod(tile_texture, fg.r, fg.g, fg.b);
  SDL_RenderCopy(renderer, tile_texture, &tex_rect, &dest_rect);
  return;
}

int TCOD_sdl_render_console(struct SDL_Renderer *renderer,
                            struct TCOD_Tileset *tileset,
                            TCOD_console_data_t *console,
                            TCOD_console_data_t **cache_console) {
  int console_x, console_y; /* console coordinate */
  TCOD_console_data_t *cache=NULL;
  struct SDL_Texture *texture = NewTexture(renderer, tileset);
  if (!texture) { return -1; }
  if (cache_console) {
    cache = *cache_console;
  }
  /* renderer will fill in all rects without blending */
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  /* texture will blend using the alpha channel */
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(texture, 255);
  for (console_y = 0; console_y < console->h; ++console_y) {
    for (console_x = 0; console_x < console->w; ++console_x) {
      RenderTile(renderer, tileset, console, cache, texture,
                 console_x, console_y);
    }
  }
  SDL_DestroyTexture(texture);
  return 0;
}
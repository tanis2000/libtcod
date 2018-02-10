
#include <libtcod_sdl_render.h>

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

int TCOD_sdl_render_console(struct SDL_Renderer *renderer,
                            struct TCOD_Tileset *tileset,
                            const TCOD_console_data_t *console) {
  int console_x, console_y; /* console coordinate */
  struct SDL_Texture *texture = NewTexture(renderer, tileset);
  if (!texture) { return -1; }
  /* renderer will fill in all rects without blending */
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  /* texture will blend using the alpha channel */
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(texture, 255);
  for (console_y = 0; console_y < console->h; ++console_y) {
    for (console_x = 0; console_x < console->w; ++console_x) {
      int i = console_y*console->w+console_x;
      int ch = console->ch_array[i];
      int tile_id = TCOD_tileset_get_tile_for_charcode(tileset, ch);
      struct SDL_Rect dest_rect = GetDestinationRect(tileset,
                                                     console_x, console_y);
      struct SDL_Rect tex_rect = GetTileRect(tileset, tile_id);
      TCOD_color_t bg = console->bg_array[i];
      TCOD_color_t fg = console->fg_array[i];
      /* fill in the background color */
      SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 255);
      SDL_RenderFillRect(renderer, &dest_rect);
      /* skip invisible glyphs */
      if (bg.r == fg.r && bg.g == fg.g && bg.b == fg.b) { continue; }
      if (ch == 0 || ch == 0x20) { continue; }
      /* draw the foreground glyph */
      SDL_SetTextureColorMod(texture, fg.r, fg.g, fg.b);
      SDL_RenderCopy(renderer, texture, &tex_rect, &dest_rect);
    }
  }
  SDL_DestroyTexture(texture);
  return 0;
}
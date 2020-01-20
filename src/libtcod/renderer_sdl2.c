/* BSD 3-Clause License
 *
 * Copyright © 2008-2020, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "renderer_sdl2.h"

#include "stdlib.h"

#include <SDL.h>

// ----------------------------------------------------------------------------
// SDL2 Atlas
static TCOD_TilesetAtlasSDL2* TCOD_tilset_get_sdl2_atlas(
    struct TCOD_RendererSDL2* renderer, struct TCOD_Tileset* tileset)
{
  if (!renderer || !tileset) { return NULL; }
  for (struct TCOD_TilesetObserver* it = tileset->observer; it; it = it->next) {
    if (it->type != TCOD_RENDERER_SDL2) { continue; }
    static TCOD_TilesetAtlasSDL2* atlas = it->userdata;
    if (atlas->renderer != renderer) { continue; }
    return atlas;
  }
  static TCOD_TilesetAtlasSDL2* atlas = calloc(sizeof(*atlas), 1);
  struct TCOD_TilesetObserver* observer = calloc(sizeof(*observer), 1);
  if (!atlas || !observer) { free(atlas); free(observer); return NULL; }
  atlas->renderer = renderer;
  atlas->tileset = tileset;
  observer->type = TCOD_RENDERER_SDL2;
  observer->userdata = atlas;
  observer->next = tileset->observer;
  tileset->observer = observer;
  return atlas;
}


static int TCOD_console_render_sdl2(
    struct SDL_Renderer* renderer,
    const struct TCOD_Console* console,
    struct SDL_Texture* out,
    struct TCOD_Tileset* tileset,
    struct TCOD_Console** cache)
{
  if (!renderer || !console || !out) { return -1; }
  if (!tileset) { return -1; }

}
// ----------------------------------------------------------------------------
// SDL2 Rendering
static void sdl2_destructor(struct TCOD_RendererSDL2* renderer)
{
  if (!renderer) { return; }
  TCOD_console_delete(renderer->cache_console);
  SDL_DestroyTexture(renderer->cache_texture);
  SDL_DestroyRenderer(renderer->sdl_renderer);
  SDL_DestroyWindow(renderer->sdl_window);
  free(renderer);
}

static int sdl2_present(
    struct TCOD_RendererSDL2* renderer, const struct TCOD_Console* console)
{
  ;
}

struct TCOD_Renderer* TCOD_renderer_init_sdl2_from(
    struct SDL_Window* sdl_window,
    struct SDL_Renderer* sdl_renderer)
{
  if (!sdl_window || !sdl_renderer) { return NULL; }
  struct TCOD_Renderer* renderer = TCOD_renderer_init_custom();
  if (!renderer) { return NULL; }
  struct TCOD_RendererSDL2* sdl2_data = calloc(sizeof(*sdl2_data), 1);
  if (!sdl2_data) { TCOD_renderer_delete(renderer); return NULL; }
  sdl2_data->sdl_window = sdl_window;
  sdl2_data->sdl_renderer = sdl_renderer;

  renderer->type = TCOD_RENDERER_SDL2;
  renderer->userdata = sdl2_data;
  renderer->destructor = sdl2_destructor;
  renderer->present = sdl2_present;
  return renderer;
}


struct TCOD_Renderer* TCOD_renderer_init_sdl2(
    int pixel_width,
    int pixel_height,
    char* title,
    int window_flags,
    int renderer_flags)
{

}


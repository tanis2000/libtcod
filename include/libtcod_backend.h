
#ifndef LIBTCOD_BACKEND_H_
#define LIBTCOD_BACKEND_H_

#include "libtcod_portability.h"
#include "libtcod_backend.h"
#include "console.h"

struct TCOD_Backend_ {
  int (*render)(void*, TCOD_console_t);
  int (*render_and_present)(void*, TCOD_console_t);
  int (*destroy)(void*);
};

#endif /* LIBTCOD_BACKEND_H_ */

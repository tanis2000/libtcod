
#ifndef LIBTCOD_BACKEND_H_
#define LIBTCOD_BACKEND_H_

#include "libtcod_portability.h"
#include "libtcod_backend.h"
#include "console.h"

struct TCOD_Backend_ {
  void *userdata;
  int (*destroy)(void*);
  int (*present)(void*, TCOD_console_t);
};

extern struct TCOD_Backend_ *TCOD_active_backend_;

struct TCOD_Backend_* TCOD_new_custom_backend_(void);
int TCOD_delete_custom_backend_(struct TCOD_Backend_ *backend);

#endif /* LIBTCOD_BACKEND_H_ */

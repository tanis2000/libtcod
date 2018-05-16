
#include <libtcod_backend.h>

#include <stdlib.h>

struct TCOD_Backend_ *TCOD_active_backend_ = NULL;
/**
 *  Create a new custom backend.
 */
struct TCOD_Backend_* TCOD_new_custom_backend_(void) {
  struct TCOD_Backend_ *backend = calloc(1, sizeof(*backend));
  if (!backend) { return NULL; }
  backend->userdata = &backend;
  return backend;
}

int TCOD_delete_custom_backend_(struct TCOD_Backend_ *backend) {
  if (backend->destroy) {
    return backend->destroy(backend->userdata);
  } else {
    free(backend);
    return 0;
  }
}

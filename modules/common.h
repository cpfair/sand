#include "colour_bw_remap.h"

#define GPATH_DEFINE(NAME, POINT_COUNT, POINTS) \
  static const GPathInfo NAME ## _path_info = {\
    .num_points = POINT_COUNT,\
    .points = POINTS\
  };\


#define GPATH_DRAW(CTX, NAME, X, Y) {\
  static GPath *NAME ## _path = NULL;\
  if (!NAME ## _path) {NAME ## _path = gpath_create(&NAME ## _path_info);}\
  gpath_move_to(NAME ## _path, GPoint(X, Y));\
  gpath_draw_outline(CTX, NAME ## _path);\
  gpath_draw_filled(CTX, NAME ## _path);\
}

#define LOG_GRECT(key, grect) APP_LOG(APP_LOG_LEVEL_DEBUG, key "x %d y %d w %d h %d", grect.origin.x, grect.origin.y, grect.size.w, grect.size.h)

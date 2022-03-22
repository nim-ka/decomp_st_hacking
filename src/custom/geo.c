#include <ultra64.h>
#include "sm64.h"
#include "geo_commands.h"

#include "game/level_geo.h"
#include "game/geo_misc.h"
#include "game/camera.h"
#include "game/moving_texture.h"
#include "game/screen_transition.h"
#include "game/paintings.h"

#include "make_const_nonconst.h"

#include "levels/bowser_3/header.h"
#include "levels/bob/header.h"

// 0x0E000398
const GeoLayout bowser_3_geo_000398_hack[] = {
   GEO_NODE_SCREEN_AREA(10, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
   GEO_OPEN_NODE(),
      GEO_ZBUFFER(0),
      GEO_OPEN_NODE(),
         GEO_NODE_ORTHO(100),
         GEO_OPEN_NODE(),
            GEO_BACKGROUND(BACKGROUND_PURPLE_SKY, geo_skybox_main),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_ZBUFFER(1),
      GEO_OPEN_NODE(),
         GEO_CAMERA_FRUSTUM_WITH_FUNC(45, 100, 12800, geo_camera_fov),
         GEO_OPEN_NODE(),
            GEO_CAMERA(11, 0, 2000, 6000, 0, 0, 0, geo_camera_main),
            GEO_OPEN_NODE(),
               GEO_DISPLAY_LIST(LAYER_OPAQUE, bowser_3_seg7_dl_070046B0),
               GEO_RENDER_OBJ(),
               GEO_ASM(0, geo_envfx_main),
            GEO_CLOSE_NODE(),
         GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x0E000488
const GeoLayout bob_geo_000488_hack[] = {
   GEO_NODE_SCREEN_AREA(10, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
   GEO_OPEN_NODE(),
      GEO_ZBUFFER(0),
      GEO_OPEN_NODE(),
         GEO_NODE_ORTHO(100),
         GEO_OPEN_NODE(),
            GEO_BACKGROUND(BACKGROUND_OCEAN_SKY, geo_skybox_main),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_ZBUFFER(1),
      GEO_OPEN_NODE(),
         GEO_CAMERA_FRUSTUM_WITH_FUNC(45, 100, 30000, geo_camera_fov),
         GEO_OPEN_NODE(),
            GEO_CAMERA(1, 0, 2000, 6000, 3072, 0, -4608, geo_camera_main),
            GEO_OPEN_NODE(),
               GEO_DISPLAY_LIST(LAYER_OPAQUE, bob_seg7_dl_07004390),
               GEO_DISPLAY_LIST(LAYER_OPAQUE, bob_seg7_dl_07009D80),
               GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, bob_seg7_dl_0700A470),
               GEO_DISPLAY_LIST(LAYER_ALPHA, bob_seg7_dl_0700A920),
               GEO_DISPLAY_LIST(LAYER_OPAQUE, bob_seg7_dl_0700DD18),
               GEO_DISPLAY_LIST(LAYER_OPAQUE, bob_seg7_dl_0700E338),
               GEO_RENDER_OBJ(),
               GEO_ASM(0, geo_envfx_main),
            GEO_CLOSE_NODE(),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_ZBUFFER(0),
      GEO_OPEN_NODE(),
         GEO_ASM(0, geo_cannon_circle_base),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

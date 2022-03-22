#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"
#include "actors/group12.h"
#include "actors/group14.h"

#include "make_const_nonconst.h"
#include "levels/bowser_3/header.h"
#include "levels/bob/header.h"
#include "header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_BOWSER_3_FALLING_PLATFORM_1,  /*pos*/     0,   0,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvFallingBowserPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_3_FALLING_PLATFORM_2,  /*pos*/     0,   0,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00020000, /*beh*/ bhvFallingBowserPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_3_FALLING_PLATFORM_3,  /*pos*/     0,   0,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvFallingBowserPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_3_FALLING_PLATFORM_4,  /*pos*/     0,   0,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00040000, /*beh*/ bhvFallingBowserPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_3_FALLING_PLATFORM_5,  /*pos*/     0,   0,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00050000, /*beh*/ bhvFallingBowserPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_3_FALLING_PLATFORM_6,  /*pos*/     0,   0,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00060000, /*beh*/ bhvFallingBowserPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_3_FALLING_PLATFORM_7,  /*pos*/     0,   0,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00070000, /*beh*/ bhvFallingBowserPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_3_FALLING_PLATFORM_8,  /*pos*/     0,   0,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00080000, /*beh*/ bhvFallingBowserPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_3_FALLING_PLATFORM_9,  /*pos*/     0,   0,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00090000, /*beh*/ bhvFallingBowserPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_3_FALLING_PLATFORM_10, /*pos*/     0,   0,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x000A0000, /*beh*/ bhvFallingBowserPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_BOMB,                  /*pos*/ -2122, 512, -2912, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBowserBomb),
    OBJECT(/*model*/ MODEL_BOWSER_BOMB,                  /*pos*/ -3362, 512,  1121, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBowserBomb),
    OBJECT(/*model*/ MODEL_BOWSER_BOMB,                  /*pos*/     0, 512,  3584, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBowserBomb),
    OBJECT(/*model*/ MODEL_BOWSER_BOMB,                  /*pos*/  3363, 512,  1121, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBowserBomb),
    OBJECT(/*model*/ MODEL_BOWSER_BOMB,                  /*pos*/  2123, 512, -2912, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBowserBomb),
    OBJECT(/*model*/ MODEL_STAR,                         /*pos*/     0, 12000,   0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvGrandStar),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT(/*model*/ MODEL_BOB_CHAIN_CHOMP_GATE,  /*pos*/  1456,   768,   446, /*angle*/ 0, 326, 0,  /*behParam*/ 0x00000000, /*beh*/ bhvChainChompGate),
    OBJECT(/*model*/ MODEL_BOB_SEESAW_PLATFORM,   /*pos*/ -2303,   717,  1024, /*angle*/ 0, 45, 0,   /*behParam*/ 0x00030000, /*beh*/ bhvSeesawPlatform),
    OBJECT(/*model*/ MODEL_NONE,                  /*pos*/ -2050,     0, -3069, /*angle*/ 0, 25, 0,   /*behParam*/ 0x00000000, /*beh*/ bhvOpenableGrill),
    OBJECT(/*model*/ MODEL_PURPLE_SWITCH,         /*pos*/ -2283,     0, -3682, /*angle*/ 0, 27, 0,   /*behParam*/ 0x00000000, /*beh*/ bhvFloorSwitchGrills),
    OBJECT(/*model*/ MODEL_CHECKERBOARD_PLATFORM, /*pos*/  1612,   300,  4611, /*angle*/ 0, 0, 0,    /*behParam*/ 0x00280000, /*beh*/ bhvCheckerboardElevatorGroup),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT(/*model*/ MODEL_NONE,                  /*pos*/  1535, 3840, -5561, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00000000, /*beh*/ bhvBobBowlingBallSpawner),
    OBJECT(/*model*/ MODEL_NONE,                  /*pos*/   524, 2825, -5400, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00020000, /*beh*/ bhvBobBowlingBallSpawner),
    OBJECT(/*model*/ MODEL_BOWLING_BALL,          /*pos*/  -993,  886, -3565, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00000000, /*beh*/ bhvPitBowlingBall),
    OBJECT(/*model*/ MODEL_BOWLING_BALL,          /*pos*/  -785,  886, -4301, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00000000, /*beh*/ bhvPitBowlingBall),
    OBJECT(/*model*/ MODEL_BOBOMB_BUDDY,          /*pos*/ -5723,  140,  6017, /*angle*/ 0, 0, 0,   /*behParam*/ DIALOG_002 << 16, /*beh*/ bhvBobombBuddy),
    OBJECT(/*model*/ MODEL_BOBOMB_BUDDY,          /*pos*/ -6250,    0,  6680, /*angle*/ 0, 0, 0,   /*behParam*/ DIALOG_001 << 16, /*beh*/ bhvBobombBuddy),
    OBJECT(/*model*/ MODEL_CANNON_BASE,           /*pos*/ -5694,  128,  5600, /*angle*/ 0, 135, 0, /*behParam*/ 0x00010000, /*beh*/ bhvWaterBombCannon),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT(/*model*/ MODEL_KING_BOBOMB,           /*pos*/  1636, 4242, -5567, /*angle*/ 0, -147, 0, /*behParam*/ 0x00000000, /*beh*/ bhvKingBobomb),
    OBJECT(/*model*/ MODEL_NONE,                  /*pos*/ -6000, 1000,  2400, /*angle*/ 0, 0, 0,    /*behParam*/ 0x03000000, /*beh*/ bhvHiddenRedCoinStar),
    OBJECT(/*model*/ MODEL_NONE,                  /*pos*/ -6600, 1000,  1250, /*angle*/ 0, 0, 0,    /*behParam*/ 0x04040000, /*beh*/ bhvHiddenStar),
    OBJECT(/*model*/ MODEL_STAR,                  /*pos*/  1550, 1200,   300, /*angle*/ 0, 0, 0,    /*behParam*/ 0x05000000, /*beh*/ bhvStar),
    RETURN(),
};

const LevelScript level_bowser_3_entry_hack[] = {
    INIT_LEVEL(),
    LOAD_MIO0(/*seg*/ 0x07, _bowser_3_segment_7SegmentRomStart, _bowser_3_segment_7SegmentRomEnd),
    LOAD_MIO0(/*seg*/ 0x06, _group12_mio0SegmentRomStart, _group12_mio0SegmentRomEnd),
    LOAD_RAW( /*seg*/ 0x0D, _group12_geoSegmentRomStart,  _group12_geoSegmentRomEnd),
    LOAD_MIO0(/*seg*/ 0x0A, _bits_skybox_mio0SegmentRomStart, _bits_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE(/*seg*/ 0x09, _generic_mio0SegmentRomStart, _generic_mio0SegmentRomEnd),
    LOAD_MIO0(        /*seg*/ 0x05, _group3_mio0SegmentRomStart, _group3_mio0SegmentRomEnd),
    LOAD_RAW(         /*seg*/ 0x0C, _group3_geoSegmentRomStart,  _group3_geoSegmentRomEnd),
    LOAD_MIO0(        /*seg*/ 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW(         /*seg*/ 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER,                  bowser_geo),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_BOMB_CHILD_OBJ,   bowser_bomb_geo),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_BOMB,             bowser_bomb_geo),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_SMOKE,            bowser_impact_smoke_geo),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_FLAMES,           bowser_flames_geo),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_WAVE,             invisible_bowser_accessory_geo),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_NO_SHADOW,        bowser_geo_no_shadow),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_1,  bowser_3_geo_000290), // 0x36
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_2,  bowser_3_geo_0002A8), // 0x37
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_3,  bowser_3_geo_0002C0), // 0x38
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_4,  bowser_3_geo_0002D8), // 0x39
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_5,  bowser_3_geo_0002F0), // 0x3A
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_6,  bowser_3_geo_000308), // 0x3B
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_7,  bowser_3_geo_000320), // 0x3C
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_8,  bowser_3_geo_000338), // 0x3D
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_9,  bowser_3_geo_000350), // 0x3E
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_10, bowser_3_geo_000368), // 0x3F
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,            bowser_3_geo_000380), // 0x03

    JUMP_LINK(script_func_global_1), // 0x78-0xE1 (roughly)

//    LOAD_MODEL_FROM_GEO(MODEL_WATER_BOMB,                     water_bomb_geo), // 0x54
//    LOAD_MODEL_FROM_GEO(MODEL_WATER_BOMB_SHADOW,       water_bomb_shadow_geo), // 0x55
//    LOAD_MODEL_FROM_GEO(MODEL_KING_BOBOMB,                   king_bobomb_geo), // 0x56

//    LOAD_MODEL_FROM_GEO(MODEL_PIRANHA_PLANT,                 piranha_plant_geo), // 0x64
//    LOAD_MODEL_FROM_GEO(MODEL_WHOMP,                                 whomp_geo), // 0x67
//    LOAD_MODEL_FROM_GEO(MODEL_KOOPA_WITH_SHELL,           koopa_with_shell_geo), // 0x68
//    LOAD_MODEL_FROM_GEO(MODEL_KOOPA_WITHOUT_SHELL,     koopa_without_shell_geo), // 0xBF
//    LOAD_MODEL_FROM_GEO(MODEL_METALLIC_BALL,                 metallic_ball_geo), // 0x65
//    LOAD_MODEL_FROM_GEO(MODEL_CHAIN_CHOMP,                     chain_chomp_geo), // 0x66
//    LOAD_MODEL_FROM_GEO(MODEL_KOOPA_FLAG,                       koopa_flag_geo), // 0x6A
//    LOAD_MODEL_FROM_GEO(MODEL_WOODEN_POST,                     wooden_post_geo), // 0x6B

//    LOAD_MODEL_FROM_GEO(MODEL_BOB_BUBBLY_TREE,     bubbly_tree_geo), // 0x17
//    LOAD_MODEL_FROM_GEO(MODEL_BOB_CHAIN_CHOMP_GATE, bob_geo_000440), // 0x36
//    LOAD_MODEL_FROM_GEO(MODEL_BOB_SEESAW_PLATFORM,  bob_geo_000458), // 0x37
//    LOAD_MODEL_FROM_GEO(MODEL_BOB_BARS_GRILLS,      bob_geo_000470), // 0x38

    LOAD_MODEL_FROM_GEO(0xF8, trans_star_geo_hack),

    AREA(/*index*/ 1, bowser_3_geo_000398_hack),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 1000, 1307, 0, /*angle*/ 0, 183, 0, /*behParam*/ 0x000A0000, /*beh*/ bhvSpinAirborneCircleWarp),
        WARP_NODE(/*id*/ 0x0A, /*destLevel*/ LEVEL_BOWSER_3, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        WARP_NODE(/*id*/ 0xF0, /*destLevel*/ LEVEL_BITS, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ 0xF1, /*destLevel*/ LEVEL_BITS, /*destArea*/ 0x01, /*destNode*/ 0x0C, /*flags*/ WARP_NO_CHECKPOINT),
        TERRAIN(/*terrainData*/ bowser_3_seg7_collision_level_hack),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ 0x0002, /*seq*/ SEQ_LEVEL_BOSS_KOOPA_FINAL),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
    END_AREA(),

    AREA(/*index*/ 2, bob_geo_000488_hack),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        JUMP_LINK(script_func_local_4),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -6558,  1000,  6464, /*angle*/ 0, 135, 0,  /*behParam*/ 0x000A0000, /*beh*/ bhvSpinAirborneWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/   583,  2683, -5387, /*angle*/ 0, -154, 0, /*behParam*/ 0x000B0000, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/  1680,  3835, -5523, /*angle*/ 0, -153, 0, /*behParam*/ 0x000C0000, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -6612,  1024, -3351, /*angle*/ 0, 107, 0,  /*behParam*/ 0x000D0000, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/  1980,   768,  6618, /*angle*/ 0, -151, 0, /*behParam*/ 0x000E0000, /*beh*/ bhvFadingWarp),
        WARP_NODE(/*id*/ 0x0A, /*destLevel*/ LEVEL_BOWSER_3, /*destArea*/ 0x02, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ 0x0B, /*destLevel*/ LEVEL_BOWSER_3, /*destArea*/ 0x02, /*destNode*/ 0x0C, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ 0x0C, /*destLevel*/ LEVEL_BOWSER_3, /*destArea*/ 0x02, /*destNode*/ 0x0B, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ 0x0D, /*destLevel*/ LEVEL_BOWSER_3, /*destArea*/ 0x02, /*destNode*/ 0x0E, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ 0x0E, /*destLevel*/ LEVEL_BOWSER_3, /*destArea*/ 0x02, /*destNode*/ 0x0D, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ 0xF0, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x32, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ 0xF1, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x64, /*flags*/ WARP_NO_CHECKPOINT),
        TERRAIN(/*terrainData*/ bob_seg7_collision_level_hack),
        MACRO_OBJECTS(/*objList*/ bob_seg7_macro_objs_hack),
        SHOW_DIALOG(/*index*/ 0x00, DIALOG_000),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ 0x0000, /*seq*/ SEQ_LEVEL_GRASS),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_GRASS),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 183, /*pos*/ 0, 307, 0),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};

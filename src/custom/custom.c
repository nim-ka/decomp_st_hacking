#include <ultra64.h>
#include <stdio.h>

#include "sm64.h"
#include "level_table.h"
#include "behavior_data.h"
#include "surface_terrains.h"
#include "model_ids.h"
#include "segment_symbols.h"

#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "engine/surface_load.h"

#include "game/main.h"
#include "game/game_init.h"
#include "game/area.h"
#include "game/level_update.h"
#include "game/behavior_actions.h"
#include "game/obj_behaviors.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "game/memory.h"
#include "game/camera.h"
#include "game/spawn_sound.h"
#include "game/mario_actions_cutscene.h"
#include "game/print.h"
#include "game/spawn_object.h"
#include "game/hud.h"
#include "game/segment2.h"

#include "levels/bowser_3/header.h"
#include "levels/bob/header.h"

#include "actors/common1.h"
#include "actors/group3.h"
#include "actors/group12.h"
#include "actors/group14.h"

#include "header.h"
#include "custom.h"

extern f32 absf_2(f32);

#ifndef o
#define o gCurrentObject
#define __O_DEF
#endif

#define HACK_FUNCTION(target, hack) \
	((u32 *) ((target)))[0] = 0x08000000 | (((u32) &(hack)) >> 2 & 0x00FFFFFF); \
	((u32 *) ((target)))[1] = 0x00000000;

#define HACK_LSCRIPT(hack) \
	if (*i == (hack)) { \
		*i = virtual_to_segmented(0x02, (hack)); \
	}

#define LOAD_MODEL_FROM_GEO_BITS(model, geo) \
	if (!sLoadedBits) { \
		sAaaLoadedGraphNodesBits[(model)] = process_geo_layout(sCustomPool, (void *) ((geo))); \
	} \
	gLoadedGraphNodes[(model)] = sAaaLoadedGraphNodesBits[(model)];

#define LOAD_MODEL_FROM_GEO_BOB(model, geo) \
	if (!sLoadedBits) { \
		sAaaLoadedGraphNodesBob[(model)] = process_geo_layout(sCustomPool2, (void *) ((geo))); \
	} \
	gLoadedGraphNodes[(model)] = sAaaLoadedGraphNodesBob[(model)];

#define RED1_X -2645
#define RED1_Y 1023
#define RED1_Z 418

#define RED2_X 206
#define RED2_Y 1141
#define RED2_Z 2243

#define RED3_X 2106
#define RED3_Y 878
#define RED3_Z -1868

#define RED4_X -2296
#define RED4_Y 676
#define RED4_Z -2689

#define RED5_X 1654
#define RED5_Y 349
#define RED5_Z 308

static struct Surface *find_floor_from_list(struct SurfaceNode *surfaceNode, s32 x, s32 y, s32 z, f32 *pheight);
static void bowser_spawn_coins(void);
static void *new_segmented_to_virtual(const void *addr);
static void *new_virtual_to_segmented(u32 segmented, const void *addr);
static void render_hud_timer_hack(void);

static void plat_act_0(void);
static void plat_act_1(void);

extern const uintptr_t bhvRedsManager[];

static u8 sEnteredBowser3 = FALSE;

static struct AllocOnlyPool *sCustomPool = NULL;
static struct AllocOnlyPool *sCustomPool2 = NULL;

static u8 sLoadedBits = FALSE;
static u8 sLoadedBob = FALSE;

static void *sAaaLoadedGraphNodesBits[0x100];
static void *sAaaLoadedGraphNodesBob[0x100];

static u8 sJustSwappedPlats[10] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

static u8 sCanSwap = FALSE;

static u8 sRedsAlive[5] = { TRUE, TRUE, TRUE, TRUE, TRUE };

static f32 sJumboSpeed = 500;
static f32 sJumboY = 12000;

static u32 sTimer = 0;

// a lot of my mem pool code is really wonky because of a lot of last minute fixes i did to stop mem leaks
// for example having 2 pools totalling like 600kb is totally unnecessary
// ignore pls

void custom_entry(void) {
	u32 addr;
	void **i;

	struct Object *jumbo = find_obj_with_behavior(bhvGrandStar);

	if (gMarioState && gMarioState->action == 0x1909) {
		sCanSwap = FALSE;
	}

	if (jumbo) {
		jumbo->oAction = 2;
		jumbo->oAngleVelYaw = 0x400;

		if (!sCanSwap || find_obj_with_behavior(bhvRedCoin)) {
			cur_obj_disable_rendering_and_become_intangible(jumbo);
		} else {
			cur_obj_enable_rendering_and_become_tangible(jumbo);

			if (jumbo->oPosY > 512) {
				sJumboY -= sJumboSpeed;
				sJumboSpeed *= 1.0 - 500.0 / (12000.0 - 512.0);
			}
		}

		jumbo->oPosY = sJumboY;
	}

	if (sEnteredBowser3) {
		if (gCurrLevelNum == LEVEL_BOWSER_3) {
			if (!sCustomPool) {
				sCustomPool = alloc_only_pool_init(0x50000, MEMORY_POOL_LEFT);
			}

			if (!sCustomPool2) {
				sCustomPool2 = alloc_only_pool_init(0x50000, MEMORY_POOL_LEFT);
			}
		} else {
			if (sCustomPool) {
				alloc_only_pool_resize(sCustomPool, 0);
				sCustomPool = NULL;
			}

			if (sCustomPool2) {
				alloc_only_pool_resize(sCustomPool2, 0);
				sCustomPool2 = NULL;
			}
		}
	}

	if (gCurrLevelNum == LEVEL_BOWSER_3 && gCurrentArea && gCurrentArea->index == 1 && sCanSwap) {
		sRedsAlive[0] = FALSE;
		sRedsAlive[1] = FALSE;
		sRedsAlive[2] = FALSE;
		sRedsAlive[3] = FALSE;
		sRedsAlive[4] = FALSE;

		if (find_obj_with_behavior(bhvRedCoin)) {
			struct ObjectNode *listHead = &gObjectLists[OBJ_LIST_LEVEL];
			struct Object *obj = (struct Object *) listHead->next;

			while (listHead != (struct ObjectNode *) obj) {
				if (obj->behavior == segmented_to_virtual(bhvRedCoin) && obj->activeFlags) {
					sRedsAlive[0] |= absf_2(obj->oPosX - RED1_X) < 10.0;
					sRedsAlive[1] |= absf_2(obj->oPosX - RED2_X) < 10.0;
					sRedsAlive[2] |= absf_2(obj->oPosX - RED3_X) < 10.0;
					sRedsAlive[3] |= absf_2(obj->oPosX - RED4_X) < 10.0;
					sRedsAlive[4] |= absf_2(obj->oPosX - RED5_X) < 10.0;
				}

				obj = (struct Object *) ((struct ObjectNode *) obj)->next;
			}
		}
	}

	if (gCurrLevelNum == LEVEL_BOWSER_3) {
		gHudDisplay.flags |= HUD_DISPLAY_FLAG_TIMER;
	}

	if (sCanSwap && sTimer < 5 * 60 * 30) {
		sTimer++;
	}

	if (gPlayer1Controller->buttonPressed & L_TRIG) {
		if (sEnteredBowser3 && gCurrLevelNum == LEVEL_BOWSER_3) {
			if (sCanSwap) {
				s16 cameraAngle = gMarioState->area->camera->yaw;

				struct Surface *floor;
				f32 floorHeight = gMarioState->pos[1] - find_floor(gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2], &floor);
				f32 yChange;

				main_pool_pop_state();
				main_pool_push_state();

				if (gCurrentArea->index == 1) { // swap to bob
					load_segment(0x0E, _bobSegmentRomStart, _bobSegmentRomEnd, MEMORY_POOL_LEFT);
					load_segment_decompress(0x07, _bob_segment_7SegmentRomStart, _bob_segment_7SegmentRomEnd);
					load_segment_decompress(0x06, _group14_mio0SegmentRomStart, _group14_mio0SegmentRomEnd);
					load_segment(0x0D, _group14_geoSegmentRomStart, _group14_geoSegmentRomEnd, MEMORY_POOL_LEFT);
					load_segment_decompress(0x0A, _water_skybox_mio0SegmentRomStart, _water_skybox_mio0SegmentRomEnd);

					LOAD_MODEL_FROM_GEO_BOB(MODEL_WATER_BOMB,                     water_bomb_geo); // 0x54
					LOAD_MODEL_FROM_GEO_BOB(MODEL_WATER_BOMB_SHADOW,       water_bomb_shadow_geo); // 0x55
					LOAD_MODEL_FROM_GEO_BOB(MODEL_KING_BOBOMB,                   king_bobomb_geo); // 0x56

					LOAD_MODEL_FROM_GEO_BOB(MODEL_PIRANHA_PLANT,                 piranha_plant_geo); // 0x64
					LOAD_MODEL_FROM_GEO_BOB(MODEL_WHOMP,                                 whomp_geo); // 0x67
					LOAD_MODEL_FROM_GEO_BOB(MODEL_KOOPA_WITH_SHELL,           koopa_with_shell_geo); // 0x68
					LOAD_MODEL_FROM_GEO_BOB(MODEL_KOOPA_WITHOUT_SHELL,     koopa_without_shell_geo); // 0xBF
					LOAD_MODEL_FROM_GEO_BOB(MODEL_METALLIC_BALL,                 metallic_ball_geo); // 0x65
					LOAD_MODEL_FROM_GEO_BOB(MODEL_CHAIN_CHOMP,                     chain_chomp_geo); // 0x66
					LOAD_MODEL_FROM_GEO_BOB(MODEL_KOOPA_FLAG,                       koopa_flag_geo); // 0x6A
					LOAD_MODEL_FROM_GEO_BOB(MODEL_WOODEN_POST,                     wooden_post_geo); // 0x6B

					LOAD_MODEL_FROM_GEO_BOB(MODEL_BOB_BUBBLY_TREE,     bubbly_tree_geo); // 0x17
					LOAD_MODEL_FROM_GEO_BOB(MODEL_BOB_CHAIN_CHOMP_GATE, bob_geo_000440); // 0x36
					LOAD_MODEL_FROM_GEO_BOB(MODEL_BOB_SEESAW_PLATFORM,  bob_geo_000458); // 0x37
					LOAD_MODEL_FROM_GEO_BOB(MODEL_BOB_BARS_GRILLS,      bob_geo_000470); // 0x38

					if (!sLoadedBob) {
						sLoadedBob = TRUE;
					}

					gMarioState->pos[0] += 1000;
					gMarioState->area->camera->pos[0] += 1000;
					gMarioState->area->camera->focus[0] += 1000;
					gLakituState.curPos[0] += 1000;
					gLakituState.curFocus[0] += 1000;
					gLakituState.goalPos[0] += 1000;
					gLakituState.goalFocus[0] += 1000;
					change_area(2);

					find_obj_with_behavior(bhvChainChompGate)->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_BOB_CHAIN_CHOMP_GATE];
					find_obj_with_behavior(bhvSeesawPlatform)->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_BOB_SEESAW_PLATFORM];

					find_obj_with_behavior(bhvKingBobomb)->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_KING_BOBOMB];

					if (find_obj_with_behavior(bhvCheckerboardElevatorGroup) == NULL) {
						struct Object *obj = create_object(segmented_to_virtual(bhvCheckerboardElevatorGroup));

						obj->parentObj = obj;
						obj->header.gfx.areaIndex = 2;
						obj->header.gfx.activeAreaIndex = 2;

						obj->oPosX = 1612;
						obj->oPosY = 300;
						obj->oPosZ = 4611;

						obj->oBehParams = 0x00280000;

						geo_obj_init((struct GraphNodeObject *) &obj->header.gfx, gLoadedGraphNodes[MODEL_CHECKERBOARD_PLATFORM], gVec3fZero, gVec3sZero);
					}
				} else { // swap to bits
					load_segment(0x0E, _bowser_3SegmentRomStart, _bowser_3SegmentRomEnd, MEMORY_POOL_LEFT);
					load_segment_decompress(0x07, _bowser_3_segment_7SegmentRomStart, _bowser_3_segment_7SegmentRomEnd);
					load_segment_decompress(0x06, _group12_mio0SegmentRomStart, _group12_mio0SegmentRomEnd);
					load_segment(0x0D, _group12_geoSegmentRomStart, _group12_geoSegmentRomEnd, MEMORY_POOL_LEFT);
					load_segment_decompress(0x0A, _bits_skybox_mio0SegmentRomStart, _bits_skybox_mio0SegmentRomEnd);

					LOAD_MODEL_FROM_GEO_BITS(MODEL_BOWSER_3_FALLING_PLATFORM_1,  bowser_3_geo_000290); // 0x36
					LOAD_MODEL_FROM_GEO_BITS(MODEL_BOWSER_3_FALLING_PLATFORM_2,  bowser_3_geo_0002A8); // 0x37
					LOAD_MODEL_FROM_GEO_BITS(MODEL_BOWSER_3_FALLING_PLATFORM_3,  bowser_3_geo_0002C0); // 0x38

					LOAD_MODEL_FROM_GEO_BITS(MODEL_BOWSER,                  bowser_geo);
					LOAD_MODEL_FROM_GEO_BITS(MODEL_BOWSER_BOMB_CHILD_OBJ,   bowser_bomb_geo);
					LOAD_MODEL_FROM_GEO_BITS(MODEL_BOWSER_SMOKE,            bowser_impact_smoke_geo);
					LOAD_MODEL_FROM_GEO_BITS(MODEL_BOWSER_FLAMES,           bowser_flames_geo);
					LOAD_MODEL_FROM_GEO_BITS(MODEL_BOWSER_WAVE,             invisible_bowser_accessory_geo);
					LOAD_MODEL_FROM_GEO_BITS(MODEL_BOWSER_NO_SHADOW,        bowser_geo_no_shadow);

					if (!sLoadedBits) {
						sLoadedBits = TRUE;
					}

					gMarioState->pos[0] -= 1000;
					gMarioState->area->camera->pos[0] -= 1000;
					gMarioState->area->camera->focus[0] -= 1000;
					gLakituState.curPos[0] -= 1000;
					gLakituState.curFocus[0] -= 1000;
					gLakituState.goalPos[0] -= 1000;
					gLakituState.goalFocus[0] -= 1000;
					change_area(1);

					sJustSwappedPlats[0] = TRUE;
					sJustSwappedPlats[1] = TRUE;
					sJustSwappedPlats[2] = TRUE;
					sJustSwappedPlats[3] = TRUE;
					sJustSwappedPlats[4] = TRUE;
					sJustSwappedPlats[5] = TRUE;
					sJustSwappedPlats[6] = TRUE;
					sJustSwappedPlats[7] = TRUE;
					sJustSwappedPlats[8] = TRUE;
					sJustSwappedPlats[9] = TRUE;

					{
						struct Object *bowser = find_obj_with_behavior(bhvBowser);

						if (bowser) {
							unload_object(bowser);
						}

						if (sRedsAlive[0]) {
							struct Object *red1 = create_object(segmented_to_virtual(bhvRedCoin));
							red1->oPosX = RED1_X;
							red1->oPosY = RED1_Y;
							red1->oPosZ = RED1_Z;
							red1->parentObj = red1;
							red1->header.gfx.areaIndex = 1;
							red1->header.gfx.activeAreaIndex = 1;
							geo_obj_init((struct GraphNodeObject *) &red1->header.gfx, gLoadedGraphNodes[MODEL_RED_COIN], gVec3fZero, gVec3sZero);
						}

						if (sRedsAlive[1]) {
							struct Object *red2 = create_object(segmented_to_virtual(bhvRedCoin));
							red2->oPosX = RED2_X;
							red2->oPosY = RED2_Y;
							red2->oPosZ = RED2_Z;
							red2->parentObj = red2;
							red2->header.gfx.areaIndex = 1;
							red2->header.gfx.activeAreaIndex = 1;
							geo_obj_init((struct GraphNodeObject *) &red2->header.gfx, gLoadedGraphNodes[MODEL_RED_COIN], gVec3fZero, gVec3sZero);
						}

						if (sRedsAlive[2]) {
							struct Object *red3 = create_object(segmented_to_virtual(bhvRedCoin));
							red3->oPosX = RED3_X;
							red3->oPosY = RED3_Y;
							red3->oPosZ = RED3_Z;
							red3->parentObj = red3;
							red3->header.gfx.areaIndex = 1;
							red3->header.gfx.activeAreaIndex = 1;
							geo_obj_init((struct GraphNodeObject *) &red3->header.gfx, gLoadedGraphNodes[MODEL_RED_COIN], gVec3fZero, gVec3sZero);
						}

						if (sRedsAlive[3]) {
							struct Object *red4 = create_object(segmented_to_virtual(bhvRedCoin));
							red4->oPosX = RED4_X;
							red4->oPosY = RED4_Y;
							red4->oPosZ = RED4_Z;
							red4->parentObj = red4;
							red4->header.gfx.areaIndex = 1;
							red4->header.gfx.activeAreaIndex = 1;
							geo_obj_init((struct GraphNodeObject *) &red4->header.gfx, gLoadedGraphNodes[MODEL_RED_COIN], gVec3fZero, gVec3sZero);
						}

						if (sRedsAlive[4]) {
							struct Object *red5 = create_object(segmented_to_virtual(bhvRedCoin));
							red5->oPosX = RED5_X;
							red5->oPosY = RED5_Y;
							red5->oPosZ = RED5_Z;
							red5->parentObj = red5;
							red5->header.gfx.areaIndex = 1;
							red5->header.gfx.activeAreaIndex = 1;
							geo_obj_init((struct GraphNodeObject *) &red5->header.gfx, gLoadedGraphNodes[MODEL_RED_COIN], gVec3fZero, gVec3sZero);
						}
					}
				}

				{
					struct ObjectNode *listHead = &gObjectLists[OBJ_LIST_SURFACE];
					struct Object *obj = (struct Object *) listHead->next;

					while (listHead != (struct ObjectNode *) obj) {
						gCurrentObject = obj;

						if (obj->behavior == segmented_to_virtual(bhvFallingBowserPlatform)) {
							plat_act_0();
						}

						if (obj->collisionData) {
							load_object_collision_model();
						}

						gCurrentObject = NULL;

						obj = (struct Object *) ((struct ObjectNode *) obj)->next;
					}
				}

				gMarioState->area = gCurrentArea;
				gMarioState->area->camera->yaw = cameraAngle;

				yChange = floorHeight + find_floor(gMarioState->pos[0], 20000, gMarioState->pos[2], &floor) - gMarioState->pos[1];

				gMarioState->pos[1] += yChange;
				gMarioState->area->camera->pos[1] += yChange;
				gMarioState->area->camera->focus[1] += yChange;
				gLakituState.curPos[1] += yChange;
				gLakituState.curFocus[1] += yChange;
				gLakituState.goalPos[1] += yChange;
				gLakituState.goalFocus[1] += yChange;
			}
		} else {
			warp_special(LEVEL_BOWSER_3);
			sEnteredBowser3 = TRUE;
		}
	}

	if (gPlayer1Controller->buttonDown & D_JPAD) {
		char buf[256];
		u32 mainPoolFreeSpace = * (u32 *) 0x8033B480;
		u32 mainPoolTotalSpace = (* (u32 *) 0x8033B488) - (* (u32 *) 0x8033B484);

		sprintf(buf, "mpfs %08x", mainPoolFreeSpace);
		print_text(22, 190, buf);
		sprintf(buf, "mpts %08x", mainPoolTotalSpace);
		print_text(22, 170, buf);
		sprintf(buf, "mppt %.2f", 100.0 * mainPoolFreeSpace / mainPoolTotalSpace);
		print_text(22, 150, buf);

		if (sCustomPool) {
			sprintf(buf, "cpfs %08x", sCustomPool->usedSpace);
			print_text(22, 130, buf);
			sprintf(buf, "cpts %08x", sCustomPool->totalSpace);
			print_text(22, 110, buf);
			sprintf(buf, "cppt %.2f", 100.0 * sCustomPool->usedSpace / sCustomPool->totalSpace);
			print_text(22, 90, buf);
		} else {
			print_text(22, 130, "cpfs nul");
			print_text(22, 110, "cpts nul");
			print_text(22, 90, "cppt nul");
		}

		if (sCustomPool2) {
			sprintf(buf, "cpfs %08x", sCustomPool2->usedSpace);
			print_text(22, 70, buf);
			sprintf(buf, "cpts %08x", sCustomPool2->totalSpace);
			print_text(22, 50, buf);
			sprintf(buf, "cppt %.2f", 100.0 * sCustomPool2->usedSpace / sCustomPool2->totalSpace);
			print_text(22, 30, buf);
		} else {
			print_text(22, 70, "cpfs nul");
			print_text(22, 50, "cpts nul");
			print_text(22, 30, "cppt nul");
		}
	}

	addr = (u32) &sPlatActions;
	* (u16 *) 0x802B838E = (addr >> 16) + ((addr & 0xFFFF) >= 0x8000);
	* (u16 *) 0x802B8396 = addr & 0xFFFF;

	* (u8 *) 0x802B32D4 = 0x10;

	HACK_FUNCTION(0x8038156C, find_floor_from_list);
	HACK_FUNCTION(0x802B65D0, bowser_spawn_coins);
	HACK_FUNCTION(&segmented_to_virtual, new_segmented_to_virtual);
	HACK_FUNCTION(&virtual_to_segmented, new_virtual_to_segmented);
	HACK_FUNCTION(0x802E395C, render_hud_timer_hack);

	* (void **) segmented_to_virtual((void *) 0x15000644) = virtual_to_segmented(0x02, level_bowser_3_entry_hack);

	for (i = (void **) level_bowser_3_entry_hack; *i != (void *) 0x02040000; i++) {
		HACK_LSCRIPT(bowser_3_geo_000398_hack);
		HACK_LSCRIPT(bob_geo_000488_hack);
		HACK_LSCRIPT(bowser_3_seg7_collision_level_hack);
		HACK_LSCRIPT(bob_seg7_collision_level_hack);
		HACK_LSCRIPT(bob_seg7_macro_objs_hack);
		HACK_LSCRIPT(trans_star_geo_hack);
	}
}

struct Object sPlatforms[10];

static void plat_act_0(void) {
	struct Object *bowser;
	s32 idx = o->oBehParams2ndByte - 1;

	* (u16 *) segmented_to_virtual((u8 *) sBowserFallingPlatform[o->oBehParams2ndByte].collision + 0x34) = SURFACE_NOT_SLIPPERY;
	obj_set_collision_data(o, sBowserFallingPlatform[o->oBehParams2ndByte].collision);

	if (sJustSwappedPlats[idx]) {
		if (idx == 0) {
			//spawn_object(o, MODEL_NONE, virtual_to_segmented(0x02, bhvRedsManager));
		}

		o->rawData = sPlatforms[idx].rawData;
		sJustSwappedPlats[idx] = FALSE;

		plat_act_1();
		return;
	} else {
		sPlatforms[idx].rawData = o->rawData;
	}

	bowser = o->oBitsPlatformBowser = cur_obj_nearest_object_with_behavior(bhvBowser);

	if (bowser && bowser->oAction == 4 && bowser->oSubAction == 11) {
		o->oAction = 1;
	}
}

static u8 sChangeCamera = TRUE;

static void plat_act_1(void) {
	s32 idx = o->oBehParams2ndByte - 1;

	if (sJustSwappedPlats[idx]) {
		o->rawData = sPlatforms[idx].rawData;
		sJustSwappedPlats[idx] = FALSE;
	} else {
		sPlatforms[idx].rawData = o->rawData;
	}

	if (gLakituState.mode == CAMERA_MODE_8_DIRECTIONS) {
		sChangeCamera = FALSE;
	} else if (sChangeCamera) {
		set_camera_mode(gCamera, CAMERA_MODE_8_DIRECTIONS, 1);
	}

	if (o->oSubAction == 0) {
		if (o->oTimer > o->oBehParams2ndByte * 15) {
			o->oTimer = 0;
			o->oSubAction++;
		}

		o->oHomeY = o->oPosY;

		o->oVelX = 0;
		o->oVelY = 0;
		o->oVelZ = 0;
		o->oGravity = 0;
	} else if (o->oSubAction == 1) {
		if (o->oTimer < 25) {
			o->oVelY = 20 * sins(o->oTimer * 0x4000);
		} else {
			o->oTimer = 0;
			o->oSubAction++;
			o->oSubAction++;
		}
	} else if (o->oSubAction == 2) {
		if (o->oVelY == 0) {
			o->oVelY = 31;
		}
		o->oGravity = -2;

		if (o->oPosY <= o->oHomeY) {
			o->oPosY = o->oHomeY;
			o->oVelY = 0;
			o->oGravity = 0;

			o->oTimer = 0;
			o->oSubAction++;
		}
	} else if (o->oSubAction == 3) {
		Vec3f center, inverse;
		Vec3s rotate, pos;
		Mat4 transform;
		f32 scale = o->oTimer / 30.0;

		u16 oldSeed = gRandomSeed16;
		gRandomSeed16 = o->oBehParams2ndByte * 0x74BD;

		rotate[0] = scale * random_sign() * ((random_u16() % 50) + 10) * 0x10000 / 360.0; // pitch
		rotate[1] = scale * 0; // yaw
		rotate[2] = scale * random_sign() * ((random_u16() % 13) + 5) * 0x10000 / 360.0; // roll

		gRandomSeed16 = oldSeed;

		center[0] = scale * sBowserFallingPlatform[o->oBehParams2ndByte].posX + sins(sBowserFallingPlatform[o->oBehParams2ndByte].angle) * 1000;
		center[1] = scale * -500;
		center[2] = scale * sBowserFallingPlatform[o->oBehParams2ndByte].posZ + coss(sBowserFallingPlatform[o->oBehParams2ndByte].angle) * 1000;

		vec3f_set(inverse, -center[0], -center[1], -center[2]);
		vec3s_set(pos, center[0], center[1], center[2]);

		mtxf_translate(transform, inverse);
		mtxf_rotate_zxy_and_translate(transform, center, rotate);
		mtxf_mul_vec3s(transform, pos);

		pos[0] = scale * 0.1 * pos[0];
		pos[1] = scale * (700 + pos[1]);
		pos[2] = scale * 0.1 * pos[2];

		o->oVelX = pos[0] - o->oPosX;
		o->oVelY = pos[1] - o->oPosY;
		o->oVelZ = pos[2] - o->oPosZ;

		rotate[0] = scale * -rotate[0];
		rotate[1] = scale * -rotate[1];
		rotate[2] = scale * -rotate[2];

		o->oAngleVelPitch = rotate[0] - o->oFaceAnglePitch;
		o->oAngleVelYaw = rotate[1] - o->oFaceAngleYaw;
		o->oAngleVelRoll = rotate[2] - o->oFaceAngleRoll;

		if (o->oTimer > 30) {
			o->oVelX = 0;
			o->oVelY = 0;
			o->oVelZ = 0;

			o->oAngleVelPitch = 0;
			o->oAngleVelRoll = 0;

			o->oSubAction++;
		}
	}

	cur_obj_rotate_face_angle_using_vel();
	cur_obj_move_using_vel_and_gravity();
}

static void plat_act_2(void) {
	s32 idx = o->oBehParams2ndByte - 1;

	if (sJustSwappedPlats[idx]) {
		o->rawData = sPlatforms[idx].rawData;
		sJustSwappedPlats[idx] = FALSE;
	} else {
		sPlatforms[idx].rawData = o->rawData;
	}
}

void (*sPlatActions[])(void) = {
	plat_act_0,
	plat_act_1,
	plat_act_2
};

s32 true(void) {
	return TRUE;
}

s32 false(void) {
	return FALSE;
}

#define RED_MOVE_INIT(obj)     obj->oMoveAngleYaw = atan2s(obj->oHomeZ - obj->oPosZ, obj->oHomeX - obj->oPosX); \
    obj->oStarSpawnDisFromHome = sqrtf(sqr(obj->oHomeX - obj->oPosX) + sqr(obj->oHomeZ - obj->oPosZ)); \
    obj->oVelY = (obj->oHomeY - obj->oPosY) / 30.0f; \
    obj->oForwardVel = obj->oStarSpawnDisFromHome / 30.0f; \
    obj->oStarSpawnUnkFC = obj->oPosY; \
    obj->activeFlags |= 0x20; \
    obj->oIntangibleTimer = -1;

static void reds_manager_init(void) {
	struct Object *red1 = spawn_object(o, MODEL_RED_COIN, bhvRedCoin);
	struct Object *red2 = spawn_object(o, MODEL_RED_COIN, bhvRedCoin);
	struct Object *red3 = spawn_object(o, MODEL_RED_COIN, bhvRedCoin);
	struct Object *red4 = spawn_object(o, MODEL_RED_COIN, bhvRedCoin);
	struct Object *red5 = spawn_object(o, MODEL_RED_COIN, bhvRedCoin);

	o->OBJECT_FIELD_OBJ(0x1C) = red1;
	o->OBJECT_FIELD_OBJ(0x1D) = red2;
	o->OBJECT_FIELD_OBJ(0x1E) = red3;
	o->OBJECT_FIELD_OBJ(0x1F) = red4;
	o->OBJECT_FIELD_OBJ(0x20) = red5;

	red1->oHomeX = RED1_X;
	red1->oHomeY = RED1_Y;
	red1->oHomeZ = RED1_Z;

	red2->oHomeX = RED2_X;
	red2->oHomeY = RED2_Y;
	red2->oHomeZ = RED2_Z;

	red3->oHomeX = RED3_X;
	red3->oHomeY = RED3_Y;
	red3->oHomeZ = RED3_Z;

	red4->oHomeX = RED4_X;
	red4->oHomeY = RED4_Y;
	red4->oHomeZ = RED4_Z;

	red5->oHomeX = RED5_X;
	red5->oHomeY = RED5_Y;
	red5->oHomeZ = RED5_Z;

	RED_MOVE_INIT(red1)
	RED_MOVE_INIT(red2)
	RED_MOVE_INIT(red3)
	RED_MOVE_INIT(red4)
	RED_MOVE_INIT(red5)

	cutscene_object(CUTSCENE_RED_COIN_STAR_SPAWN, red1);

	o->oTimer = 0;
}

#undef RED_MOVE_INIT

#define RED_MOVE_STAGE_1(obj)             obj_move_xyz_using_fvel_and_yaw(obj); \
            obj->oStarSpawnUnkFC += obj->oVelY; \
            obj->oPosY = obj->oStarSpawnUnkFC + sins((o->oTimer * 0x8000) / 30) * 400.0f; \
            spawn_object(obj, MODEL_NONE, bhvSparkleSpawn);

#define RED_MOVE_STAGE_2(obj)            if (o->oTimer < 20) \
                obj->oVelY = 20 - o->oTimer; \
            else \
                obj->oVelY = -10.0f; \
            \
            spawn_object(obj, MODEL_NONE, bhvSparkleSpawn); \
            obj_move_xyz_using_fvel_and_yaw(obj); \
            cur_obj_play_sound_1(SOUND_ENV_STAR); \
            \
            if (obj->oPosY < obj->oHomeY) { \
                cur_obj_play_sound_2(SOUND_GENERAL_STAR_APPEARS); \
                obj->oIntangibleTimer = 0; \
                obj->oPosY = obj->oHomeY; \
                o->oAction = 2; \
            }

static void reds_manager_loop(void) {
	struct Object *red1 = o->OBJECT_FIELD_OBJ(0x1C);
	struct Object *red2 = o->OBJECT_FIELD_OBJ(0x1D);
	struct Object *red3 = o->OBJECT_FIELD_OBJ(0x1E);
	struct Object *red4 = o->OBJECT_FIELD_OBJ(0x1F);
	struct Object *red5 = o->OBJECT_FIELD_OBJ(0x20);

	if (o->oSubAction < 5 && red1->parentObj != o) {
		red1->parentObj = o;
		red2->parentObj = o;
		red3->parentObj = o;
		red4->parentObj = o;
		red5->parentObj = o;
		o->oSubAction++;
	}

	if (o->oAction == 0) {
		RED_MOVE_STAGE_1(red1)
		RED_MOVE_STAGE_1(red2)
		RED_MOVE_STAGE_1(red3)
		RED_MOVE_STAGE_1(red4)
		RED_MOVE_STAGE_1(red5)

		if (o->oTimer == 29) {
			red1->oForwardVel = 0;
			red2->oForwardVel = 0;
			red3->oForwardVel = 0;
			red4->oForwardVel = 0;
			red5->oForwardVel = 0;
			o->oAction++;
		}
	} else if (o->oAction == 1) {
		RED_MOVE_STAGE_2(red1)
		RED_MOVE_STAGE_2(red2)
		RED_MOVE_STAGE_2(red3)
		RED_MOVE_STAGE_2(red4)
		RED_MOVE_STAGE_2(red5)
	} else if (o->oAction == 2) {
gMarioState->pos[0] = 0;
gMarioState->pos[1] = 307;
gMarioState->pos[2] = 0;
gMarioState->faceAngle[0] = 0;
gMarioState->faceAngle[1] = 0;
gMarioState->faceAngle[2] = 0;
		if (o->oTimer == 20) {
			gObjCutsceneDone = TRUE;

			red1->activeFlags &= ~0x20;
			red2->activeFlags &= ~0x20;
			red3->activeFlags &= ~0x20;
			red4->activeFlags &= ~0x20;
			red5->activeFlags &= ~0x20;

			* (s16 *) 0x8033C776 = -4.84 * 0x10000 / 8;
			o->oAction++;
		}
	} else if (o->oAction == 3) {
		if (o->oTimer < 30) {
			 * (s16 *) 0x8033C776 += 0x10000 / 64;
		}

		if (o->oTimer == 40) {
			set_mario_npc_dialog(0);
			o->oAction++;
		}
	} else if (o->oAction == 4) {
		if (gMarioState->action != 0x20001306) {
			sCanSwap = TRUE;
			o->oAction++;
		}
	}
}

#undef RED_MOVE_STAGE_1
#undef RED_MOVE_STAGE_2

const uintptr_t bhvRedsManager[] = {
	0x00080000,
	0x11010001,
	0x01000030,
	0x0C000000,
	(u32) &reds_manager_init,
	0x08000000,
	0x0C000000,
	(u32) &reds_manager_loop,
	0x09000000
};

static void bowser_spawn_coins(void) {
	gSecondCameraFocus = spawn_object(o, MODEL_NONE, virtual_to_segmented(0x02, bhvRedsManager));
}

/**
 * Iterate through the list of floors and find the first floor under a given point.
 */
static struct Surface *find_floor_from_list(struct SurfaceNode *surfaceNode, s32 x, s32 y, s32 z, f32 *pheight) {
    register struct Surface *surf;
    register s32 x1, z1, x2, z2, x3, z3;
    f32 nx, ny, nz;
    f32 oo;
    f32 height;
    struct Surface *floor = NULL;

    // Iterate through the list of floors until there are no more floors.
    while (surfaceNode != NULL) {
        surf = surfaceNode->surface;
        surfaceNode = surfaceNode->next;

        x1 = surf->vertex1[0];
        z1 = surf->vertex1[2];
        x2 = surf->vertex2[0];
        z2 = surf->vertex2[2];

        // Check that the point is within the triangle bounds.
        if ((z1 - z) * (x2 - x1) - (x1 - x) * (z2 - z1) < 0) {
            continue;
        }

        // To slightly save on computation time, set this later.
        x3 = surf->vertex3[0];
        z3 = surf->vertex3[2];

        if ((z2 - z) * (x3 - x2) - (x2 - x) * (z3 - z2) < 0) {
            continue;
        }
        if ((z3 - z) * (x1 - x3) - (x3 - x) * (z1 - z3) < 0) {
            continue;
        }

        // Determine if we are checking for the camera or not.
        if (gCheckingSurfaceCollisionsForCamera != 0) {
            if (surf->flags & SURFACE_FLAG_NO_CAM_COLLISION) {
                continue;
            }
        }
        // If we are not checking for the camera, ignore camera only floors.
        else if (surf->type == SURFACE_CAMERA_BOUNDARY) {
            continue;
        }

        nx = surf->normal.x;
        ny = surf->normal.y;
        nz = surf->normal.z;
        oo = surf->originOffset;

        // If a wall, ignore it. Likely a remnant, should never occur.
        if (ny == 0.0f) {
            continue;
        }

        // Find the height of the floor at a given location.
        height = -(x * nx + nz * z + oo) / ny;
        // Checks for floor interaction with a 78 unit buffer.
        if (y - (height + -78.0f) < 0.0f) {
            continue;
        }

        if (floor == NULL || *pheight < height) {
            *pheight = height;
            floor = surf;
        }
    }

    //! (Surface Cucking) Since only the first floor is returned and not the highest,
    //  higher floors can be "cucked" by lower floors.
    return floor;
}

extern uintptr_t sSegmentTable[32];

static void *new_segmented_to_virtual(const void *addr) {
	size_t segment = (uintptr_t) addr >> 24;
	size_t offset = (uintptr_t) addr & 0x00FFFFFF;

	return (void *) (((sSegmentTable[segment] + offset) & 0x00FFFFFF) | 0x80000000);
}

static void *new_virtual_to_segmented(u32 segment, const void *addr) {
	size_t offset = ((uintptr_t) addr & 0x1FFFFFFF) - sSegmentTable[segment];
	offset &= 0x00FFFFFF;

	return (void *) ((segment << 24) | offset);
}

struct Object *find_obj_with_behavior(const BehaviorScript *behavior) {
	uintptr_t *behaviorAddr = segmented_to_virtual(behavior);
	struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];
	struct ObjectNode *obj;

	if (!listHead) {
		return NULL;
	}

	obj = listHead->next;

	while (listHead != obj) {
		if (((struct Object *) obj)->behavior == behaviorAddr) {
			return (struct Object *) obj;
		}

		obj = obj->next;
	}

	return NULL;
}

void render_hud_timer_hack(void) {
	u16 mins = sTimer / 1800;
	u16 secs = (sTimer - mins * 1800) / 30;
	u16 fracs = ((f32)((sTimer - mins * 1800 - secs * 30) & 0xFFFF) * 3.3333) + 0.5;
	u8 *(*hudLUT)[58] = segmented_to_virtual(&main_hud_lut);

	print_text(320 - 150, 185, "TIME");
	print_text_fmt_int(320 - 91, 185, "%0d", mins);
	print_text_fmt_int(320 - 71, 185, "%02d", secs);
	print_text_fmt_int(320 - 37, 185, "%02d", fracs);
	gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
	render_hud_tex_lut(320 - 81, 32, (*hudLUT)[GLYPH_APOSTROPHE]);
	render_hud_tex_lut(320 - 46, 32, (*hudLUT)[GLYPH_DOUBLE_QUOTE]);
	gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
}

#ifdef __O_DEF
#undef o
#undef __O_DEF
#endif

#undef HACK_FUNCTION
#undef HACK_LSCRIPT
#undef LOAD_MODEL_FROM_GEO_BITS
#undef LOAD_MODEL_FROM_GEO_BOB

#undef RED1_X
#undef RED1_Y
#undef RED1_Z
#undef RED2_X
#undef RED2_Y
#undef RED2_Z
#undef RED3_X
#undef RED3_Y
#undef RED3_Z
#undef RED4_X
#undef RED4_Y
#undef RED4_Z
#undef RED5_X
#undef RED5_Y
#undef RED5_Z

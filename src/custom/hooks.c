#include <ultra64.h>
#include <stdio.h>

#include "sm64.h"

#include "segments.h"
#include "segment_symbols.h"

#include "game/memory.h"
#include "game/profiler.h"

#include "custom.h"

#define ALIGN16(val) (((val) + 0xF) & ~0xF)

extern void draw_profiler_mode_1(void);

static u32 gCustomLoaded = CUSTOM_LOADED;

UNUSED void custom_hook(enum ProfilerGameEvent eventID) {
    if (eventID != LEVEL_SCRIPT_EXECUTE) {
        return;
    }

    if (gCustomLoaded != CUSTOM_LOADED) {
        draw_profiler_mode_1(); // load_custom_code_segment_hook();
    } else {
        custom_entry();
    }
}

UNUSED void load_custom_code_segment_hook(void) {
    void *addr = (void *) SEG_CUSTOM;

    u32 size = ALIGN16(_customSegmentRomEnd - _customSegmentRomStart);
    u32 noloadSize = ALIGN16(_customSegmentNoloadEnd - _customSegmentNoloadStart);

    bzero(addr, size);
    osWritebackDCacheAll();
    dma_read(addr, _customSegmentRomStart, _customSegmentRomEnd);
    osInvalICache(addr, size);
    osInvalDCache(addr, size);

    bzero(_customSegmentNoloadStart, noloadSize);
}

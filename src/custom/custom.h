#ifndef CUSTOM_H
#define CUSTOM_H

extern void (*sPlatActions[])(void);

extern u8 gCustomEnd[];

extern void custom_entry(void);

extern s32 true(void);
extern s32 false(void);

extern struct Object *find_obj_with_behavior(const BehaviorScript *behavior);

#endif

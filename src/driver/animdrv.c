/*
 * Warning, inlining hell
 */
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"

#include <stdio.h>

#include <string.h>

#define TESTHEAP_SIZE (u32)(1.5 * 1024 * 1024)
#define PI 3.14159265358979323846264338327950288419716939937510f

extern GlobalWork* gp;
extern int sprintf(char* str, const char* format, ...);
extern double tan(double arg);

//.data
u8 _vivihimoData[0xC00] = {
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x4C, 0xCC, 0xCD, 0x3F, 0x99, 0x99, 0x9A, 0x3F, 0x4C, 0xCC, 0xCD,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x4C, 0xCC, 0xCD, 0x3F, 0x33, 0x33, 0x33,
    0x3F, 0x00, 0x00, 0x00, 0x3F, 0x33, 0x33, 0x33, 0x3E, 0xCC, 0xCC, 0xCD, 0x3F, 0x33, 0x33, 0x33,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3E, 0x4C, 0xCC, 0xCD, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3E, 0x4C, 0xCC, 0xCD, 0x3F, 0x4C, 0xCC, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x4C, 0xCC, 0xCD,
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x4C, 0xCC, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x4C, 0xCC, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x4C, 0xCC, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x4C, 0xCC, 0xCD,
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x4C, 0xCC, 0xCD, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x4C, 0xCC, 0xCD, 0x3F, 0x66, 0x66, 0x66, 0x3F, 0x33, 0x33, 0x33,
    0x3F, 0x19, 0x99, 0x9A, 0x3F, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
};

typedef struct MoaChain {
    const char* a;
    const char* b;
} MoaChain;
const s32 dat_80419a28 = 0x2F;
MoaChain g_moamoa_chain_tbl[47] = {
    { "joint_00", "jointdai1_A01" },
    { "jointdai1_A01", "jointdai1_A02" },
    { "jointdai1_A02", "jointdai1_A03" },
    { "jointdai1_A03", "jointdai1_A04" },
    { "jointdai1_A04", "jointdai1_A05" },
    { "joint_00", "jointdai1_B01" },
    { "jointdai1_B01", "jointdai1_B02" },
    { "jointdai1_B02", "jointdai1_B03" },
    { "jointdai1_B03", "jointdai1_B04" },
    { "jointdai1_B04", "jointdai1_B05" },
    { "joint_00", "jointdai1_C01" },
    { "jointdai1_C01", "jointdai1_C02" },
    { "jointdai1_C02", "jointdai1_C03" },
    { "jointdai1_C03", "jointdai1_C04" },
    { "jointdai1_C04", "jointdai1_C05" },
    { "joint_00", "jointdai2_01" },
    { "jointdai2_01", "jointdai2_02" },
    { "jointdai2_02", "jointdai2_03" },
    { "jointdai2_03", "jointdai2_04" },
    { "jointdai2_04", "jointdai2_05" },
    { "jointdai2_05", "jointdai2_06" },
    { "jointdai2_06", "jointdai2_07" },
    { "jointdai2_07", "jointdai2_upper01" },
    { "jointdai2_upper01", "jointdai2_upper02" },
    { "jointdai2_upper02", "jointdai2_upper03" },
    { "jointdai2_upper03", "jointdai2_upper04" },
    { "jointdai2_07", "jointdai2_lower01" },
    { "jointdai2_lower01", "jointdai2_lower02" },
    { "jointdai2_lower02", "jointdai2_lower03" },
    { "jointdai2_lower03", "jointdai2_lower04" },
    { "joint_00", "jointasi_A01" },
    { "jointasi_A01", "jointasi_A02" },
    { "jointasi_A02", "jointasi_A03" },
    { "jointasi_A03", "jointasi_A04" },
    { "joint_00", "jointasi_B01" },
    { "jointasi_B01", "jointasi_B02" },
    { "jointasi_B02", "jointasi_B03" },
    { "jointasi_B03", "jointasi_B04" },
    { "joint_00", "jointasi_C01" },
    { "jointasi_C01", "jointasi_C02" },
    { "jointasi_C02", "jointasi_C03" },
    { "jointasi_C03", "jointasi_C04" },
    { "joint_00", "jointasi_D01" },
    { "jointasi_D01", "jointasi_D02" },
    { "jointasi_D02", "jointasi_D03" },
    { "jointasi_D03", "jointasi_D04" },
    { "joint_00", "joint_core" },
};

//.bss
Mtx g_modeling_mtx_stack[32];
static AnimationWork work;
f32 tanfTbl[0xB2];

//.sdata
s32 vivihimoTailGroupNo[4] = { 0, 2, -1, -1 };
void* vivihimoData[4] = {
    NULL,
    (u8*)&_vivihimoData,
    (u8*)&_vivihimoData + 0x400,
    (u8*)&_vivihimoData + 0x800,
};
static AnimationWork* wp = &work;

//.sbss
s32 anim_unused_sbss;
Mtx* g_modeling_mtx;
s32 g_modeling_mtx_lv;

// local declarations
void animPoseRefresh(void);
void animPaperPoseDisp(CameraId camId, void* param);
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);
void animPaperPoseDispSub(s32 unused, AnimationPose* pose);
void renderProc(s32 shapeId, s32 unused);
void pushGXModelMtx_JointNode__(AnimPoseNode* node, f32 sx, f32 sy, f32 sz);
void pushGXModelMtx_TransformNode__(AnimPoseNode* node);
void evalProc(s32 parentId, s32 groupId);


AnimationWork* animGetPtr(void) {
    return wp;
}

OSTime animTimeGetTime(BOOL globalTime) {
    if (globalTime == FALSE) {
        return OSTicksToMilliseconds(gp->renderFieldTime);
    }
    return OSTicksToMilliseconds(gp->renderTime);
}

void initTestHeap(void) {
    if (!wp->testHeap) { // allocate 1.5MiB test heap
        wp->testHeap = __memAlloc(HEAP_DEFAULT, TESTHEAP_SIZE);
    }
    wp->testAlloc = (u32)wp->testHeap;
}

// heavily inlined
void* testAlloc(u32 size) {
    void* alloc = (void*)wp->testAlloc;

    if (size & 31) {
        size += 32 - (size & 31);
    }
    wp->testAlloc += size;
    if ((u32)wp->testAlloc >= (u32)wp->testHeap + TESTHEAP_SIZE) { // need to allocate more
        animPoseRefresh();
        alloc = testAlloc(size);
    }
    return alloc;
}

void animInit(void) {
    int i;
    FileEntry* file;

    wp->totalFiles = 64; // TODO: #define these?
    wp->totalTextures = 64;
    wp->totalPoses = 256;
    wp->mFloatScratchRP = 1024;
    wp->mFloatScratchWP = 0;
    wp->files = __memAlloc(HEAP_DEFAULT, sizeof(AnimationPoseFile) * wp->totalFiles);
    wp->textures = __memAlloc(HEAP_DEFAULT, sizeof(AnimationTextureFile) * wp->totalTextures);
    wp->poses = __memAlloc(HEAP_DEFAULT, sizeof(AnimationPose) * wp->totalPoses);
    wp->mbUseFloatScratch = 0;
    wp->mpFloatScratch = __memAlloc(HEAP_DEFAULT, sizeof(Vec) * wp->mFloatScratchRP);

    wp->mpTexObjs[0] = NULL;
    wp->mpTexObjs[1] = NULL;
    wp->mpCurTexObj = NULL;

    wp->mpTexDatas[0] = NULL;
    wp->mpTexDatas[1] = NULL;
    wp->mpCurTexData = NULL;

    wp->paperMtxPtr[0] = NULL;
    wp->paperMtxPtr[1] = NULL;
    wp->paperMtxPtr[2] = NULL;

    wp->field_0xD8 = 1;
    wp->field_0xDC = 1;

    wp->testHeap = NULL;
    wp->inBattle = FALSE;
    initTestHeap(); // inlined

    for (i = 0; i < 0xB2; i++) {
        tanfTbl[i] = (f32)tan((i - 0x59) * 0.017453289f);
    }
    file = fileAllocf(0, "a/vivian.bin");
    if (file) {
        memcpy_as4(vivihimoData[1], *file->data, sizeof(_vivihimoData));
        memcpy_as4(vivihimoTailGroupNo, (void*)((u32)*file->data + sizeof(_vivihimoData)), sizeof(vivihimoTailGroupNo));
        fileFree(file);
    }
    fileSetCurrentArchiveType(0);
    wp->mp_ag2tg = fileAllocf(0, "a/ag2tg.bin");
}

void animMain(void) {
    dispEntry(CAMERA_OFFSCREEN, 0.0f, 1, animPaperPoseDisp, NULL);
}

inline AnimationPoseData* animPoseGetBaseData(AnimationPose* pose) {
    return *wp->files[pose->fileId].handle->data;
}

#define animPoseGetBaseData(pose) (*(AnimationPoseData**)(wp->files[(pose)->fileId].handle->data))

// heavily inlined, TODO helper function for that AnimPoseData* data, copy of animPoseGetAnimBaseDataPtr with AnimPose*
// arg
void animPose_AllocBuffer(AnimationPose* pose) {
    pose->mpBufferVtxPos = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferPosNum);
    pose->mpVtxArrayPos = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferPosNum);

    pose->mpBufferVtxNrm = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferNrmNum);
    pose->mpVtxArrayNrm = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferNrmNum);

    pose->mpBufferGroupVisibility = testAlloc(sizeof(u8) * animPoseGetBaseData(pose)->mBufferVisibilityNum);
    pose->mpGroupVisibility = pose->mpBufferGroupVisibility;

    pose->mpBufferNode = testAlloc(sizeof(AnimPoseNode*) * animPoseGetBaseData(pose)->mBufferNodeNum);
    pose->mpNodes = testAlloc(sizeof(AnimPoseNode*) * animPoseGetBaseData(pose)->mBufferNodeNum);

    pose->mpBufferTexAnimEntries = testAlloc(sizeof(AnimTexMtx) * animPoseGetBaseData(pose)->mBufferTexAnimNum);
    pose->mpTexAnimEntries = pose->mpBufferTexAnimEntries;

    pose->lastFrameZero = -1;
}

// heavily inlined
void animPoseRefresh(void) {
    int i;

    initTestHeap(); // inline
    for (i = 0; i < wp->totalPoses; i++) {
        AnimationPose* pose = &wp->poses[i]; // inline
        if (pose->flags) {
            // TODO: cleanup in rewrite?
            if ((!wp->inBattle || pose->type != 0) && (wp->inBattle || pose->type != 1)) {
                animPose_AllocBuffer(pose);
                pose->lastFrameZero = -1; // TODO: redundant?
            }
        }
    }
}

void animPoseBattleInit(void) {
    wp->inBattle = TRUE;
    animPoseRefresh();
}

s32 animPoseEntry(const char *animName, s32 type)
{
  static s32 _dummy = -1;
  AnimationTextureFile *new_var3[2];
  AnimationPoseFile *file;
  AnimationPoseFile *tempfile;
  AnimationPose *pose;
  AnimationPose *temp;
  AnimationPoseData *data;
  s32 texId;
  s32 fileId;
  FileEntry *new_var2;
  s32 poseId;
  AnimationPoseFile *new_var;
  s32 i;
  char textureName[256];
  switch (type)
  {
    case 0:
      fileSetCurrentArchiveType(1);
      break;

    case 1:
      fileSetCurrentArchiveType(2);
      break;

    case 2:
      fileSetCurrentArchiveType(0);
      break;

  }

  for (poseId = 0, i = 0; i < wp->totalPoses; poseId++, i++)
  {
    temp = &wp->poses[poseId];
    if (!temp->flags)
    {
      pose = temp;
      goto label_1;
    }
  }

  pose = (void *) 0;
  poseId = -1;
  label_1:
  pose->flags |= 1;

  pose->typeFlags = 0;
  pose->references = 0;
  texId = wp->totalFiles;
  for (i = 0; i < texId; i++)
  {
    file = &wp->files[i];
    if (file->active)
    {
      data = *file->handle->data;
      if (!strcmp(data->fileName, animName))
      {
        file->references++;
        goto label_5;
      }
    }
  }

  for (i = 0, fileId = 0; i < wp->totalFiles; i++, fileId++)
  {
    tempfile = &wp->files[fileId];
    if (!tempfile->active)
    {
      file = tempfile;
      goto label_2;
    }
  }

  file = (void *) 0;
  fileId = -1;
  label_2:
  file->active = 1;

  file->references++;
  file->handle = fileAllocf(5, "%s/%s", "a", animName);
  new_var2 = file->handle;
  if (!file->handle)
  {
    fileId = -2;
    file->active = 0;
    file->references--;
  }
  else
  {
    data = *file->handle->data;
    if (!data)
    {
      fileFree(new_var2);
      fileId = -2;
      file->active = 0;
      new_var = file;
      new_var->references--;
    }
    else
    {
      sprintf(textureName, "%s/%s-", "a", data->textureName);
      texId = wp->totalTextures;
      for (i = 0; i < texId; i++)
      {
        new_var3[0] = &wp->textures[i];
        if (*((u32 *) (&new_var3[0]->active)))
        {
          if (new_var3[0]->handle)
          {
            if (!strcmp(new_var3[0]->handle->filename, textureName))
            {
              new_var3[0]->references++;
              goto label_4;
            }
          }
        }
      }

      for (i = 0, texId = 0; i < wp->totalTextures; i++, texId++)
      {
        new_var3[1] = &wp->textures[texId];
        if (!(*((u32 *) (&new_var3[1]->active))))
        {
          new_var3[0] = new_var3[1];
          goto label_3;
        }
      }

      new_var3[0] = (void *) 0;
      texId = -1;
      label_3:
      new_var3[0]->active = 1;

      new_var3[0]->references++;
      new_var3[0]->handle = fileAlloc(textureName, 4);
      label_4:
      new_var->textureId = texId;

    }
  }
  label_5:
  pose->fileId = fileId;

  if (pose->fileId == (-2))
  {
    pose->flags = 0;
    return -2;
  }
  pose->animId = 0;
  pose->localTime = animTimeGetTime(type);
  pose->lastFrameZero = -1;
  pose->lastFrameTime = 0.0f;
  pose->field_0x80 = 0;
  pose->loopTime = 0.0f;
  pose->field_0x70 = 0.0f;
  pose->rotationY = 0.0f;
  pose->field_0x7C = 0.0f;
  pose->type = type;
  pose->paperId = -1;
  animPose_AllocBuffer(pose);
  pose->gxCallback = (void *) 0;
  pose->disableDraw = 0;
  pose->materialFlag = 0;
  pose->materialLightFlag = 0;
  pose->materialColor = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
  pose->materialColor2 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
  pose->field_0xF8 = (pose->field_0xFC = 1.0f);
  pose->field_0x100 = 0.0f;
  pose->localTimeRate = 1.0f;
  PSMTXIdentity(pose->matrix);
  if (!strcmp(animName, "tst_vivi"))
  {
    pose->vivianType = 1;
    pose->vivianGroupNo = &vivihimoTailGroupNo[0];
  }
  else
    if (!strcmp(animName, "c_vivian"))
  {
    pose->vivianType = 1;
    pose->vivianGroupNo = &vivihimoTailGroupNo[1];
  }
  else
    if (!strcmp(animName, "c_maririn"))
  {
    pose->vivianType = 2;
    pose->vivianGroupNo = &vivihimoTailGroupNo[2];
  }
  else
    if (!strcmp(animName, "c_majyorin"))
  {
    pose->vivianType = 3;
    pose->vivianGroupNo = &vivihimoTailGroupNo[3];
  }
  else
  {
    pose->vivianType = 0;
    pose->vivianGroupNo = &_dummy;
  }
  pose->dispCallback = (void *) 0;
  pose->dispUserDataCallback = (void *) 0;
  return poseId;
}

s32 animPaperPoseEntry(const char* animName, s32 group) {
    s32 i;
    AnimationPose* pose;

    i = animPaperPoseGetId(animName, group);
    if (i >= 0) {
        pose = animPoseGetAnimPosePtr(i);
        pose->references++;
        return i;
    }
    i = animPoseEntry(animName, group);
    if (i == -2) {
        return i;
    }
    pose = animPoseGetAnimPosePtr(i);
    pose->typeFlags = 3;
    pose->references = 0;
    pose->flags |= 1;
    return i;
}

const char* animGetTexName(const char* name);

BOOL animEffectAsync(const char* animName, u32 group) {
    s32 model, anim;
    BOOL ret;

    switch (group) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }
    model = fileAsyncf(5, NULL, "%s/%s", "a", animName);
    anim = fileAsyncf(4, NULL, "%s/%s-", "a", animGetTexName(animName));
    if (model == -1) {
        ret = TRUE;
    } else if ((u32)model == 0) {
        ret = FALSE;
    } else if (anim == -1) {
        ret = TRUE;
    } else if ((u32)anim == 0) {
        ret = FALSE;
    } else {
        ret = TRUE;
    }
    return ret;
}

void animPosePeraOn(s32 poseId) {
    wp->poses[poseId].flags |= 8;
}

void animPosePeraOff(s32 poseId) {
    wp->poses[poseId].flags &= ~8;
}

void animPosePaperPeraOn(s32 poseId) {
    AnimationPose* dst = &wp->poses[wp->poses[poseId].paperId];
    AnimationPose* src = &wp->poses[poseId];

    dst->flags |= 8;
    dst->field_0x70 = src->field_0x70;
    dst->rotationY = src->rotationY;
    dst->field_0x7C = src->field_0x7C;
}

void animPosePaperPeraOff(s32 poseId) {
    wp->poses[wp->poses[poseId].paperId].flags &= ~8;
}

void animPoseSetLocalTimeRate(s32 poseId, f32 localTimeRate) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->localTimeRate = localTimeRate;
}

void animPoseSetLocalTime(s32 poseId, f32 localTimeFrames) {
    AnimationPose *pose, *pera;

    pose = &wp->poses[poseId];
    pose->localTime = animTimeGetTime(pose->type) - (u64)(16.666666f * localTimeFrames);
    pose->mLocalTimeInit = pose->localTime;
    if (pose->paperId != -1) {
        pera = &wp->poses[pose->paperId];
        if (!(pera->typeFlags & 2)) {
            pera->localTime = pose->localTime;
        }
    }
}

void animPoseSetStartTime(s32 poseId, OSTime startTime) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->localTime = startTime;
}

#define animPosePaperAnimId(pose) (*(s32*)(pose)->field_0x94)
#define animPoseGetBaseDataId(id) (*(AnimationPoseData**)(wp->files[wp->poses[id].fileId].handle->data))

inline s32 animPoseGetAnimIdx(AnimationPoseData* data, const char* name) {
    s32 count = data->mAnimCount;
    s32 i = (s32)name;

    if (i < 0) {
        for (i = 0; i < count; i++) {
            if (!strcmp(data->mpAnims[i].name, name)) {
                return i;
            }
        }
    }
    else if (i < count) {
        return i;
    }
    return 0;
}

void animPoseSetAnim(s32 poseId, const char* animName, BOOL reset) {
    AnimationPose* pose;
    s32 animId;

    pose = &wp->poses[poseId];
    animId = animPoseGetAnimIdx(animPoseGetBaseData(pose), animName);
    if (reset || animId != pose->animId) {
        pose->animId = animId;
        if (pose->paperId != -1 && pose->flags & 2) {
            pose->flags |= 4;
        }
        pose->localTime = animTimeGetTime(pose->type);
        pose->lastFrameZero = -1;
        pose->lastFrameTime = 0.0f;
        pose->field_0x80 = 0;
        pose->loopTime = 0.0f;
    }
}

s32 animPaperPoseGetId(const char* animName, s32 group) {
    AnimationPose* pose;
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if (pose->flags & 1 && pose->typeFlags & 1 && pose->typeFlags & 2 && pose->type == group &&
            !strcmp(animPoseGetBaseData(pose)->fileName, animName)) {
            break;
        }
    }
    if (i == wp->totalPoses) {
        i = -1;
    }
    return i;
}

void animPoseSetPaperAnimGroup(s32 poseId, const char* animName, BOOL someflag) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);

    if (!animName) {
        pose->paperId = -1;
        animPoseSetMaterialLightFlagOff(poseId, 1);
        return;
    }
    pose->paperId = animPaperPoseGetId(animName, pose->type);
    if (pose->paperId < 0) {
        pose->paperId = animPaperPoseGetId(animName, 2);
    }
    animPosePaperAnimId(pose) = -1;
    pose->mLocalTimeInit = animTimeGetTime(pose->type);
    if (someflag) {
        pose->flags |= 2;
        pose->flags |= 4;
    }
    else {
        pose->flags &= ~2;
        pose->flags &= ~4;
    }
    animPoseSetMaterialLightFlagOn(poseId, 1);
}

void animPoseSetPaperAnim(s32 poseId, const char* animName, BOOL reset) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    AnimationPoseData* data;
    s32 id;
    s32 idx;

    data = animPoseGetAnimBaseDataPtr(pose->paperId);
    idx = animPoseGetAnimIdx(data, animName);
    animPosePaperAnimId(pose) = idx;
    pose->mLocalTimeInit = animTimeGetTime(pose->type);
    id = pose->paperId;
    if (id != -1 && !(*(u32*)((u8*)wp->poses + id * sizeof(AnimationPose) + 4) & 2)) {
        animPoseMain(pose->paperId);
    }
}

void animPoseFileRelease(s32 fileId);

inline s32 animEffectPoseEntry(const char* animName, s32 group) {
    s32 i;
    AnimationPose* pose;

    i = animPoseEntry(animName, group);
    if (i == -2) {
        return i;
    }
    pose = animPoseGetAnimPosePtr(i);
    pose->typeFlags = 1;
    pose->references = 0;
    pose->flags |= 1;
    return i;
}

void animPoseSetEffect(s32 poseId, const char* animName, BOOL someflag) {
    AnimationPose* paper;
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);

    if (pose->paperId != -1) {
        if (animName && !strcmp((*(AnimationPoseData**)(((AnimationPoseFile*)((u8*)wp->files + animPoseGetAnimPosePtr(pose->paperId)->fileId * 16))->handle->data))->fileName, animName) &&
            (s32)((pose->flags >> 1) & 1) == someflag) {
            return;
        }
        paper = &wp->poses[pose->paperId];
        paper->references--;
        if (paper->references < 0) {
            animPoseFileRelease(paper->fileId);
            paper->flags = 0;
        }
    }
    if (!animName) {
        pose->paperId = -1;
        animPoseSetMaterialLightFlagOff(poseId, 1);
        return;
    }
    pose->paperId = animEffectPoseEntry(animName, pose->type);
    animPosePaperAnimId(pose) = -1;
    pose->mLocalTimeInit = animTimeGetTime(pose->type);
    if (someflag) {
        pose->flags |= 2;
        pose->flags |= 4;
    }
    else {
        pose->flags &= ~2;
    }
    animPoseSetMaterialLightFlagOn(poseId, 1);
}

void animPoseSetEffectAnim(s32 poseId, const char* animName, BOOL reset) {
    AnimationPose* pose = &wp->poses[poseId];

    if (pose->paperId != -1) {
        animPoseSetAnim(pose->paperId, animName, reset);
    }
}

void animPoseSetGXFunc(s32 poseId, void (*callback)(s32 wXluStage), BOOL disableDraw) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->gxCallback = callback;
    pose->disableDraw = disableDraw;
}

f32 animPoseGetRadius(s32 poseId) {
    return 2.0f * animPoseGetAnimBaseDataPtr(poseId)->radius;
}

f32 animPoseGetHeight(s32 poseId) {
    return 2.0f * animPoseGetAnimBaseDataPtr(poseId)->height;
}

f32 animPoseGetLoopTimes(s32 id) {
    AnimationPose* pose = &wp->poses[id];
    return pose->loopTime;
}

BOOL animPoseGetPeraEnd(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];

    if (!(pose->flags & 8)) {
        return TRUE;
    }
    if (__fabs(pose->field_0x7C - pose->rotationY) < 10.0) {
        return TRUE;
    }
    return FALSE;
}

void animPoseSetMaterialFlagOn(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialFlag |= mask;
}

void animPoseSetMaterialFlagOff(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialFlag &= ~mask;
}

void animPoseSetMaterialLightFlagOn(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialLightFlag |= mask;
}

void animPoseSetMaterialLightFlagOff(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialLightFlag &= ~mask;
}

void animPoseSetMaterialEvtColor(s32 poseId, GXColor color) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialColor = color;
}

u32 animPoseGetMaterialFlag(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return pose->materialFlag;
}

u32 animPoseGetMaterialLightFlag(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return pose->materialLightFlag;
}

GXColor animPoseGetMaterialEvtColor(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return pose->materialColor;
}

extern double atan2(double y, double x);
#define APM_U(p, o) (*(u32*)((u8*)(p) + (o)))
#define APM_S(p, o) (*(s32*)((u8*)(p) + (o)))
#define APM_F(p, o) (*(f32*)((u8*)(p) + (o)))
#define APM_P(p, o) (*(u8**)((u8*)(p) + (o)))
#define APM_S8(dst, p, off) asm { psq_l dst, off(p), 1, 4 }
void animPoseMain(s32 poseId) {
    u8* pose;
    u8* file;
    u8* anim;
    u8* poses;
    u8 *grpB, *grpA;
    u8* q;
    f32 key;
    u8* kf;
    u8* kfEnd;
    u8* v;
    register u8* d;
    u8* e;
    register f32 x, y, z;
    s32 loop, frame, nextFrame, cnt, i, b;
    s32 nA, nB, nC, nD, nE;
    f32 t, f, cur, start, len, tm, dt, s, t2, t3;
    s64 ms;
    struct APMG { u8 pad[8]; u8* p; u8 pad2[4]; };
    struct APMA { u8 pad[0x3c]; u8* p; };
    struct APMA* aa;
    struct APMK { s32 a; s32 b; };
    struct APMF { u8 pad[0x2c]; };
    struct APMD { u8 pad[0x28]; struct APMF* f; u8 pad2[0x40 - 0x2c]; struct APMK* k; };

    poses = APM_P(wp, 0x10);
    pose = poses + poseId * 0x170;
    file = (u8*)animPoseGetBaseData((AnimationPose*)pose);
    anim = (u8*)((AnimationPoseData*)file)->mpAnims[((AnimationPose*)pose)->animId].data;
    asm {
        li r3, 4
        oris r3, r3, 4
        mtspr GQR2, r3
        li r3, 5
        oris r3, r3, 5
        mtspr GQR3, r3
        li r3, 6
        oris r3, r3, 6
        mtspr GQR4, r3
        li r3, 7
        oris r3, r3, 7
        mtspr GQR5, r3
    }
    if (APM_S(wp, 0xe8) != 0) {
        grpA = APM_P(wp, 0xf8);
        grpB = APM_P(wp, 0xfc);
    } else {
        grpA = APM_P(pose, 0x4c);
        grpB = APM_P(pose, 0x54);
    }
    if (APM_S(pose, 0x90) != -1 && !(APM_U(poses + APM_S(pose, 0x90) * 0x170, 4) & 2)) {
        animPoseMain(APM_S(pose, 0x90));
    }
    if (anim == NULL) return;
    loop = *(s32*)APM_P(anim, 0x24);
    if (APM_S(pose, 0xc) == 0) {
        ms = OSTicksToMilliseconds(*(s64*)((u8*)gp + 0x40));
    } else {
        ms = OSTicksToMilliseconds(*(s64*)((u8*)gp + 0x38));
    }
    tm = (f32)(u64)(ms - *(s64*)(pose + 0x18));
    APM_F(pose, 0x20) = 60.0f * tm / 1000.0f;
    APM_F(pose, 0x20) *= APM_F(pose, 0x44);
    cur = APM_F(pose, 0x20);
    f = APM_F(APM_P(anim, 0x24), 8);
    start = APM_F(APM_P(anim, 0x24), 4);
    len = f - start;
    if (0.0f == len) {
        APM_F(pose, 0x84) = cur - start;
    } else if (loop != 0) {
        tm = cur - start;
        if (tm < 0.0f) {
            APM_F(pose, 0x84) = 0.0f;
        } else {
            APM_F(pose, 0x84) = tm / len;
            start = cur - (f32)(s32)APM_F(pose, 0x84) * len;
        }
    } else if (cur < start) {
        APM_F(pose, 0x84) = 0.0f;
    } else if (cur > f) {
        APM_F(pose, 0x84) = (cur - start) / len;
        start = f;
    } else {
        APM_F(pose, 0x84) = (cur - start) / len;
        start = cur;
    }
    APM_F(pose, 0x24) = start;
    key = APM_F(pose, 0x24);
    cnt = APM_S(anim, 8);
    q = APM_P(anim, 0x28);
    for (i = 0; i < cnt; i++, q += 0x2c) {
        if (*(f32*)q > key) break;
    }
    APM_S(pose, 0x28) = i - 1;
    APM_S(pose, 0x2c) = APM_S(pose, 0x28) + 1;
    key = APM_F(pose, 0x24);
    q = APM_P(anim, 0x40);
    cnt = APM_S(anim, 0x20);
    for (i = 0; i < cnt; q += 8, i++) {
        if (*(f32*)q > key) break;
    }
    APM_S(pose, 0x34) = i - 1;
    APM_S(pose, 0x38) = APM_S(pose, 0x34) + 1;
    if (loop == 0) {
        if (APM_S(pose, 0x2c) >= APM_S(anim, 8)) APM_S(pose, 0x2c) = APM_S(pose, 0x28);
        if (APM_S(pose, 0x38) >= APM_S(anim, 0x20)) APM_S(pose, 0x38) = APM_S(pose, 0x34);
    }
    APM_S(pose, 0x80) = ((struct APMD*)anim)->k[APM_S(pose, 0x34)].b;
    start = *(f32*)(APM_P(anim, 0x28) + APM_S(pose, 0x28) * 0x2c);
    tm = APM_F(pose, 0x24) - start;
    if (loop != 0) {
        if (APM_S(pose, 0x2c) >= APM_S(anim, 8)) {
            APM_S(pose, 0x2c) = 0;
            len = APM_F(APM_P(anim, 0x24), 8) - *(f32*)(APM_P(anim, 0x28) + APM_S(pose, 0x28) * 0x2c);
        } else {
            len = *(f32*)(APM_P(anim, 0x28) + APM_S(pose, 0x2c) * 0x2c) - start;
        }
    } else {
        len = *(f32*)(APM_P(anim, 0x28) + APM_S(pose, 0x2c) * 0x2c) - start;
    }
    if (0.0f == len) {
        APM_F(pose, 0x30) = 0.0f;
    } else {
        APM_F(pose, 0x30) = tm / len;
    }
    frame = APM_S(pose, 0x28);
    nextFrame = APM_S(pose, 0x2c);
    t = APM_F(pose, 0x30);
    nA = APM_S(file, 0xf0);
    nB = APM_S(file, 0xf8);
    nC = APM_S(file, 0x13c);
    nD = APM_S(file, 0x140);
    nE = APM_S(file, 0x12c);
    if (APM_S(pose, 0x3c) != frame) {
        if (APM_S(pose, 0x3c) == -1 || APM_S(pose, 0x3c) > frame) {
            kfEnd = (u8*)((struct APMD*)anim)->f + (frame * 0x2c + 0x2c);
            kf = (u8*)((struct APMD*)anim)->f;
            if (nA != 0) memcpy_as4(APM_P(pose, 0x48), APM_P(file, 0x154), nA * 0xc);
            if (nB != 0) memcpy_as4(APM_P(pose, 0x50), APM_P(file, 0x15c), nB * 0xc);
            if (nC != 0) memcpy(APM_P(pose, 0x58), APM_P(file, 0x1a0), nC);
            if (nD != 0) memcpy_as4(APM_P(pose, 0x60), APM_P(file, 0x1a4), nD * 4);
            if (nE != 0) memcpy_as4(APM_P(pose, 0x68), APM_P(file, 0x190), nE * 0x18);
            if (APM_S(pose, 0x3c) == -1) APM_S(pose, 0x3c) = 0;
            s = 0.0625f;
            for (; kf < kfEnd; kf += 0x2c) {
                v = APM_P(pose, 0x48);
                d = APM_P(anim, 0x2c) + APM_S(kf, 4) * 4;
                e = d + APM_S(kf, 8) * 4;
                for (; d < e; d += 4) {
                    v += d[0] * 0xc;
                    APM_S8(x, d, 1);
                    APM_S8(y, d, 2);
                    APM_S8(z, d, 3);
                    ((f32*)v)[0] += s * x;
                    ((f32*)v)[1] += s * y;
                    ((f32*)v)[2] += s * z;
                }
                v = APM_P(pose, 0x50);
                d = APM_P(anim, 0x30) + APM_S(kf, 0xc) * 4;
                e = d + APM_S(kf, 0x10) * 4;
                for (; d < e; d += 4) {
                    v += d[0] * 0xc;
                    APM_S8(x, d, 1);
                    APM_S8(y, d, 2);
                    APM_S8(z, d, 3);
                    ((f32*)v)[0] += s * x;
                    ((f32*)v)[1] += s * y;
                    ((f32*)v)[2] += s * z;
                }
                v = APM_P(pose, 0x58);
                d = APM_P(anim, 0x38) + APM_S(kf, 0x1c) * 2;
                e = d + APM_S(kf, 0x20) * 2;
                for (; d < e; d += 2) {
                    v += d[0];
                    v[0] += d[1];
                }
                v = APM_P(pose, 0x60);
                d = APM_P(anim, 0x3c) + APM_S(kf, 0x24) * 4;
                e = d + APM_S(kf, 0x28) * 4;
                for (; d < e; d += 4) {
                    v += d[0] * 4;
                    APM_S8(x, d, 1);
                    ((f32*)v)[0] += s * x;
                }
                v = APM_P(pose, 0x68);
                d = APM_P(anim, 0x34) + APM_S(kf, 0x14) * 0xc;
                e = d + APM_S(kf, 0x18) * 0xc;
                for (; d < e; d += 0xc) {
                    v += d[0] * 0x18;
                    v[0] += d[1];
                    APM_F(v, 4) += APM_F(d, 4);
                    APM_F(v, 8) += APM_F(d, 8);
                }
            }
        } else if (APM_S(pose, 0x3c) < frame) {
            kf = APM_P(anim, 0x28) + (APM_S(pose, 0x3c) + 1) * 0x2c;
            kfEnd = APM_P(anim, 0x28) + (frame + 1) * 0x2c;
            s = 0.0625f;
            for (; kf < kfEnd; kf += 0x2c) {
                v = APM_P(pose, 0x48);
                d = APM_P(anim, 0x2c) + APM_S(kf, 4) * 4;
                e = d + APM_S(kf, 8) * 4;
                for (; d < e; d += 4) {
                    v += d[0] * 0xc;
                    APM_S8(x, d, 1);
                    APM_S8(y, d, 2);
                    APM_S8(z, d, 3);
                    ((f32*)v)[0] += s * x;
                    ((f32*)v)[1] += s * y;
                    ((f32*)v)[2] += s * z;
                }
                v = APM_P(pose, 0x50);
                d = APM_P(anim, 0x30) + APM_S(kf, 0xc) * 4;
                e = d + APM_S(kf, 0x10) * 4;
                for (; d < e; d += 4) {
                    v += d[0] * 0xc;
                    APM_S8(x, d, 1);
                    APM_S8(y, d, 2);
                    APM_S8(z, d, 3);
                    ((f32*)v)[0] += s * x;
                    ((f32*)v)[1] += s * y;
                    ((f32*)v)[2] += s * z;
                }
                v = APM_P(pose, 0x58);
                d = APM_P(anim, 0x38) + APM_S(kf, 0x1c) * 2;
                e = d + APM_S(kf, 0x20) * 2;
                for (; d < e; d += 2) {
                    v += d[0];
                    v[0] += d[1];
                }
                v = APM_P(pose, 0x60);
                d = APM_P(anim, 0x3c) + APM_S(kf, 0x24) * 4;
                e = d + APM_S(kf, 0x28) * 4;
                for (; d < e; d += 4) {
                    v += d[0] * 4;
                    APM_S8(x, d, 1);
                    ((f32*)v)[0] += s * x;
                }
                v = APM_P(pose, 0x68);
                d = APM_P(anim, 0x34) + APM_S(kf, 0x14) * 0xc;
                e = d + APM_S(kf, 0x18) * 0xc;
                for (; d < e; d += 0xc) {
                    v += d[0] * 0x18;
                    v[0] += d[1];
                    APM_F(v, 4) += APM_F(d, 4);
                    APM_F(v, 8) += APM_F(d, 8);
                }
            }
        }
    } else {
        if (APM_F(pose, 0x40) > t) APM_F(pose, 0x40) = 0.0f;
    }
    if (frame == nextFrame) {
        if (nA != 0) memcpy_as4(grpA, APM_P(pose, 0x48), nA * 0xc);
        if (nB != 0) memcpy_as4(grpB, APM_P(pose, 0x50), nB * 0xc);
        if (nD != 0) memcpy_as4(APM_P(pose, 0x64), APM_P(pose, 0x60), nD * 4);
    } else if (frame < nextFrame) {
        kf = APM_P(anim, 0x28) + nextFrame * 0x2c;
        s = 0.0625f * t;
        v = grpA;
        d = APM_P(anim, 0x2c) + APM_S(kf, 4) * 4;
        e = d + APM_S(kf, 8) * 4;
        if (nA != 0) memcpy_as4(grpA, APM_P(pose, 0x48), nA * 0xc);
        for (; d < e; d += 4) {
            v += d[0] * 0xc;
            APM_S8(x, d, 1);
            APM_S8(y, d, 2);
            APM_S8(z, d, 3);
            ((f32*)v)[0] += x * s;
            ((f32*)v)[1] += y * s;
            ((f32*)v)[2] += z * s;
        }
        v = grpB;
        d = APM_P(anim, 0x30) + APM_S(kf, 0xc) * 4;
        e = d + APM_S(kf, 0x10) * 4;
        if (nB != 0) memcpy_as4(grpB, APM_P(pose, 0x50), nB * 0xc);
        for (; d < e; d += 4) {
            v += d[0] * 0xc;
            APM_S8(x, d, 1);
            APM_S8(y, d, 2);
            APM_S8(z, d, 3);
            ((f32*)v)[0] += x * s;
            ((f32*)v)[1] += y * s;
            ((f32*)v)[2] += z * s;
        }
        dt = *(f32*)(APM_P(anim, 0x28) + nextFrame * 0x2c) - *(f32*)(APM_P(anim, 0x28) + frame * 0x2c);
        d = APM_P(anim, 0x3c) + APM_S(kf, 0x24) * 4;
        e = d + APM_S(kf, 0x28) * 4;
        v = APM_P(pose, 0x64);
        if (nD != 0) memcpy_as4(v, APM_P(pose, 0x60), nD * 4);
        t2 = t * t;
        for (; d < e; d += 4) {
            b = (s8)d[3];
            v += d[0] * 4;
            if (b < 0x59 && b > -0x59) {
                APM_S8(x, d, 1);
                t3 = t * t2;
                ((f32*)v)[0] = ((f32*)v)[0] + ((t + (t3 - 2.0f * t2)) * (dt * tanfTbl[(s8)d[2] + 0x59]) +
                                (-2.0f * t3 + 3.0f * t2) * (0.0625f * x) +
                                (t3 - t2) * (dt * tanfTbl[b + 0x59]));
            }
        }
    }
    APM_S(pose, 0x3c) = frame;
    APM_F(pose, 0x40) = t;
    if (APM_U(pose, 0) & 8) {
        CameraEntry* cam = camGetPtr(CAMERA_3D);
        f = 57.29579f * (f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z));
        while (f < -360.0f) f += 360.0f;
        while (f >= 360.0f) f -= 360.0f;
        if (f < 0.0f) f += 360.0f;
        f -= APM_F(pose, 0x74);
        while (f < -360.0f) f += 360.0f;
        while (f >= 360.0f) f -= 360.0f;
        if ((f > 90.0f && f <= 270.0f) || (f < -90.0f && f >= -270.0f)) {
            APM_F(pose, 0x78) = -1.0f;
        } else {
            APM_F(pose, 0x78) = 1.0f;
        }
    }
}

void pushGXModelMtx_TransformNode__(AnimPoseNode* node) {
    Mtx trans, scale, rot, tmp;
    Mtx sc1, sc2, sc3;
    Mtx rc1, rc2, rc3;
    MtxPtr p;
    u32 flags;
    f32 sx, sy, sz;
    f32 rx, ry, rz;
    f32 rcx, rcy, rcz;
    f32 scx, scy, scz;
    f32 rtx, rty, rtz;
    f32 stx, sty, stz;
    f32 tx, ty, tz;

    flags = 0;
    tx = node->mTranslation.x;
    ty = node->mTranslation.y;
    tz = node->mTranslation.z;
    sx = node->mScale.x;
    sy = node->mScale.y;
    sz = node->mScale.z;
    rx = 2.0f * node->mRot1.x;
    ry = 2.0f * node->mRot1.y;
    rz = 2.0f * node->mRot1.z;
    rcx = node->mRotationCenter.x;
    rcy = node->mRotationCenter.y;
    rcz = node->mRotationCenter.z;
    scx = node->mScaleCenter.x;
    scy = node->mScaleCenter.y;
    scz = node->mScaleCenter.z;
    rtx = node->mRotationTranslation.x;
    rty = node->mRotationTranslation.y;
    rtz = node->mRotationTranslation.z;
    stx = node->mScaleTranslation.x;
    sty = node->mScaleTranslation.y;
    stz = node->mScaleTranslation.z;

    if (tx || ty || tz) {
        PSMTXTrans(trans, tx, ty, tz);
        flags |= 1;
    }
    if (sx != 1.0f || sy != 1.0f || sz != 1.0f) {
        PSMTXScale(scale, sx, sy, sz);
        flags |= 2;
    }
    if (scx != 0.0f || scy != 0.0f || scz != 0.0f) {
        if (flags & 2) {
            PSMTXTrans(sc2, -scx, -scy, -scz);
            PSMTXTrans(sc1, scx, scy, scz);
            PSMTXTrans(sc3, stx, sty, stz);
            PSMTXConcat(sc3, sc1, tmp);
            PSMTXConcat(tmp, scale, tmp);
            PSMTXConcat(tmp, sc2, scale);
        }
        else if (stx || sty || stz) {
            PSMTXTrans(scale, stx, sty, stz);
            flags |= 2;
        }
    }
    PSMTXIdentity(rot);
    if (rz) {
        PSMTXRotRad(tmp, 'z', 0.017453292f * rz);
        PSMTXConcat(rot, tmp, rot);
        flags |= 4;
    }
    if (ry) {
        PSMTXRotRad(tmp, 'y', 0.017453292f * ry);
        PSMTXConcat(rot, tmp, rot);
        flags |= 4;
    }
    if (rx) {
        PSMTXRotRad(tmp, 'x', 0.017453292f * rx);
        PSMTXConcat(rot, tmp, rot);
        flags |= 4;
    }
    if (rcx != 0.0f || rcy != 0.0f || rcz != 0.0f) {
        if (flags & 4) {
            PSMTXTrans(rc2, -rcx, -rcy, -rcz);
            PSMTXTrans(rc1, rcx, rcy, rcz);
            PSMTXTrans(rc3, rtx, rty, rtz);
            PSMTXConcat(rc3, rc1, tmp);
            PSMTXConcat(tmp, rot, rot);
            PSMTXConcat(rot, rc2, rot);
        }
        else if (rtx || rty || rtz) {
            PSMTXTrans(rot, rtx, rty, rtz);
            flags |= 4;
        }
    }
    g_modeling_mtx_lv++;
    g_modeling_mtx = &g_modeling_mtx_stack[g_modeling_mtx_lv];
    switch (flags) {
        case 0:
            PSMTXIdentity(tmp);
            p = tmp;
            break;
        case 1:
            p = trans;
            break;
        case 2:
            p = scale;
            break;
        case 3:
            PSMTXConcat(trans, scale, tmp);
            p = tmp;
            break;
        case 4:
            p = rot;
            break;
        case 5:
            PSMTXConcat(trans, rot, tmp);
            p = tmp;
            break;
        case 6:
            PSMTXConcat(rot, scale, tmp);
            p = tmp;
            break;
        case 7:
            PSMTXConcat(trans, rot, tmp);
            PSMTXConcat(tmp, scale, trans);
            p = trans;
            break;
    }
    PSMTXConcat(g_modeling_mtx_stack[g_modeling_mtx_lv - 1], p, g_modeling_mtx_stack[g_modeling_mtx_lv]);
}
void pushGXModelMtx_JointNode__(AnimPoseNode* node, f32 sx, f32 sy, f32 sz) {
    Mtx trans, scale, iscale, rot, tmp;
    MtxPtr p;
    u32 flags;
    f32 tx, ty, tz;
    f32 nsx, nsy, nsz;
    f32 r1x, r1y, r1z;
    f32 r2x, r2y, r2z;

    tx = node->mTranslation.x;
    ty = node->mTranslation.y;
    tz = node->mTranslation.z;
    nsx = node->mScale.x;
    nsy = node->mScale.y;
    nsz = node->mScale.z;
    r1x = 2.0f * node->mRot1.x;
    r1y = 2.0f * node->mRot1.y;
    r1z = 2.0f * node->mRot1.z;
    r2x = node->mRot2.x;
    r2y = node->mRot2.y;
    r2z = node->mRot2.z;
    flags = 0;

    if (tx || ty || tz) {
        PSMTXTrans(trans, tx, ty, tz);
        flags |= 1;
    }
    if (1.0f != sx || 1.0f != sy || 1.0f == sz) {
        if (sx && sy && sz) {
            PSMTXScale(iscale, 1.0f / sx, 1.0f / sy, 1.0f / sz);
        } else {
            PSMTXScale(iscale, 1.0f, 1.0f, 1.0f);
        }
        flags |= 2;
    }
    if (1.0f != nsx || 1.0f != nsy || 1.0f == nsz) {
        PSMTXScale(scale, nsx, nsy, nsz);
        flags |= 4;
    }
    PSMTXIdentity(rot);
    if (r2z) {
        PSMTXRotRad(tmp, 'z', 0.017453292f * r2z);
        PSMTXConcat(rot, tmp, rot);
        flags |= 8;
    }
    if (r2y) {
        PSMTXRotRad(tmp, 'y', 0.017453292f * r2y);
        PSMTXConcat(rot, tmp, rot);
        flags |= 8;
    }
    if (r2x) {
        PSMTXRotRad(tmp, 'x', 0.017453292f * r2x);
        PSMTXConcat(rot, tmp, rot);
        flags |= 8;
    }
    if (r1z) {
        PSMTXRotRad(tmp, 'z', 0.017453292f * r1z);
        PSMTXConcat(rot, tmp, rot);
        flags |= 8;
    }
    if (r1y) {
        PSMTXRotRad(tmp, 'y', 0.017453292f * r1y);
        PSMTXConcat(rot, tmp, rot);
        flags |= 8;
    }
    if (r1x) {
        PSMTXRotRad(tmp, 'x', 0.017453292f * r1x);
        PSMTXConcat(rot, tmp, rot);
        flags |= 8;
    }
    g_modeling_mtx_lv++;
    g_modeling_mtx = &g_modeling_mtx_stack[g_modeling_mtx_lv];
    switch (flags) {
        case 0: PSMTXIdentity(tmp); p = tmp; break;
        case 1: p = trans; break;
        case 2: p = iscale; break;
        case 3: PSMTXConcat(trans, iscale, tmp); p = tmp; break;
        case 4: p = scale; break;
        case 5: PSMTXConcat(trans, scale, tmp); p = tmp; break;
        case 6: PSMTXConcat(iscale, scale, tmp); p = tmp; break;
        case 7: PSMTXConcat(trans, iscale, tmp); PSMTXConcat(tmp, scale, trans); p = trans; break;
        case 8: p = rot; break;
        case 9: PSMTXConcat(trans, rot, tmp); p = tmp; break;
        case 10: PSMTXConcat(iscale, rot, tmp); p = tmp; break;
        case 11: PSMTXConcat(trans, iscale, tmp); PSMTXConcat(tmp, rot, trans); p = trans; break;
        case 12: PSMTXConcat(rot, scale, tmp); p = tmp; break;
        case 13: PSMTXConcat(trans, rot, tmp); PSMTXConcat(tmp, scale, trans); p = trans; break;
        case 14: PSMTXConcat(iscale, rot, tmp); PSMTXConcat(tmp, scale, trans); p = trans; break;
        case 15: PSMTXConcat(trans, iscale, tmp); PSMTXConcat(tmp, rot, trans); PSMTXConcat(trans, scale, tmp); p = tmp; break;
    }
    PSMTXConcat(g_modeling_mtx_stack[g_modeling_mtx_lv - 1], p, g_modeling_mtx_stack[g_modeling_mtx_lv]);
}

#include "sdk/texPalette.h"

void animSetMaterial_Texture(s32 texCount, s32 *texIdRemap, AnimPoseTexEntry *pTexEntries, AnimTexMtx *animEntries, s32 texBindsCapacity, AnimTexBind *texBinds, AnimationTextureFile *texFile)
{
  int i;
  s32 bindId;
  s32 wrapFlags;
  u32 texId;
  AnimPoseTexEntry *entry;
  TEXDescriptor *desc;
  TEXHeader *hdr;
  s32 wrapS;
  s32 wrapT;
  u32 mipmap;
  GXTlutObj tlut;
  u16 *new_var;
  GXTexObj obj;
  Mtx rot;
  Mtx trans;
  Mtx scale;
  Mtx tmp;
  MtxPtr m;
  AnimTexMtx *anim;
  f32 scaleS;
  f32 scaleT;
  f32 transS;
  f32 transT;
  f32 rotate;
  u32 flags;
  u32 texMtx;
  u32 j;
  for (i = 0; i < texCount; i++)
  {
    s32 id = texIdRemap[(u32) ((texCount - i) - 1)];
    entry = &pTexEntries[id];
    bindId = entry->bindId + animEntries[id].mTexBindIdx;
    if ((bindId < 0) || (bindId >= texBindsCapacity))
    {
      bindId = 0;
            printf("Error: dispSetTevMode: \203x\201[\203X\203\202\203f\203\213%s \203A\203j\203\201 %s \223\340\202\305 \203t\203\214\201[\203\200\203G\203N\203X\203e\203\223\203V\203\207\203\223\202\314\220\335\222\350\202\252\215s\202\355\202\352\202\304\202\242\202\334\202\267\202\252\201A\212Y\223\226\202\267\202\351\203e\203N\203X\203`\203\203\202\252\202\240\202\350\202\334\202\271\202\361 \nframeExt\224\324\215\206 %d? \202\252\226\263\202\242\202\346\202\244\202\305\202\267\201B\203\202\203f\203\213\202\334\202\275\202\315\203A\203j\203\201\202\311\217C\220\263\202\252\225K\227v\202\305\202\267\201BtextureIdx=%d\n",
                   animPoseGetBaseData(wp->mpCurPose)->fileName,
                   animPoseGetBaseData(wp->mpCurPose)->mpAnims[wp->mpCurPose->animId].name);
    }
    wrapFlags = entry->wrapFlags;
    texId = *((u32 *) (((u8 *) (&texBinds[bindId])) + 4));
    if ((*((s32 *) (((u8 *) (&texBinds[bindId])) + 8))) != 0)
    {
      continue;
    }
    if (TEXGet(*((TEXPalette **) texFile->handle->data), texId)->CLUTHeader)
    {
      desc = TEXGet(*((TEXPalette **) texFile->handle->data), texId);
      hdr = desc->textureHeader;
      wrapS = hdr->wrapS;
      wrapT = hdr->wrapT;
      if (wrapFlags >= 0)
      {
        if (wrapFlags & 4)
        {
          wrapS = GX_MIRROR;
        }
        else
        {
          wrapS = wrapFlags;
          wrapS = wrapS & 1;
        }
        if (wrapFlags & 8)
        {
          wrapT = GX_MIRROR;
        }
        else
        {
          wrapT = (wrapFlags >> 1) & 1;
        }
      }
      mipmap = hdr->minLOD != hdr->maxLOD;
      new_var = &desc->textureHeader->width;
      GXInitTlutObj(&tlut, desc->CLUTHeader->data, desc->CLUTHeader->format, desc->CLUTHeader->numEntries);
      GXInitTexObjCI(&obj, desc->textureHeader->data, *new_var, desc->textureHeader->height, (GXCITexFmt) desc->textureHeader->format, wrapS, wrapT, mipmap, 0);
      hdr = desc->textureHeader;
      GXInitTexObjLOD(&obj, hdr->minFilter, hdr->magFilter, hdr->minLOD, hdr->maxLOD, hdr->LODBias, (GXBool) 0, hdr->edgeLODEnable, GX_ANISO_1);
      GXLoadTlut(&tlut, 0);
      GXLoadTexObj(&obj, (GXTexMapID) i);
    }
    else
    {
      desc = TEXGet(*((TEXPalette **) texFile->handle->data), texId);
      hdr = desc->textureHeader;
      wrapS = hdr->wrapS;
      wrapT = hdr->wrapT;
      if (wrapFlags >= 0)
      {
        if (wrapFlags & 4)
        {
          wrapS = GX_MIRROR;
        }
        else
        {
          wrapS = wrapFlags;
          wrapS = wrapS & 1;
        }
        if (wrapFlags & 8)
        {
          wrapT = GX_MIRROR;
        }
        else
        {
          wrapT = (wrapFlags >> 1) & 1;
        }
      }
      GXInitTexObj(&obj, hdr->data, hdr->width, hdr->height, (GXTexFmt) hdr->format, wrapS, wrapT, hdr->minLOD != hdr->maxLOD);
      hdr = desc->textureHeader;
      GXInitTexObjLOD(&obj, hdr->minFilter, hdr->magFilter, hdr->minLOD, hdr->maxLOD, hdr->LODBias, (GXBool) 0, hdr->edgeLODEnable, GX_ANISO_1);
      GXLoadTexObj(&obj, (GXTexMapID) i);
    }
  }

  for (i = 0; i < texCount; i++)
  {
    flags = 0;
    anim = &animEntries[texIdRemap[(u32) ((texCount - i) - 1)]];
    rotate = anim->mRotate;
    scaleS = anim->mScaleS;
    scaleT = anim->mScaleT;
    transS = anim->mTransS;
    transT = anim->mTransT;
    if (rotate != 0.0f)
    {
      flags |= 1;
      PSMTXTrans(tmp, 0.5f, 0.5f, 0.0f);
      PSMTXRotRad(rot, 'z', 0.017453292f * (-rotate));
      PSMTXConcat(tmp, rot, trans);
      PSMTXTrans(tmp, -0.5f, -0.5f, 0.0f);
      PSMTXConcat(trans, tmp, rot);
    }
    if (((transS != 0.0f) || (transT != 0.0f)) || (scaleT != 0.0f))
    {
      flags |= 2;
      PSMTXTrans(trans, transS, (1.0f - transT) - scaleT, 0.0f);
    }
    if ((scaleS != 1.0f) || (scaleT != 1.0f))
    {
      flags |= 4;
      PSMTXScale(scale, scaleS, scaleT, 1.0f);
    }
    switch (flags)
    {
      case 0:
        GXSetTexCoordGen2((GXTexCoordID) i, GX_TG_MTX2x4, (GXTexGenSrc) (GX_TG_TEX0 + i), GX_IDENTITY, (GXBool) 0, GX_PTIDENTITY);
        continue;

      case 1:
        m = rot;
        break;

      case 2:
        m = trans;
        break;

      case 3:
        PSMTXConcat(rot, trans, tmp);
        m = tmp;
        break;

      case 4:
        m = scale;
        break;

      case 5:
        PSMTXConcat(rot, scale, tmp);
        m = tmp;
        break;

      case 6:
        PSMTXConcat(trans, scale, tmp);
        m = tmp;
        break;

      case 7:
        PSMTXConcat(rot, trans, tmp);
        PSMTXConcat(tmp, scale, trans);
        m = trans;
        break;

    }

    j = i * 3;
    texMtx = GX_TEXMTX0 + j;
    GXSetTexCoordGen2((GXTexCoordID) i, GX_TG_MTX2x4, (GXTexGenSrc) (GX_TG_TEX0 + i), texMtx, (GXBool) 0, GX_PTIDENTITY);
    GXLoadTexMtxImm(m, texMtx, GX_MTX2x4);
  }

}

inline BOOL animSetMaterial_ChangeTexture(AnimationPoseData* data, AnimPoseShapeDraw* draw, AnimationPose* pose, GXTexObj* texObj, void** texData, Mtx mtx, s32 texType) {
    AnimTexBind* bind;
    s32 bindCount;
    AnimTexMtx* texAnim;
    AnimPoseTexEntry* texEntry;
    s32 texCount;
    s32 dataId, bindId, frameExt;
    GXTexObj* smartObj;
    int i;

    bind = data->texBinds;
    bindCount = data->texBindCount;
    texAnim = pose->mpTexAnimEntries;
    texEntry = data->texData;
    texCount = draw->texCount;

    for (i = 0; i < texCount; i++) {
        dataId = texCount - i - 1;
        dataId = draw->texDataIds[dataId];
        frameExt = texAnim[dataId].mTexBindIdx;
        bindId = texEntry[dataId].bindId + frameExt;
        if (bindId < 0 || bindId >= bindCount) {
            bindId = 0;
            //"Error: animSetMaterial_ChangeTexture: テクスチャがありません frameExt=%d\n"
            printf("Error: animSetMaterial_ChangeTexture: "
                   "\x83\x65\x83\x4E\x83\x58\x83\x60\x83\x83\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1 "
                   "frameExt=%d\n",
                   frameExt);
        }
        if (bind[bindId].texType == texType) {
            if (texObj) {
                smartObj = smartTexObj(texObj, texData);
                GXLoadTexObj(smartObj, i);
                if (mtx) {
                    GXLoadTexMtxImm(mtx, GX_TEXMTX0 + (u32)(i * 3), GX_MTX2x4);
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_TEXMTX0 + (u32)(i * 3));
                } else {
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_IDENTITY);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

static s32 tbl[4] = { 2, 1, 3, 0 };

void materialProc(s32 shapeId, s32 xluStage)
{
  int i;
  s32 drawCount;
  AnimPoseShapeDraw *draw;
  CameraEntry *camera;
  AnimationPose *pose;
  GXTexObj *texObj;
  GXTexObj *currTexObj;
  BOOL texChange1;
  BOOL check1;
  s32 dispMode;
  AnimPoseShape *shape;
  AnimationPoseData *new_var;
  Mtx camMtx;
  Mtx invCamMtx;
  Vec lightPos;
  Vec lightPos2;
  if (shapeId != (-1))
  {
    camera = camGetCurPtr();
    PSMTXConcat(camera->view, *g_modeling_mtx, camMtx);
    GXLoadPosMtxImm(camMtx, 0);
    PSMTXInvXpose(camMtx, invCamMtx);
    GXLoadNrmMtxImm(invCamMtx, 0);
    GXSetCurrentMtx(0);
    shape = &wp->mpCurPoseData->shapes[shapeId];
    dispMode = shape->dispMode;
    drawCount = shape->shapeDrawCount;
    draw = &wp->mpCurPoseData->draws[shape->shapeDrawStart];
    if (((((dispMode == 0) && (wp->currDispMode == 2)) || (dispMode == wp->currDispMode)) || (wp->mpCurPose->materialColor.a != 0xFF)) || (wp->mpCurPose->materialColor2.a != 0xFF))
    {
      GXSetCullMode(tbl[shape->cullMode]);
      for (i = 0; i < drawCount; i++, draw++)
      {
        s32 texCount = draw->texCount;
        if (((u32) wp->mpCurPose->disableDraw) == 0)
        {
          lightPos = (Vec){0.0f, 0.0f, 0.0f};
          PSMTXMultVec(*g_modeling_mtx, &lightPos, &lightPos);
          lightPos2 = lightPos;
          mapSetMaterialLight(wp->mpCurPose->materialLightFlag, lightPos2);
          animSetMaterial_Texture(draw->texCount, draw->texDataIds, wp->mpCurPoseData->texData, wp->mpCurPose->mpTexAnimEntries, wp->mpCurPoseData->texBindCount, wp->mpCurPoseData->texBinds, &wp->textures[wp->files[wp->mpCurPose->fileId].textureId]);
          mapSetMaterialLastStageBlend(wp->mpCurPose->materialFlag, wp->mpCurPose->materialColor, wp->mpCurPose->materialColor2);
          camera = camGetPtr(CAMERA_3D);
          if (camGetCurPtr() == camera)
          {
            mapSetMaterialFog();
          }
          mapSetMaterialTev(texCount, draw->drawMode, wp->mpCurPose->materialFlag, *g_modeling_mtx);
        }
        pose = wp->mpCurPose;
        if (((u32) pose->typeFlags) != 0)
        {
          texChange1 = animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, wp->mpTexObjs[0], wp->mpTexDatas[0], *wp->paperMtxPtr[0], 1);
          check1 = texChange1;
          if (!texChange1)
          {
            check1 = animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, wp->mpTexObjs[1], wp->mpTexDatas[1], *wp->paperMtxPtr[1], 2);
          }
          if (check1 && (!draw->drawMode))
          {
            currTexObj = wp->mpCurTexObj;
            if (currTexObj)
            {
              smartTexObj(currTexObj, wp->mpCurTexData);
              if (wp->currDispMode != 3)
              {
                GXSetZMode((GXBool) 1, GX_LEQUAL, (GXBool) 1);
                GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                GXSetZCompLoc((GXBool) 0);
                GXSetAlphaCompare(GX_GEQUAL, 0x80u, GX_AOP_OR, GX_NEVER, 0);
              }
              texObj = wp->mpTexObjs[texChange1 == 0];
              if (texObj)
              {
                GXLoadTexObj(texObj, GX_TEXMAP0);
                GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, (GXBool) 0, GX_PTIDENTITY);
                GXLoadTexMtxImm(*wp->paperMtxPtr[!texChange1], GX_TEXMTX0, GX_MTX2x4);
              }
              GXLoadTexObj(currTexObj, GX_TEXMAP1);
              GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, (GXBool) 0, GX_PTIDENTITY);
              GXLoadTexMtxImm(*wp->paperMtxPtr[2], GX_TEXMTX1, GX_MTX2x4);
              mapSetMaterialTev(2, 12, wp->mpCurPose->materialFlag, *g_modeling_mtx);
            }
          }
        }
        pose = wp->mpCurPose;
        if (pose->flags & 0x10)
        {
          texObj = wp->mpTexObjs[0];
          if (texObj)
          {
            Mtx *mtx = wp->paperMtxPtr[0];
            if (mtx)
            {
              new_var = wp->mpCurPoseData;
              animSetMaterial_ChangeTexture(new_var, draw, pose, texObj, wp->mpTexDatas[0], *mtx, 1);
            }
          }
        }
      }

    }
  }
}


// I'm so sorry, whoever gets tasked with matching this function
// note: I detect at least 8 inlined functions here, good luck finding the separation
void renderProc(s32 shapeId, s32 unused) {
    int i;
    CameraEntry* camera;
    AnimationPoseData* data;
    AnimPoseShape *shapes, *shape;
    AnimPoseShapeDraw *draws, *draw;
    s32 dispMode, drawId;
    s32 posFirstVtx, nrmFirstVtx, clr0FirstVtx;

    f32* buffer;
    s32 nrmVtxSize;
    s32 texCount;
    AnimationPose* pose;
    Mtx* mtx;
    GXTexObj* texObj;
    BOOL texChange1, check1, texChange2, check2;
    GXTexObj* currTexObj;
    s8* texIds;
    s32* firstId;
    AnimDrawCallEntry* drawCall;
    s32 drawCallCount;
    GXColor* clr0VtxPtr;
    s32 posFirstId, nrmFirstId, clr0FirstId;
    Vec *nrmVtxPtr, *posVtxPtr;

    s32 count;
    u16 *posIndexPtr, *nrmIndexPtr, *clr0IndexPtr, *texIndexPtr;
    s32 firstDrawId;
    s32 index;
    s32 texIndex;

    int j, k, l;

    // stack variables
    s32 clr0VtxSize, posVtxSize; // TODO: make sure stack, +0xB0, +0xAC
    s32 drawCount;               // TODO: make sure stack, +0xA8
    Mtx camMtx, invCamMtx;
    f32* texVtxIndices[8];
    Vec lightPos, lightPos2; // TODO: rename? 2 is probably implicit stack copy

    if (shapeId != -1) {
        camera = camGetCurPtr();
        MTXConcat(camera->view, *g_modeling_mtx, camMtx);
        GXLoadPosMtxImm(camMtx, 0);
        MTXInvXpose(camMtx, invCamMtx);
        GXLoadNrmMtxImm(invCamMtx, 0);
        GXSetCurrentMtx(0);

        data = wp->mpCurPoseData;
        shapes = data->shapes;
        draws = data->draws;
        shape = &shapes[shapeId];
        dispMode = shape->dispMode;
        drawId = shape->shapeDrawStart;
        posFirstVtx = shape->vtxPos.first;
        nrmFirstVtx = shape->vtxNrm.first;
        clr0FirstVtx = shape->vtxClr0.first;
        drawCount = shape->shapeDrawCount;
        draw = &draws[drawId];

        if ((dispMode == 0 && wp->currDispMode == 2) || dispMode == wp->currDispMode ||
            wp->mpCurPose->materialColor.a != 0xFF || wp->mpCurPose->materialColor2.a != 0xFF) {
            GXSetCullMode(tbl[shape->cullMode]);
            data = wp->mpCurPoseData;
            clr0VtxSize = (s32)sizeof(GXColor) * clr0FirstVtx;
            buffer = data->texVtxBuffer;
            texVtxIndices[0] = &buffer[2 * shape->vtxTex[0].first];
            texVtxIndices[1] = &buffer[2 * shape->vtxTex[1].first];
            texVtxIndices[2] = &buffer[2 * shape->vtxTex[2].first];
            texVtxIndices[3] = &buffer[2 * shape->vtxTex[3].first];
            texVtxIndices[4] = &buffer[2 * shape->vtxTex[4].first];
            texVtxIndices[5] = &buffer[2 * shape->vtxTex[5].first];
            texVtxIndices[6] = &buffer[2 * shape->vtxTex[6].first];
            texVtxIndices[7] = &buffer[2 * shape->vtxTex[7].first];
            nrmVtxSize = (s32)sizeof(Vec) * nrmFirstVtx;
            posVtxSize = (s32)sizeof(Vec) * posFirstVtx;

            for (i = 0; i < drawCount; i++, draw++) {
                texCount = draw->texCount;
                if ((u32)wp->mpCurPose->disableDraw == 0) {
                    lightPos = (Vec){ 0.0f, 0.0f, 0.0f };
                    MTXMultVec(*g_modeling_mtx, &lightPos, &lightPos);
                    lightPos2 = lightPos;
                    mapSetMaterialLight(wp->mpCurPose->materialLightFlag, lightPos2);
                    animSetMaterial_Texture(draw->texCount, draw->texDataIds, wp->mpCurPoseData->texData,
                                            wp->mpCurPose->mpTexAnimEntries, wp->mpCurPoseData->texBindCount,
                                            wp->mpCurPoseData->texBinds,
                                            &wp->textures[wp->files[wp->mpCurPose->fileId].textureId]);
                    mapSetMaterialLastStageBlend(wp->mpCurPose->materialFlag, wp->mpCurPose->materialColor,
                                                 wp->mpCurPose->materialColor2);
                    camera = camGetPtr(CAMERA_3D);
                    if (camGetCurPtr() == camera) {
                        mapSetMaterialFog();
                    }
                    mapSetMaterialTev(texCount, draw->drawMode, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                }
                pose = wp->mpCurPose;
                if ((u32)pose->typeFlags != 0) {
                    texChange1 = animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, wp->mpTexObjs[0], wp->mpTexDatas[0], *wp->paperMtxPtr[0], 1);
                    check1 = texChange1;
                    if (!texChange1) {
                        texChange2 = animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, wp->mpTexObjs[1], wp->mpTexDatas[1], *wp->paperMtxPtr[1], 2);
                        check1 = texChange2;
                        check2 = texChange2;
                    } else {
                        check2 = FALSE;
                    }

                    if (check1 && (texChange1 && (s32)wp->field_0xD8 == 0 || check2 && (s32)wp->field_0xDC == 0) && !draw->drawMode) {
                        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
                    }
                    if (check1 && !draw->drawMode) {
                        currTexObj = wp->mpCurTexObj;
                        if (currTexObj) {
                            smartTexObj(currTexObj, wp->mpCurTexData);
                            if (wp->currDispMode != 3) {
                                GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
                                GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                                GXSetZCompLoc(GX_FALSE);
                                GXSetAlphaCompare(GX_GEQUAL, 0x80u, GX_AOP_OR, GX_NEVER, 0);
                            }
                            texObj = wp->mpTexObjs[texChange1 == 0];
                            if (texObj) {
                                GXLoadTexObj(texObj, GX_TEXMAP0);
                                GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE,
                                                  GX_PTIDENTITY);
                                GXLoadTexMtxImm(*wp->paperMtxPtr[!texChange1], GX_TEXMTX0, GX_MTX2x4);
                            }
                            GXLoadTexObj(currTexObj, GX_TEXMAP1);
                            GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE,
                                              GX_PTIDENTITY);
                            GXLoadTexMtxImm(*wp->paperMtxPtr[2], GX_TEXMTX1, GX_MTX2x4);
                            mapSetMaterialTev(2, 12, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                        }
                    }
                }

                pose = wp->mpCurPose;
                if (pose->flags & 0x10) {
                    texObj = wp->mpTexObjs[0];
                    if (texObj) {
                        mtx = wp->paperMtxPtr[0];
                        if (mtx) {
                            animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, texObj, wp->mpTexDatas[0], *mtx, 1);
                        }
                    }
                }

                texIds = (s8*)draw->texArrayIds;
                firstId = draw->firstIdTex;
                data = wp->mpCurPoseData; // inlined function here
                drawCall = &data->drawCalls[draw->firstDrawCall];
                texCount = draw->texCount;
                drawCallCount = draw->drawCallCount;
                clr0VtxPtr = (GXColor*)((u8*)data->clr0VtxBuffer + clr0VtxSize);
                posFirstId = draw->firstIdPos;
                nrmFirstId = draw->firstIdNrm;
                clr0FirstId = draw->firstIdClr0;

                if ((s32)wp->mbUseFloatScratch != 0) {
                    nrmVtxPtr = (Vec*)((u8*)wp->vtxArrayNrm + nrmVtxSize);
                    posVtxPtr = (Vec*)((u8*)wp->vtxArrayPos + posVtxSize);
                } else {
                    pose = wp->mpCurPose;
                    nrmVtxPtr = (Vec*)((u8*)pose->mpVtxArrayNrm + nrmVtxSize);
                    posVtxPtr = (Vec*)((u8*)pose->mpVtxArrayPos + posVtxSize);
                }
                GXClearVtxDesc();
                GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
                GXSetArray(GX_VA_POS, posVtxPtr, sizeof(Vec));
                GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
                GXSetArray(GX_VA_NRM, nrmVtxPtr, sizeof(Vec));
                GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
                GXSetArray(GX_VA_CLR0, clr0VtxPtr, sizeof(GXColor));

                if (texCount == 1) { // just use texIndex[0]
                    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
                    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
                    GXSetArray(GX_VA_TEX0, texVtxIndices[texIds[0]], sizeof(f32) * 2);
                    for (j = 0; j < drawCallCount; j++, drawCall++) {
                        firstDrawId = drawCall->first;
                        data = wp->mpCurPoseData;
                        count = (u16)drawCall->count;
                        posIndexPtr = (u16*)((u32*)data->posIndex + (firstDrawId + posFirstId)) + 1;
                        nrmIndexPtr = (u16*)((u32*)data->nrmIndex + (firstDrawId + nrmFirstId)) + 1;
                        clr0IndexPtr = (u16*)((u32*)data->clr0Index + (firstDrawId + clr0FirstId)) + 1;
                        texIndexPtr = (u16*)((u32*)data->texIndex[texIds[0]] + (firstDrawId + firstId[texIds[0]])) + 1;
                        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, count);
                        for (k = count; k > 0; k--) {
                            GXPosition1x16(*posIndexPtr);
                            posIndexPtr += 2;
                            GXNormal1x16(*nrmIndexPtr);
                            nrmIndexPtr += 2;
                            GXColor1x16(*clr0IndexPtr);
                            clr0IndexPtr += 2;
                            GXTexCoord1x16(*texIndexPtr);
                            texIndexPtr += 2;
                        }
                    }
                } else { // enumerate texIndex
                    for (j = 0; j < texCount; j++) {
                        GXSetVtxDesc(GX_VA_TEX0 + j, GX_INDEX16);
                        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + j, GX_TEX_ST, GX_F32, 0);
                        GXSetArray(GX_VA_TEX0 + j, texVtxIndices[texIds[texCount - j - 1]], sizeof(f32) * 2);
                    }
                    for (j = 0; j < drawCallCount; j++, drawCall++) {
                        firstDrawId = drawCall->first;
                        data = wp->mpCurPoseData;
                        count = (u16)drawCall->count;
                        posIndexPtr = (u16*)((u32*)data->posIndex + (firstDrawId + posFirstId)) + 1;
                        nrmIndexPtr = (u16*)((u32*)data->nrmIndex + (firstDrawId + nrmFirstId)) + 1;
                        clr0IndexPtr = (u16*)((u32*)data->clr0Index + (firstDrawId + clr0FirstId)) + 1;
                        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, count);
                        for (k = 0; k < count; k++) {
                            GXPosition1x16(*posIndexPtr);
                            GXNormal1x16(*nrmIndexPtr);
                            GXColor1x16(*clr0IndexPtr);
                            for (l = texCount, index = 0; l > 0; l--, index++) {
                                texIndex = texIds[(u32)(texCount - index - 1)];
                                GXTexCoord1x16((&((u32*)wp->mpCurPoseData->texIndex[texIndex])[firstDrawId + firstId[texIndex]])[k]);
                            }
                            posIndexPtr += 2;
                            nrmIndexPtr += 2;
                            clr0IndexPtr += 2;
                        }
                    }
                }
            }
        }
    }
}

// dispProc

void materialProc(s32 shapeId, s32 xluStage);
void vivihimo(Mtx a, Mtx b, f32 speed, f32 scale, f32 offA, f32 offB);

void dispProc(s32 parentId, s32 groupId, s32 xluStage, s32 arg3, f32 arg4) {
    AnimPoseGroup* group;
    AnimPoseGroup* parent;
    AnimPoseNode* node;
    AnimationPose* pose;
    AnimationPoseData* data;
    f32 sx, sy, sz;

    if (groupId == -1) {
        return;
    }
    pose = wp->mpCurPose;
    data = wp->mpCurPoseData;
    group = &data->groups[groupId];
    if (*(s8*)&pose->mpGroupVisibility[group->visibilityId]) {
        pose = wp->mpCurPose;
        node = (AnimPoseNode*)((u32*)pose->mpNodes + group->nodeId);
        if (group->useSSC) {
            if (parentId == -1) {
                sx = sy = sz = 1.0f;
            }
            else {
                parent = &data->groups[parentId];
                if (!parent->useSSC) {
                    sx = sy = sz = 1.0f;
                }
                else {
                    AnimPoseNode* pnode = (AnimPoseNode*)((u32*)pose->mpNodes + parent->nodeId);
                    sx = pnode->mScale.x;
                    sy = pnode->mScale.y;
                    sz = pnode->mScale.z;
                }
            }
            pushGXModelMtx_JointNode__(node, sx, sy, sz);
        }
        else {
            pushGXModelMtx_TransformNode__(node);
        }
        if (wp->mpCurPose->flags & 0x40) {
            Vec pos = {0.0f, 0.0f, 0.0f};
            pos.x = (*g_modeling_mtx)[0][3];
            pos.y = (*g_modeling_mtx)[1][3];
            pos.z = (*g_modeling_mtx)[2][3];
            (*wp->mpCurPose->worldPositions)[groupId] = pos;
        }
        if (wp->mpCurPose->flags & 0x80) {
            PSMTXCopy(*g_modeling_mtx, (*wp->mpCurPose->worldMatrices)[groupId]);
        }
        pose = wp->mpCurPose;
        if (pose->flags & 0x20 && groupId == pose->vivianGroupNo[0]) {
            Mtx* m;
            MtxPtr mat;
            f32* speed;
            f32 offA, offB;
            int i, j;
            s32 type;

            offB = offA = 0.0f;
            type = pose->vivianType;
            speed = (f32*)vivihimoData[type] + pose->animId * 2;
            m = g_modeling_mtx;
            mat = pose->matrix;
            switch (type) {
                case 1:
                    offA = -2.0f;
                    offB = 0.5f;
                    break;
                case 2:
                    offA = -3.0f;
                    offB = 0.5f;
                    break;
                case 3:
                    offA = -1.0f;
                    offB = 0.5f;
                    break;
            }
            materialProc(group->shapeId, xluStage);
            vivihimo(*m, mat, speed[0], speed[1], offA, offB);
            wp->mpCurPose->flags &= ~0x20;
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 3; j++) {
                    mat[i][j] += 0.04f * ((*m)[i][j] - mat[i][j]);
                }
            }
        }
        else {
            if (pose->dispCallback) {
                ((void (*)(void*, s32, Mtx*))pose->dispCallback)(pose->dispUserDataCallback, groupId, g_modeling_mtx);
            }
            renderProc(group->shapeId, xluStage);
            dispProc(groupId, group->childGroupId, xluStage, arg3, arg4);
        }
        g_modeling_mtx_lv--;
        g_modeling_mtx = &g_modeling_mtx_stack[g_modeling_mtx_lv];
    }
    dispProc(groupId, group->brotherGroupId, xluStage, arg3, arg4);
}


void animPoseDraw(s32 poseId, f32 x, f32 y, f32 z, f32 rotY, f32 scale, s32 xluMode) {
    Mtx matrix;

    MTXTrans(matrix, x, y, z);
    animPoseDrawMtx(poseId, matrix, xluMode, rotY, scale);
}

// _animPoseDrawMtx
extern BOOL mapTestXLU(u32 flags, GXColor* color, GXColor* matColor);
extern double atan2(double y, double x);
void animPoseMainSub(AnimationPose* pose);

static inline f32 _animClampAngle(f32 a) {
    while (a < -360.0f) a += 360.0f;
    while (a >= 360.0f) a -= 360.0f;
    if (a < 0.0f) a += 360.0f;
    return a;
}

static inline f32 _animClampAngle2(f32 a) {
    while (a < -360.0f) a += 360.0f;
    while (a >= 360.0f) a -= 360.0f;
    return a;
}

static inline f32 _animCamAngle(void) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    return _animClampAngle(57.29579f * (f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z)));
}

void _animPoseDrawMtx(AnimationPose* pose, Mtx matrix, s32 xluMode, f32 rotY, f32 scale) {
    Mtx result;
    Mtx tmp2;
    Mtx tmp1;
    Mtx tmp3;
    GXColor col1;
    GXColor col2;
    AnimationPoseData* data;
    AnimData* anim;
    s32 arg3, arg4;
    f32 argf;
    f32 diff;
    s32 mode;

    data = animPoseGetBaseData(pose);
    arg3 = *(s32*)((u8*)pose + 0x28);
    arg4 = *(s32*)((u8*)pose + 0x2C);
    argf = *(f32*)((u8*)pose + 0x30);
    anim = data->mpAnims[pose->animId].data;
    if (anim == NULL) {
        return;
    }
    if (wp->inBattle && pose->type == 0) {
        return;
    }
    if (!wp->inBattle && pose->type == 1) {
        return;
    }
    if (!(xluMode == 2 && (data->flags & 1)) && !(data->flags & (1 << xluMode))) {
        return;
    }
    mode = xluMode;
    col2 = pose->materialColor2;
    col1 = pose->materialColor;
    if (mapTestXLU(pose->materialFlag, &col1, &col2)) {
        mode = 3;
    }
    if (pose->gxCallback != NULL) {
        pose->gxCallback(mode);
    }
    else {
        switch (mode) {
            case 1:
                GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
                GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                GXSetZCompLoc(GX_TRUE);
                GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
                break;
            case 0:
            case 2:
                GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
                GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                GXSetZCompLoc(GX_FALSE);
                GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
                break;
            case 3:
                GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
                GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                GXSetZCompLoc(GX_TRUE);
                GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
                break;
        }
        wp->currDispMode = mode;
    }
    if (pose->flags & 8) {
        rotY += -90.0f;
        pose->field_0x70 = rotY;
        diff = _animClampAngle2(_animCamAngle() - pose->field_0x70);
        if ((diff > 90.0f && diff <= 270.0f) || (diff < -90.0f && diff >= -270.0f)) {
            pose->field_0x7C = _animClampAngle2(180.0f + _animCamAngle());
        }
        else {
            pose->field_0x7C = _animCamAngle();
        }
        while (pose->field_0x7C - pose->rotationY > 180.0f) {
            pose->rotationY += 360.0f;
        }
        while (pose->field_0x7C - pose->rotationY < -180.0f) {
            pose->rotationY -= 360.0f;
        }
        if (pose->flags & 0x100) {
            pose->rotationY = pose->field_0x7C;
        }
        else {
            pose->rotationY += 0.1f * (pose->field_0x7C - pose->rotationY);
        }
        rotY = pose->rotationY;
        if (rotY != 0.0f) {
            PSMTXRotRad(tmp2, 'y', 0.017453292f * rotY);
            PSMTXConcat(matrix, tmp2, result);
            matrix = result;
        }
        if (pose->field_0x78 == -1.0f) {
            PSMTXScale(tmp2, 1.0f, 1.0f, -1.0f);
            PSMTXConcat(matrix, tmp2, result);
            matrix = result;
        }
    }
    else if (rotY) {
        PSMTXRotRad(tmp1, 'y', 0.017453292f * rotY);
        PSMTXConcat(matrix, tmp1, result);
        matrix = result;
    }
    if (scale != 1.0f) {
        PSMTXScale(tmp3, scale, scale, scale);
        PSMTXConcat(matrix, tmp3, result);
        matrix = result;
    }
    if (pose->lastFrameZero == -1) {
        animPoseMainSub(pose);
    }
    g_modeling_mtx_lv = 0;
    g_modeling_mtx = &g_modeling_mtx_stack[0];
    PSMTXCopy(matrix, *g_modeling_mtx);
    wp->mpCurPose = pose;
    wp->mpCurAnimData = anim;
    wp->mpCurPoseData = data;
    if ((s32)wp->mbUseFloatScratch != 0) {
        DCFlushRange(wp->vtxArrayPos, animPoseGetBaseData(pose)->mBufferPosNum * sizeof(Vec));
        DCFlushRange(wp->vtxArrayNrm, animPoseGetBaseData(pose)->mBufferNrmNum * sizeof(Vec));
    }
    else {
        DCFlushRange(pose->mpVtxArrayPos, animPoseGetBaseData(pose)->mBufferPosNum * sizeof(Vec));
        DCFlushRange(pose->mpVtxArrayNrm, animPoseGetBaseData(pose)->mBufferNrmNum * sizeof(Vec));
    }
    GXInvalidateVtxCache();
    dispProc(-1, data->groupCount - 1, arg3, arg4, argf);
}



#define PDM_F(p, off) (*(f32*)((u8*)(p) + (off)))

inline void animSetPaperMtx(Mtx m0, Mtx* m1, Mtx* m2) {
    if (m0 == NULL) {
        wp->paperMtxPtr[0] = NULL;
    }
    else {
        PSMTXCopy(m0, wp->paperMtx[0]);
        wp->paperMtxPtr[0] = &wp->paperMtx[0];
    }
    wp->paperMtxPtr[1] = m1;
    wp->paperMtxPtr[2] = m2;
}

void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale)
{
  Mtx m1;
  static Mtx mtx;
  Mtx m2;
  Mtx scl;
  Mtx rot;
  Mtx tmp;
  AnimationPose *pose;
  AnimationPose *paper;
  AnimationPose *p2;
  AnimationPoseData *data;
  const char *name;
  s32 animId;
  s32 new_var[2];
  pose = &wp->poses[poseId];
  if (pose->paperId != (-1))
  {
    if (pose->flags & 0xC0)
    {
      Mtx *m;
      data = *((AnimationPoseData **) wp->files[pose->fileId].handle->data);
      m = (Mtx *) matrix;
      if ((!(pose->flags & 8)) && rotY)
      {
        PSMTXRotRad(rot, 'y', 0.017453292f * rotY);
        PSMTXConcat(matrix, rot, tmp);
        m = &tmp;
      }
      if (1.0f != scale)
      {
        PSMTXScale(scl, scale, scale, scale);
        PSMTXConcat(*m, scl, tmp);
        m = &tmp;
      }
      g_modeling_mtx_lv = 0;
      g_modeling_mtx = &g_modeling_mtx_stack[0];
      PSMTXCopy(*m, *g_modeling_mtx);
      wp->mpCurPose = pose;
      wp->mpCurPoseData = data;
      evalProc(-1, data->groupCount - 1);
    }
    paper = &wp->poses[pose->paperId];
    data = *((AnimationPoseData **) wp->files[paper->fileId].handle->data);
    if (((xluMode != 2) || (!(data->flags & 1))) && (!(data->flags & (1 << xluMode))))
    {
      return;
    }
    if (paper->typeFlags & 2)
    {
      new_var[1] = data->mBufferPosNum;
      new_var[0] = data->mBufferNrmNum;
      if ((new_var[1] + new_var[0]) <= wp->mFloatScratchRP)
      {
        if ((wp->mFloatScratchWP + (new_var[1] + new_var[0])) > wp->mFloatScratchRP)
        {
          wp->mFloatScratchWP = 0;
        }
        wp->mbUseFloatScratch = 1;
        wp->vtxArrayPos = &wp->mpFloatScratch[wp->mFloatScratchWP];
        wp->mFloatScratchWP += new_var[1];
        wp->vtxArrayNrm = &wp->mpFloatScratch[wp->mFloatScratchWP];
        wp->mFloatScratchWP += new_var[0];
      }
      name = (const char *) (*((s32 *) pose->field_0x94));
      if (name != ((const char *) (-1)))
      {
        p2 = &wp->poses[pose->paperId];
        animId = animPoseGetAnimIdx(*((AnimationPoseData **) wp->files[p2->fileId].handle->data), name);
        if (animId != p2->animId)
        {
          p2->animId = animId;
          if ((p2->paperId != (-1)) && (p2->flags & 2))
          {
            p2->flags |= 4;
          }
          p2->localTime = animTimeGetTime(p2->type);
          p2->lastFrameZero = -1;
          p2->lastFrameTime = 0.0f;
          p2->field_0x80 = 0;
          p2->loopTime = 0.0f;
        }
      }
      paper->localTime = animTimeGetTime(paper->type);
      paper->lastFrameZero = -1;
      paper->lastFrameTime = 0.0f;
      paper->field_0x80 = 0;
      paper->loopTime = 0.0f;
      paper->localTime = pose->mLocalTimeInit;
      animPoseMain(pose->paperId);
    }
    {
      void *t0 = *((void **) (((u8 *) pose) + 0x98));
      void *t1 = *((void **) (((u8 *) pose) + 0x9C));
      wp->mpTexObjs[0] = (t0) ? ((GXTexObj *) (((u8 *) pose) + 0xA0)) : ((void *) 0);
      wp->mpTexObjs[1] = (t1) ? ((GXTexObj *) (((u8 *) pose) + 0xC0)) : ((void *) 0);
      wp->mpCurTexObj = (void *) 0;
      wp->mpTexDatas[0] = t0;
      wp->mpTexDatas[1] = t1;
      wp->mpCurTexData = (void *) 0;
      wp->field_0xD8 = 1;
      p2->field_0x80 = 0;
      wp->field_0xDC = 1;
    }
    paper->materialFlag = pose->materialFlag;
    paper->materialLightFlag = pose->materialLightFlag;
    paper->materialColor = pose->materialColor;
    paper->materialColor2 = pose->materialColor2;
    if (pose->flags & 2)
    {
      PSMTXScale(m1, *((f32 *) (((u8 *) pose) + 0x10C)), *((f32 *) (((u8 *) pose) + 0x110)), 1.0f);
      PSMTXTrans(m2, *((f32 *) (0x104 + ((u8 *) pose))), *((f32 *) (((u8 *) pose) + 0x108)), 0.0f);
      PSMTXConcat(m1, m2, m1);
      animSetPaperMtx(m1, (void *) 0, (void *) 0);
      PSMTXScale(m1, pose->field_0xF8, pose->field_0xFC, pose->field_0xF8);
      if (wp->inBattle)
      {
        PSMTXTrans(m2, 0.0f, 2.0f * pose->field_0x100, 0.0f);
      }
      else
      {
        PSMTXTrans(m2, 0.0f, pose->field_0x100, 0.0f);
      }
      PSMTXConcat(m2, m1, m1);
      PSMTXConcat(matrix, m1, mtx);
      matrix = mtx;
    }
    else
    {
      PSMTXScale(m1, *((f32 *) (((u8 *) pose) + 0x10C)), *((f32 *) (((u8 *) pose) + 0x110)), 1.0f);
      PSMTXTrans(m2, *((f32 *) (((u8 *) pose) + 0x104)), *((f32 *) (((u8 *) pose) + 0x108)), 0.0f);
      PSMTXConcat(m1, m2, m1);
      PSMTXTrans(m2, 0.5f, 1.0f, 0.0f);
      PSMTXConcat(m1, m2, m1);
      PSMTXScale(m2, 1.0f / pose->field_0xF8, 1.0f / pose->field_0xFC, 1.0f);
      PSMTXConcat(m1, m2, m1);
      PSMTXTrans(m2, -0.5f, -1.0f, 0.0f);
      PSMTXConcat(m1, m2, m1);
      animSetPaperMtx(m1, (void *) 0, (void *) 0);
    }
  }
  else
  {
    data = *((AnimationPoseData **) wp->files[pose->fileId].handle->data);
    if (((xluMode != 2) || (!(data->flags & 1))) && (!(data->flags & (1 << xluMode))))
    {
      return;
    }
  }
  if (pose->paperId != (-1))
  {
    _animPoseDrawMtx(paper, matrix, xluMode, rotY, scale);
    wp->paperMtxPtr[0] = (void *) 0;
    wp->paperMtxPtr[1] = (void *) 0;
    wp->paperMtxPtr[2] = (void *) 0;
    if (paper->typeFlags & 2)
    {
      wp->mbUseFloatScratch = 0;
    }
  }
  else
  {
    _animPoseDrawMtx(pose, matrix, xluMode, rotY, scale);
  }
}

u32 anim_ext_tex_size;

void animSetPaperTexObj(GXTexObj* obj1, GXTexObj* obj2, GXTexObj* obj3, void* data1, void* data2, void* data3, s32 a7, s32 a8) {
    wp->mpTexObjs[0] = data1 ? obj1 : NULL;
    wp->mpTexObjs[1] = data2 ? obj2 : NULL;
    wp->mpCurTexObj = data3 ? obj3 : NULL;
    wp->mpTexDatas[0] = data1;
    wp->mpTexDatas[1] = data2;
    wp->mpCurTexData = data3;
    wp->field_0xD8 = a7;
    wp->field_0xDC = a7;
}

inline void animPoseFileRelease(s32 fileId) {
    AnimationPoseFile* file = &wp->files[fileId];
    AnimationTextureFile* tex;

    file->references--;
    if (file->references <= 0) {
        file->active = 0;
        tex = &wp->textures[file->textureId];
        tex->references--;
        if (tex->references <= 0) {
            fileFree(tex->handle);
            tex->active = FALSE;
        }
        fileFree(file->handle);
    }
}

void animPoseRelease(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];

    if ((u32)pose->typeFlags != 0) {
        animPaperPoseRelease(poseId);
    }
    else {
        if (pose->flags & 0x40) {
            smartFree((smartEntry*)pose->worldPositions);
        }
        if (pose->flags & 0x80) {
            smartFree((smartEntry*)pose->worldMatrices);
        }
        animPoseFileRelease(pose->fileId);
        pose->flags = 0;
    }
}

void animPaperPoseRelease(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];

    pose->references--;
    if (pose->references < 0) {
        animPoseFileRelease(pose->fileId);
        pose->flags = 0;
    }
}

void animPoseAutoRelease(s32 type) {
    int i;
    AnimationPose* pose;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = animPoseGetAnimPosePtr(i);
        if (pose->flags & 1 && pose->type == type) {
            animPoseRelease(i);
        }
    }
    if (type == 1) {
        wp->inBattle = FALSE;
    }
    animPoseRefresh();
}

#pragma dont_inline on
void animPaperPoseDisp(CameraId camId, void* param) {
    int i;
    AnimationPose* pose;

    for (i = 0, pose = wp->poses; i < wp->totalPoses; i++, pose++) {
        if (pose->flags & 1 && pose->paperId != -1) {
            animPaperPoseDispSub(0, pose);
        }
    }
}
#pragma dont_inline reset

#include "sdk/DEMOInit.h"
void _animPoseDrawMtx(AnimationPose* pose, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);
typedef struct PaperAnimData {
    u8 pad[0x44];
    f32 x44, x48, x4C, x50, x54;
} PaperAnimData;
#define PP_ENT(p) (*(smartEntry**)((u8*)(p) + 0x98))
#define PP_9C(p) (*(s32*)((u8*)(p) + 0x9C))
#define PP_F(p, o) (*(f32*)((u8*)(p) + (o)))
#define PP_S(p, o) (*(s16*)((u8*)(p) + (o)))

void animPaperPoseDispSub(s32 unused, AnimationPose* pose) {
    PaperAnimData* d;
    Mtx m, save;
    GXColor col1;
    u16 h;
    GXColor col2;
    s32 lightFlag;
    u32 flags;
    s32 matFlag;
    u16 left, top, right, bottom;
    u16 w;
    f32 scale, hh;
    s32 t, e, b, a, x1, y0, y1;

    scale = 8.0f;
    if (!(pose->flags & 1)) {
        return;
    }
    d = (PaperAnimData*)animPoseGetBaseData(pose)->mpAnims[pose->animId].data;
    scale *= 25.74f / (d->x54 - d->x48);
    hh = 0.75f * DEMOGetRenderModeObj()->efbHeight;
    sysWaitDrawSync();
    GXClearBoundingBox();
    MTXTrans(m, 0.0f, 0.25f * -(f32)DEMOGetRenderModeObj()->efbHeight - d->x48 * scale, 0.0f);
    flags = pose->flags;
    matFlag = pose->materialFlag;
    lightFlag = pose->materialLightFlag;
    col1 = pose->materialColor;
    col2 = pose->materialColor2;
    pose->flags = flags & ~8;
    pose->materialColor = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    pose->materialColor2 = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    pose->materialLightFlag = 0;
    if (pose->vivianType) {
        PSMTXCopy(pose->matrix, save);
        PSMTXCopy(m, pose->matrix);
    }
    _animPoseDrawMtx(pose, m, 2, 0.0f, scale);
    pose->flags = flags;
    pose->materialFlag = matFlag;
    pose->materialColor = col1;
    pose->materialColor2 = col2;
    pose->materialLightFlag = lightFlag;
    if (pose->vivianType) {
        PSMTXCopy(save, pose->matrix);
    }
    sysWaitDrawSync();
    GXReadBoundingBox(&left, &top, &right, &bottom);
    if (left > right) {
        left = 0;
        top = 0;
        right = 16;
        bottom = 16;
    }
    if (pose->flags & 2 && pose->flags & 4 && gp->inBattle) {
        a = d->x44 * scale + (s32)(DEMOGetRenderModeObj()->fbWidth / 2);
        x1 = d->x50 * scale + (s32)(DEMOGetRenderModeObj()->fbWidth / 2);
        y0 = hh - scale * (d->x54 - d->x48);
        y1 = hh;
        if (a <= left && x1 >= right && y0 <= top && y1 >= bottom) {
            left = a;
            right = x1;
            top = y0;
            bottom = y1;
        }
    }
    if (left < 2) {
        left = 2;
    }
    if (top < 2) {
        top = 2;
    }
    if (right > DEMOGetRenderModeObj()->fbWidth - 2) {
        right = DEMOGetRenderModeObj()->fbWidth - 2;
    }
    if (bottom > DEMOGetRenderModeObj()->efbHeight - 2) {
        bottom = DEMOGetRenderModeObj()->efbHeight - 2;
    }
    if (pose->flags & 2) {
        if (pose->flags & 4) {
            a = DEMOGetRenderModeObj()->fbWidth / 2 - left;
            t = right - DEMOGetRenderModeObj()->fbWidth / 2;
            b = a;
            if (a < t) {
                b = t;
            }
            a = 0.5f * b;
            e = 0.14881f * (bottom - top);
            PP_S(pose, 0x114) = DEMOGetRenderModeObj()->fbWidth / 2 - b - a;
            PP_S(pose, 0x116) = DEMOGetRenderModeObj()->fbWidth / 2 + b + a;
            PP_S(pose, 0x118) = top - e;
            PP_S(pose, 0x11A) = bottom + e;
            PP_F(pose, 0xF8) = (8.0f / scale) * ((f32)(PP_S(pose, 0x116) - PP_S(pose, 0x114)) / 150.0f);
            PP_F(pose, 0xFC) = (8.0f / scale) * ((f32)(PP_S(pose, 0x11A) - PP_S(pose, 0x118)) / 215.0f);
            PP_F(pose, 0x100) = d->x48 + (hh - bottom) / scale;
            pose->flags &= ~4;
        }
    } else {
        s32 a;
        a = DEMOGetRenderModeObj()->fbWidth / 2 - left;
        t = right - DEMOGetRenderModeObj()->fbWidth / 2;
        b = a;
        if (a < t) {
            b = t;
        }
        a = 0.5f * b;
        e = 0.14881f * (bottom - top);
        PP_S(pose, 0x114) = DEMOGetRenderModeObj()->fbWidth / 2 - b - a;
        PP_S(pose, 0x116) = DEMOGetRenderModeObj()->fbWidth / 2 + b + a;
        PP_S(pose, 0x118) = top - e;
        PP_S(pose, 0x11A) = bottom + e;
        PP_F(pose, 0xF8) = (8.0f / scale) * ((f32)(PP_S(pose, 0x116) - PP_S(pose, 0x114)) / 150.0f);
        PP_F(pose, 0xFC) = (8.0f / scale) * ((f32)(PP_S(pose, 0x11A) - PP_S(pose, 0x118)) / 215.0f);
        PP_F(pose, 0x100) = d->x48 + (hh - bottom) / scale;
    }
    left -= 2;
    top -= 2;
    right += 2;
    bottom += 2;
    left -= left & 1;
    top -= top & 1;
    right += right & 1;
    bottom += bottom & 1;
    w = right - left;
    h = bottom - top;
    PP_F(pose, 0x104) = -((f32)left - PP_S(pose, 0x114)) / (f32)(PP_S(pose, 0x116) - PP_S(pose, 0x114));
    PP_F(pose, 0x108) = -((f32)top - PP_S(pose, 0x118)) / (f32)(PP_S(pose, 0x11A) - PP_S(pose, 0x118));
    PP_F(pose, 0x10C) = (f32)(PP_S(pose, 0x116) - PP_S(pose, 0x114)) / (f32)w;
    PP_F(pose, 0x110) = (f32)(PP_S(pose, 0x11A) - PP_S(pose, 0x118)) / (f32)h;
    GXSetTexCopySrc(left, top, w, h);
    GXSetTexCopyDst(w, h, GX_TF_RGB5A3, GX_FALSE);
    PP_ENT(pose) = smartAlloc(GXGetTexBufferSize(w, h, GX_TF_RGB5A3, GX_FALSE, 0), 3);
    GXCopyTex(PP_ENT(pose)->address, GX_TRUE);
    GXPixModeSync();
    GXInitTexObj((GXTexObj*)((u8*)pose + 0xA0), PP_ENT(pose)->address, w, h, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
    PP_9C(pose) = 0;
}

void _animPoseDrawMtx(AnimationPose* pose, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);
#include "sdk/DEMOInit.h"

inline s32 animPoseSearchName(const char* name, s32 type) {
    AnimationPose* pose;
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if (pose->flags & 1 && pose->type == type && !strcmp(animPoseGetBaseData(pose)->fileName, name)) {
            break;
        }
    }
    if (i == wp->totalPoses) {
        i = -1;
    }
    return i;
}

void animPoseDisp_MakeExtTexture(const char* group, const char* anim, f32 frame, f32 scale, GXTexObj* obj,
                                 smartEntry** image, s32* x0, s32* y0, s32* x1, s32* y1) {
    AnimationPose* pose;
    AnimationPose* pera;
    AnimationPose backup;
    Mtx m;
    GXColor col1, col2;
    u32 flags;
    s32 matFlag;
    u16 left, top, right, bottom;
    u16 w, h;
    f32 hw, hh;
    s32 i;

    i = animPoseSearchName(group, gp->inBattle != 0);
    if (i < 0) {
        return;
    }
    pose = &wp->poses[i];
    backup = *pose;
    pose->paperId = -1;
    animPoseSetAnim(i, anim, FALSE);
    pose->localTime = animTimeGetTime(pose->type);
    pose->lastFrameZero = -1;
    pose->lastFrameTime = 0.0f;
    pose->field_0x80 = 0;
    pose->loopTime = 0.0f;
    animPoseSetLocalTime(i, frame);
    animPoseMain(i);

    hw = 0.5f * DEMOGetRenderModeObj()->fbWidth;
    hh = 0.75f * DEMOGetRenderModeObj()->efbHeight;
    sysWaitDrawSync();
    GXClearBoundingBox();
    MTXTrans(m, 0.0f, 0.25f * -(f32)DEMOGetRenderModeObj()->efbHeight, 0.0f);
    flags = pose->flags;
    matFlag = pose->materialFlag;
    col1 = pose->materialColor;
    col2 = pose->materialColor2;
    pose->flags = flags & ~8;
    pose->materialFlag = 0;
    pose->materialColor = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    pose->materialColor2 = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    _animPoseDrawMtx(pose, m, 1, 0.0f, scale);
    _animPoseDrawMtx(pose, m, 2, 0.0f, scale);
    _animPoseDrawMtx(pose, m, 3, 0.0f, scale);
    pose->materialColor = col1;
    pose->materialColor2 = col2;
    pose->flags = flags;
    pose->materialFlag = matFlag;
    sysWaitDrawSync();
    GXReadBoundingBox(&left, &top, &right, &bottom);
    if (left > right) {
        top = left = 0;
        bottom = right = 16;
    }
    if (left < 2) {
        left = 2;
    }
    if (top < 2) {
        top = 2;
    }
    if (right > DEMOGetRenderModeObj()->fbWidth - 2) {
        right = DEMOGetRenderModeObj()->fbWidth - 2;
    }
    if (top > DEMOGetRenderModeObj()->efbHeight - 2) {
        top = DEMOGetRenderModeObj()->efbHeight - 2;
    }
    left -= left & 1;
    top -= top & 1;
    right += right & 1;
    bottom += bottom & 1;
    w = right - left;
    h = bottom - top;
    GXSetTexCopySrc(left, top, w, h);
    GXSetTexCopyDst(w, h, GX_TF_RGB5A3, GX_FALSE);
    anim_ext_tex_size += GXGetTexBufferSize(w, h, GX_TF_RGB5A3, GX_FALSE, 0);
    *image = smartAlloc(GXGetTexBufferSize(w, h, GX_TF_RGB5A3, GX_FALSE, 0), 1);
    GXCopyTex((*image)->address, GX_TRUE);
    GXPixModeSync();
    GXInitTexObj(obj, (*image)->address, w, h, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
    *x0 = left - (s32)hw;
    *y0 = top - (s32)hh;
    *x1 = right - (s32)hw;
    *y1 = bottom - (s32)hh;
    *pose = backup;
}

void animSetPaperTexMtx(Mtx mtx1, Mtx mtx2, Mtx mtx3) {
    if (mtx1 == NULL) {
        wp->paperMtxPtr[0] = NULL;
    }
    else {
        PSMTXCopy(mtx1, wp->paperMtx[0]);
        wp->paperMtxPtr[0] = &wp->paperMtx[0];
    }
    if (mtx2 == NULL) {
        wp->paperMtxPtr[1] = NULL;
    }
    else {
        PSMTXCopy(mtx2, wp->paperMtx[1]);
        wp->paperMtxPtr[1] = &wp->paperMtx[1];
    }
    if (mtx3 == NULL) {
        wp->paperMtxPtr[2] = NULL;
    }
    else {
        PSMTXCopy(mtx3, wp->paperMtx[2]);
        wp->paperMtxPtr[2] = &wp->paperMtx[2];
    }
}

BOOL animGroupBaseAsync(const char* name, s32 group, void* callback) {
    s32 model, anim;

    switch (group) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }
    model = fileAsyncf(5, callback, "%s/%s", "a", name);
    anim = fileAsyncf(4, NULL, "%s/%s-", "a", animGetTexName(name));
    if (model == -1) {
        return TRUE;
    }
    if ((u32)model == 0) {
        return FALSE;
    }
    if (anim == -1) {
        return TRUE;
    }
    return anim != 0;
}

const char* animGetTexName(const char* name) {
    void** data = wp->mp_ag2tg->data;
    s32 size;
    s32 i;
    char* entry;

    size = (s32)data[1];
    entry = data[0];

    for (i = 0; i < size; entry += 0x20, i += 0x20) {
        if (!strcmp(entry, name)) {
            return entry + 0x20;
        }
    }
    return "";
}

void animStrippedPool(f32* p, u32 n) {
    p[0] = 0.0f;
    p[1] = 0.017453292f * p[1];
    p[2] = n;
}

AnimationPose* animPoseGetAnimPosePtr(s32 poseId) {
    return &wp->poses[poseId];
}

AnimData* animPoseGetAnimDataPtr(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    return animPoseGetAnimBaseDataPtr(poseId)->mpAnims[pose->animId].data;
}

AnimationPoseData* animPoseGetAnimBaseDataPtr(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    return *wp->files[pose->fileId].handle->data;
}

AnimTableEntry* animPoseGetCurrentAnim(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return &data->mpAnims[animPoseGetAnimPosePtr(poseId)->animId];
}

BOOL animPoseTestXLU(s32 poseId) {
    return (animPoseGetAnimBaseDataPtr(poseId)->flags >> 3) & 1;
}

void evalProc(s32 parentId, s32 groupId) {
    AnimPoseGroup* group;
    AnimPoseGroup* parent;
    AnimPoseNode* node;
    AnimationPose* pose;
    AnimationPoseData* data;
    f32 sx, sy, sz;

    if (groupId == -1) {
        return;
    }
    pose = wp->mpCurPose;
    data = wp->mpCurPoseData;
    group = &data->groups[groupId];
    if (*(s8*)&pose->mpGroupVisibility[group->visibilityId]) {
        pose = wp->mpCurPose;
        node = (AnimPoseNode*)((u32*)pose->mpNodes + group->nodeId);
        if (group->useSSC) {
            if (parentId == -1) {
                sx = sy = sz = 1.0f;
            }
            else {
                parent = &data->groups[parentId];
                if (!parent->useSSC) {
                    sx = sy = sz = 1.0f;
                }
                else {
                    AnimPoseNode* pnode = (AnimPoseNode*)((u32*)pose->mpNodes + parent->nodeId);
                    sx = pnode->mScale.x;
                    sy = pnode->mScale.y;
                    sz = pnode->mScale.z;
                }
            }
            pushGXModelMtx_JointNode__(node, sx, sy, sz);
        }
        else {
            pushGXModelMtx_TransformNode__(node);
        }
        if (wp->mpCurPose->flags & 0x40) {
            Vec pos = {0.0f, 0.0f, 0.0f};
            pos.x = (*g_modeling_mtx)[0][3];
            pos.y = (*g_modeling_mtx)[1][3];
            pos.z = (*g_modeling_mtx)[2][3];
            (*wp->mpCurPose->worldPositions)[groupId] = pos;
        }
        if (wp->mpCurPose->flags & 0x80) {
            PSMTXCopy(*g_modeling_mtx, (*wp->mpCurPose->worldMatrices)[groupId]);
        }
        if (!(wp->mpCurPose->flags & 0x20) || groupId != wp->mpCurPose->vivianGroupNo[0]) {
            evalProc(groupId, group->childGroupId);
        }
        g_modeling_mtx_lv--;
        g_modeling_mtx = &g_modeling_mtx_stack[g_modeling_mtx_lv];
    }
    evalProc(groupId, group->brotherGroupId);
}

void animPoseWorldPositionEvalOn(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = animPoseGetBaseData(pose);
    u8 heap;

    if (!(pose->flags & 0x40)) {
        pose->flags |= 0x40;
        switch (pose->type) {
            case 0:
                heap = 1;
                break;
            case 1:
                heap = 2;
                break;
            case 2:
                heap = 0;
                break;
        }
        pose->numWorldPositions = data->groupCount;
        pose->worldPositions = (Vec**)smartAlloc(sizeof(Vec) * data->groupCount, heap);
    }
}

void L_animPoseWorldPositionEvalOff(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];

    if (pose->flags & 0x40) {
        pose->flags &= ~0x40;
        smartFree((smartEntry*)pose->worldPositions);
        pose->worldPositions = NULL;
    }
}

void animPoseWorldMatrixEvalOn(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = animPoseGetBaseData(pose);
    u8 heap;

    if (!(pose->flags & 0x80)) {
        pose->flags |= 0x80;
        switch (pose->type) {
            case 0:
                heap = 1;
                break;
            case 1:
                heap = 2;
                break;
            case 2:
                heap = 0;
                break;
        }
        pose->numWorldMatrices = data->groupCount;
        pose->worldMatrices = (Mtx**)smartAlloc(sizeof(Mtx) * data->groupCount, heap);
    }
}

void animPoseVivianMain(s32 poseId, Vec* pos) {
    AnimationPose* pose = &wp->poses[poseId];

    pose->flags |= 0x20;
    pose->matrix[0][3] = pos->x;
    pose->matrix[1][3] = pos->y;
    pose->matrix[2][3] = pos->z;
}

s32 animPoseGetVivianType(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return pose->vivianType;
}

void animPoseSetDispCallBack(s32 poseId, void* callback, void* userData) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->dispCallback = callback;
    pose->dispUserDataCallback = userData;
}

inline void animPoseMainSub(AnimationPose* pose) {
    s32 id = ((u32)pose - (u32)wp->poses) / sizeof(AnimationPose);
    if (pose->typeFlags & 2) {
        AnimationPoseData* data = animPoseGetBaseData(pose);
        s32 posNum = data->mBufferPosNum;
        s32 nrmNum = data->mBufferNrmNum;
        if (posNum + nrmNum <= wp->mFloatScratchRP) {
            if (wp->mFloatScratchWP + (posNum + nrmNum) > wp->mFloatScratchRP) {
                wp->mFloatScratchWP = 0;
            }
            wp->mbUseFloatScratch = 1;
            wp->vtxArrayPos = &wp->mpFloatScratch[wp->mFloatScratchWP];
            wp->mFloatScratchWP += posNum;
            wp->vtxArrayNrm = &wp->mpFloatScratch[wp->mFloatScratchWP];
            wp->mFloatScratchWP += nrmNum;
        }
        animPoseMain(id);
        wp->mbUseFloatScratch = 0;
    }
    else {
        animPoseMain(id);
    }
}

void animPoseDrawShape(s32 poseId, s32 shapeId) {
    AnimationPose* pose;
    GXTexObj* texObj0;
    GXTexObj* texObj1;
    GXTexObj* curTexObj;
    void* texData0;
    void* texData1;
    void* curTexData;
    Mtx* paperMtx0;
    Mtx* paperMtx1;
    Mtx* paperMtx2;
    AnimationPose* curPose;
    AnimData* curAnimData;
    AnimationPoseData* curPoseData;
    u32 d8;
    u32 dc;
    s32 id;

    pose = &wp->poses[poseId];
    curPose = wp->mpCurPose;
    curAnimData = wp->mpCurAnimData;
    curPoseData = wp->mpCurPoseData;
    texObj0 = wp->mpTexObjs[0];
    texObj1 = wp->mpTexObjs[1];
    curTexObj = wp->mpCurTexObj;
    texData0 = wp->mpTexDatas[0];
    texData1 = wp->mpTexDatas[1];
    curTexData = wp->mpCurTexData;
    paperMtx0 = wp->paperMtxPtr[0];
    paperMtx1 = wp->paperMtxPtr[1];
    paperMtx2 = wp->paperMtxPtr[2];
    d8 = wp->field_0xD8;
    dc = wp->field_0xDC;

    wp->mpCurPose = pose;
    wp->mpCurAnimData = animPoseGetBaseData(pose)->mpAnims[pose->animId].data;
    wp->mpCurPoseData = animPoseGetBaseData(pose);
    wp->mpTexObjs[0] = NULL;
    wp->mpTexObjs[1] = NULL;
    wp->mpCurTexObj = NULL;
    wp->mpTexDatas[0] = NULL;
    wp->mpTexDatas[1] = NULL;
    wp->mpCurTexData = NULL;
    wp->paperMtxPtr[0] = NULL;
    wp->paperMtxPtr[1] = NULL;
    wp->paperMtxPtr[2] = NULL;
    wp->field_0xD8 = 0;
    wp->field_0xDC = 0;

    if (pose->lastFrameZero == -1) {
        animPoseMainSub(pose);
    }
    renderProc(shapeId, pose->field_0x28);

    wp->mpCurPose = curPose;
    wp->mpCurAnimData = curAnimData;
    wp->mpCurPoseData = curPoseData;
    wp->mpTexObjs[0] = texObj0;
    wp->mpTexObjs[1] = texObj1;
    wp->mpCurTexObj = curTexObj;
    wp->mpTexDatas[0] = texData0;
    wp->mpTexDatas[1] = texData1;
    wp->mpCurTexData = curTexData;
    wp->paperMtxPtr[0] = paperMtx0;
    wp->paperMtxPtr[1] = paperMtx1;
    wp->paperMtxPtr[2] = paperMtx2;
    wp->field_0xD8 = d8;
    wp->field_0xDC = dc;
}

s32 animPoseGetShapeIdx(s32 poseId, const char* name) {
    AnimationPoseData* data;
    AnimPoseShape* shape;
    s32 i;

    data = animPoseGetAnimBaseDataPtr(poseId);
    shape = data->shapes;
    for (i = 0; i < data->shapeCount; i++, shape++) {
        if (!strcmp(shape->name, name)) {
            return i;
        }
    }
    return -1;
}

s32 animPoseGetGroupIdx(s32 poseId, const char* name) {
    AnimationPoseData* data;
    AnimPoseGroup* group;
    s32 i;

    data = animPoseGetAnimBaseDataPtr(poseId);
    group = data->groups;
    for (i = 0; i < data->groupCount; i++, group++) {
        if (!strcmp(group->name, name)) {
            return i;
        }
    }
    return -1;
}

const char* animPoseGetGroupName(s32 poseId, s32 group) {
    return animPoseGetAnimBaseDataPtr(poseId)->groups[group].name;
}

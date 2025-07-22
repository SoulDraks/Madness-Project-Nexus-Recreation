#ifndef COLOR_H
#define COLOR_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <SDL.h>
#include "types/bool.h"

typedef struct {
    Uint8 r, g, b, a;
} Color;

#define INT_TO_UINT8(_int) \
    if(_int > 255) \
    { \
        _int = 255; \
    } \
    else if(_int < 0) \
    { \
        _int = 0; \
    } \

extern Color Color_add(const Color c1, const Color c2);
extern Color Color_sub(const Color c1, const Color c2);
extern Color Color_mul(const Color color, const float scalar);
extern Color Color_div(const Color color, const float scalar);
extern bool Color_eq(const Color c1, const Color c2);
extern const char* Color_tostring(const Color self);
extern void Color_register(lua_State* L);

#endif
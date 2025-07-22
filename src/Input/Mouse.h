#ifndef MOUSE_H
#define MOUSE_H

#include <SDL.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "types/bool.h"

typedef struct MouseState {
    int x;
    int y;
    bool isLeftClickPressed;
    bool isRightClickPressed;
    bool isLeftClickReleased;
    bool isRightClickReleased;
} MouseState;

extern MouseState* getMouseState();
extern void Mouse_Init(lua_State* L);
extern void Mouse_Update();
extern void Mouse_Free();

/* <----------- BINDINGS -----------> */

extern int Mouse_indexLua(lua_State* L);
extern int Mouse_newindexLua(lua_State* L);

#endif
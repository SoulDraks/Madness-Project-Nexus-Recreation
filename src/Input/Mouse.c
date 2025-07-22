#include "Mouse.h"
#include <string.h>

static MouseState* mousestate = NULL;

MouseState* getMouseState()
{
    return mousestate;
}

void Mouse_Init(lua_State* L)
{
    mousestate = malloc(sizeof(MouseState));
    mousestate->x = 0;
    mousestate->y = 0;
    mousestate->isLeftClickPressed = false;
    mousestate->isLeftClickReleased = false;
    mousestate->isRightClickPressed = false;
    mousestate->isRightClickReleased = false;

    // Registrar Mouse a Lua.
    luaL_newmetatable(L, "Mouse");

    lua_pushcfunction(L, Mouse_indexLua);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, Mouse_newindexLua);
    lua_setfield(L, -2, "__newindex");


    MouseState** mousestateLua = (MouseState**)lua_newuserdata(L, sizeof(MouseState*));
    *mousestateLua = mousestate;

    // Crear y asignar como uservalue al userdata de "Mouse" una tabla.
    lua_newtable(L);
    lua_setuservalue(L, -2);

    lua_pushvalue(L, -2); // Poner en la cima de la pila la metatabla "Mouse"
    lua_setmetatable(L, -2);

    lua_setglobal(L, "Mouse"); // Poner la "instancia" de Mouse como global.
    lua_pop(L, 1); // Sacar la metatabla "Mouse" de la pila.
}

void Mouse_Update()
{
    SDL_GetMouseState(&mousestate->x, &mousestate->y);
    Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
    bool left_click = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    bool right_click = (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    mousestate->isLeftClickPressed = left_click;
    mousestate->isRightClickPressed = right_click;
    mousestate->isLeftClickReleased = mousestate->isLeftClickPressed && !left_click;
    mousestate->isRightClickReleased = mousestate->isRightClickPressed && !right_click;
}

void Mouse_Free()
{
    free(mousestate);
}

/* <----------- BINDINGS -----------> */

int Mouse_indexLua(lua_State* L)
{
    MouseState** self = (MouseState**)luaL_checkudata(L, 1, "Mouse");
    const char* key = luaL_checkstring(L, 2);
    if(strcmp(key, "x") == 0)
        lua_pushinteger(L, (*self)->x);
    else if(strcmp(key, "y") == 0)
        lua_pushinteger(L, (*self)->y);
    else if(strcmp(key, "isLeftClickPressed") == 0)
        lua_pushboolean(L, (*self)->isLeftClickPressed);
    else if(strcmp(key, "isRightClickPressed") == 0)
        lua_pushboolean(L, (*self)->isRightClickPressed);
    else if(strcmp(key, "isLeftClickReleased") == 0)
        lua_pushboolean(L, (*self)->isLeftClickReleased);
    else if(strcmp(key, "isRightClickReleased") == 0)
        lua_pushboolean(L, (*self)->isRightClickReleased);
    else
    {
        lua_getuservalue(L, 1);
        lua_getfield(L, -1, key);
        lua_remove(L, -2);
    }
    return 1;
}

int Mouse_newindexLua(lua_State* L)
{
    MouseState** self = (MouseState**)luaL_checkudata(L, 1, "Mouse");
    const char* key = luaL_checkstring(L, 2);
    if(strcmp(key, "x") == 0)
    {
        int value = luaL_checkinteger(L, 3);
        (*self)->x = value;
    }
    else if(strcmp(key, "y") == 0)
    {
        int value = luaL_checkinteger(L, 3);
        (*self)->y = value;
    }
    else if(strcmp(key, "isLeftClickPressed") == 0)
    {
        if(!lua_isboolean(L, 3))
            return 0;
        bool value = lua_toboolean(L, 3);
        (*self)->isLeftClickPressed = value;
    }
    else if(strcmp(key, "isRightClickPressed") == 0)
    {
        if(!lua_isboolean(L, 3))
            return 0;
        bool value = lua_toboolean(L, 3);
        (*self)->isRightClickPressed = value;
    }
    else if(strcmp(key, "isLeftClickReleased") == 0)
    {
        if(!lua_isboolean(L, 3))
            return 0;
        bool value = lua_toboolean(L, 3);
        (*self)->isLeftClickReleased = value;
    }
    else if(strcmp(key, "isRightClickReleased") == 0)
    {
        if(!lua_isboolean(L, 3))
            return 0;
        bool value = lua_toboolean(L, 3);
        (*self)->isRightClickReleased = value;
    }
    else
    {
        lua_getuservalue(L, 1);
        lua_pushvalue(L, 3);
        lua_setfield(L, -2, key);
        lua_remove(L, -2);
    }
    return 0;
}
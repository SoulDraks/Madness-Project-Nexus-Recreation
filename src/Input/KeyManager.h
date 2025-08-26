#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include <SDL.h>
#include "Lua/State.h"
#include "Lua/Bindings.h"
#include "types/bool.h"

struct eventListener {
    luaFuncRef func;
    lua_State* L;
};

typedef struct {
    int keyCode;
    bool isDown;
    bool isUp;
    struct eventListener* keyDownListeners; // Array de referencias de funciones de Lua.
    int keyDownListenersCount; // Cantidad de elementos en el array
    size_t keyDownListeners_size; // Tamaño del Array.
    struct eventListener* keyUpListeners; 
    int keyUpListenersCount;
    size_t keyUpListeners_size;
} KeyManager;

extern void KeyManager_Init();
extern void KeyManager_Update(const int keyCode, const bool isPressed, const bool isReleased);
extern int KeyManager_getKeyCode();
extern void KeyManager_callKeyDownListeners();
extern void KeyManager_callKeyUpListeners();
/* <--------- METODOS PARA LUA ---------> */
extern int KeyManager_isKeyPressedLua(lua_State* L);
extern int KeyManager_isKeyReleasedLua(lua_State* L);
extern int KeyManager_addEventListenerLua(lua_State* L);
extern int KeyManager_removeEventListenerLua(lua_State* L);

#endif
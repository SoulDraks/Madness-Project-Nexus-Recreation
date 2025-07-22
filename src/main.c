#define SDL_MAIN_HANDLED
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "types/String.h"
#include "Math/SoulMath.h"
#include "Input/Mouse.h"
#include "Input/KeyManager.h"
#include "Render/Renderer.h"
#include "Render/Window.h"
#include "Lua/LuaManager.h"
#include "Engine/Engine.h"
#include "Core/UI/Label.h"

int main()
{
    // Crear el estado de Lua.
    lua_State* L = luaL_newstate();
    // Inicializar absolutamente Todo.
    String_init(L);
    SoulMath_init();
    initLua(L);
    initSDL();
    Window_init();
    initRender(getWindow());
    Mouse_Init(L);
    KeyManager_Init(L);
    Engine_Init(L);
    
    // Ejecutamos el script principal.
    lua_executescript(L, "scripts/main.lua");
    Engine_Loop();

    lua_close(L);
    renderDestroy();
    Window_destroy();
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
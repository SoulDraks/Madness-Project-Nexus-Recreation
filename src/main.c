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
#include "Lua/State.h"
#include "Engine/Engine.h"
#include "Core/UI/Label.h"
#include "Core/Animation/Animation.h"

int main()
{
    Lua_Init();
    SoulMath_init();
    initSDL();
    Window_init();
    initRender(getWindow());
    Mouse_Init();
    KeyManager_Init();
    Engine_Init();
    
    // Ejecutamos el script principal.
    lua_executescript(getLuaState(), "scripts/main.lua");
    Engine_Loop();

    Window_destroy();
    lua_close(getLuaState());
    renderDestroy();
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
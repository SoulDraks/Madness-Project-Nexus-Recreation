#include "Window.h"
#include <stdio.h>
#include "Input/Mouse.h"
#include "Input/KeyManager.h"

#define IS_RESIZABLE false
#define WINDOW_TITLE "Madness Project Nexus"
#define WINDOW_SIZE (Vector2){850.0, 550.0}

static SDL_Window* window = NULL;
static Vector2 window_pos = (Vector2){0.0f, 0.0f};
static Vector2 window_size = WINDOW_SIZE;
static String window_title = NULLSTRING;
static SDL_Event events;

SDL_Window* getWindow()
{
    return window;
}

void Window_init()
{
    SDL_WindowFlags resizable = SDL_WINDOW_ALLOW_HIGHDPI;
    if(IS_RESIZABLE)
        resizable = SDL_WINDOW_RESIZABLE;
    window_title = String_new(WINDOW_TITLE);
    window = SDL_CreateWindow(window_title.data, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)window_size.x, (int)window_size.y, resizable);
    if(window == NULL)
    {
        printf("FATAL ERROR: it Cannot create the window.\n");
        exit(EXIT_FAILURE);
    }
    // Obtenemos su posicion.
    int x, y;
    SDL_GetWindowPosition(window, &x, &y);
    window_pos.x = (double)x;
    window_pos.y = (double)y;
}

bool Window_process()
{
    bool running = true;
    while(SDL_PollEvent(&events))
    {
        switch(events.type)
        {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                KeyManager_Update(events.key.keysym.sym, true, false);
                break;
            case SDL_KEYUP:
                KeyManager_Update(events.key.keysym.sym, false, true);
                break;
            default:
                break;
        }
    }
    int x, y, w, h;
    SDL_GetWindowPosition(window, &x, &y);
    SDL_GetWindowSize(window, &w, &h);
    window_pos.x = (double)x;
    window_pos.y = (double)y;
    window_size.x = (double)w;
    window_size.y = (double)h;
    return running;
}

void Window_destroy()
{
    if(window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    if(window_title.data != NULL)
        String_free(window_title);
}

void Window_setPosition(const Vector2 newPosition)
{
    window_pos = newPosition;
    SDL_SetWindowPosition(window, (int)window_pos.x, (int)window_pos.y);
}

void Window_setSize(const Vector2 newSize)
{
    if(!IS_RESIZABLE)
        return;
    window_size = newSize;
    SDL_SetWindowSize(window, (int)window_size.x, (int)window_size.y);
}

void Window_setTitle(String newTitle)
{
    if(window_title.data != NULL)
        String_free(window_title);
    window_title = String_dup(newTitle);
    SDL_SetWindowTitle(window, window_title.data);
}

Vector2 Window_getPosition()
{
    return window_pos;
}

Vector2 Window_getSize()
{
    return window_size;
}

String Window_getTitle()
{
    return String_dup(window_title);
}

/* <----------- BINDINGS -----------> */

static int Window_newindexLua(lua_State* L)
{
    const char* key = luaL_checkstring(L, 2);
    if(strcmp(key, "pos") == 0)
    {
        Vector2* value = luaL_checkudata(L, 3, "Vector2");
        Window_setPosition(*value);
    }
    else if(strcmp(key, "pos_x") == 0)
    {
        float value = luaL_checknumber(L, 3);
        Window_setPosition((Vector2){value, window_pos.y});
    }
    else if(strcmp(key, "pos_y") == 0)
    {
        float value = luaL_checknumber(L, 3);
        Window_setPosition((Vector2){window_pos.x, value});
    }
    else if(strcmp(key, "size") == 0)
    {
        Vector2* value = luaL_checkudata(L, 3, "Vector2");
        Window_setSize(*value);
    }
    else if(strcmp(key, "size_x") == 0)
    {
        float value = luaL_checknumber(L, 3);
        Window_setSize((Vector2){value, window_size.y});
    }
    else if(strcmp(key, "size_y") == 0)
    {
        float value = luaL_checknumber(L, 3);
        Window_setSize((Vector2){window_size.x, value});
    }
    else if(strcmp(key, "title") == 0)
    {
        String newTitle = String_newFromLua(L, 3);
        Window_setTitle(newTitle);
        String_free(newTitle);
    }
    else
        lua_rawset(L, 1); // Window[key] = value
    
    return 0;
}

static int Window_indexLua(lua_State* L)
{
    const char* key = luaL_checkstring(L, 2);
    if(strcmp(key, "pos") == 0)
    {
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *vec = window_pos;
        luaL_getmetatable(L, "Vector2");
        lua_setmetatable(L, -2);
    }
    else if(strcmp(key, "size") == 0)
    {
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *vec = window_size;
        luaL_getmetatable(L, "Vector2");
        lua_setmetatable(L, -2);
    }
    else if(strcmp(key, "title") == 0)
        lua_rawgeti(L, LUA_REGISTRYINDEX, window_title.ref);
    else
        lua_rawget(L, 1);

    return 1;
}

void Window_register(lua_State* L)
{
    luaL_newmetatable(L, "Window");

    lua_pushcfunction(L, Window_newindexLua);
    lua_setfield(L, -2, "__newindex");

    lua_pushcfunction(L, Window_indexLua);
    lua_setfield(L, -2, "__index");

    lua_pushvalue(L, -1);
    lua_setmetatable(L, -2);

    lua_setglobal(L, "Window");
}
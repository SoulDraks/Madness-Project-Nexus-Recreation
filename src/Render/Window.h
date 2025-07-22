#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "types/bool.h"
#include "types/Vector2.h"
#include "types/String.h"

extern SDL_Window* getWindow();
extern void Window_init();
extern bool Window_process();
extern void Window_destroy();
extern void Window_setPosition(const Vector2 newPosition);
extern void Window_setSize(const Vector2 newSize);
extern void Window_setTitle(String newTitle);
extern Vector2 Window_getPosition();
extern Vector2 Window_getSize();
extern String Window_getTitle();
extern void Window_register(lua_State* L);

#endif
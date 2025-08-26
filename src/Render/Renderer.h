#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// Obtiene el renderer global.
SDL_Renderer* GetRenderer();

// Inicializa todo de SDL.
extern int initSDL();

// Crea el Renderer.
extern int initRender(SDL_Window* window);

// Renderiza el UI.
void renderUI();

// void render_MadWorld(lua_State* L);

void renderDestroy();

#endif
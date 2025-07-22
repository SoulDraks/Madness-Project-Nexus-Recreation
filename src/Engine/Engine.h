#ifndef GAME_H
#define GAME_H

#include "Core/MadObject.h"

typedef struct {
    double deltaTime;
    unsigned int fps;
    MadObject* root; // Nodo raiz de todo.
} Engine;

extern Engine* Engine_getInstance();
extern void Engine_Init(lua_State* L);
// Maneja toda la logica del juego y su bucle.
extern void Engine_Loop();
extern void Engine_addObjectQueue(MadObject* obj, const bool freeAllChilds);
extern void Engine_register(lua_State* L);

#endif
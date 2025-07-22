#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "Lua/LuaManager.h"
#include "types/bool.h"
#include "Animation.h"

typedef struct {
    luaObjectRef refLua;
    Animation* animation;
    lua_State* L;
} AnimationRef;

typedef struct AnimationNode {
    bool queued;
    AnimationRef animationRef;
    struct AnimationNode* next; // Puntero al siguiente nodo.
} AnimationNode;

typedef struct {
    AnimationNode* head; // Primer nodo de la lista enlazada.
    int animationsCount; // Cuenta de animaciones en la lista.
} AnimationManager;

extern void AnimationManager_addAnimation(AnimationRef ref);
extern void AnimationManager_removeAnimation(const int anim_id);
extern void AnimationManager_process(const float delta);

#endif
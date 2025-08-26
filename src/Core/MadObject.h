#ifndef MADOBJECT_H
#define MADOBJECT_H
#include <SDL.h>
#include <stdio.h>
#include "types/String.h"
#include "Lua/State.h"

struct MadObject;

typedef void (*TickMethod)(void*, double);
typedef void (*DrawMethod)(void*, SDL_Renderer*);

typedef struct MadObjectRef {
    struct MadObject* obj;  // Puntero al MadObject real
    luaObjectRef ref; // Referencia a la instancia dentro de Lua
} MadObjectRef;

/* "Clase" base para todas las "Clases" dentro del juego. */
typedef struct MadObject {
    String name;
    Vector2 pos; // Posicion Relativa del Objeto respecto a su padre.
    Vector2 globalPos; // Posicion Absoluta del Objeto para poder dibujarlo.
    Vector2 size;
    Vector2 scale; // Escala relativa del Objeto respecto a su padre.
    Vector2 globalScale;  // Escala absoluta que se utilizara para dibujarlo.
    double rotation; // Rotacion relativa en grados del objeto
    double globalRotation; // Rotacion absoluta en grados del Objeto que se usara para dibujarlo.
    Color color; // Color propio del objeto.
    bool visible; 
    bool flip;
    TickMethod tick; /* Metodo "virtual" que es llamado cada frame para la logica. */
    /* <---- Parte privada ----> */
    DrawMethod draw; /* Metodo "virtual" que es llamado cada frame para dibujar el objeto. */
    Color globalColor; /* Color final que se utilizara para poder dibujar el objeto. */
    bool queued; // Bandera para saber si el objeto se encuentra en la cola para la eliminacion.
    struct MadObject* parent; /* Puntero directo a su padre para evitar una referencia ciclica en lua. */
    MadObjectRef first_child;
    struct MadObject* last_child; /* Puntero a su ultimo hijo para evitar recorrer la lista de hijos cada vez que añadamos uno */
    MadObjectRef next_sibling;
    lua_State* L;
    luaObjectRef self_reference; /* Referencia debil propia a la instancia dentro de lua */
    size_t childCount;
} MadObject;

#define lua_refMadObject(L, t) (MadObjectRef){(*(MadObject**)lua_checkinstance((L), lua_gettop((L)), "MadObject")), luaL_ref(L, (t))}
#define NULL_MADOBJECTREF (MadObjectRef){NULL, LUA_NOREF}
#define isNullMadObjectRef(_ref) ((_ref).obj == NULL || (_ref).ref == LUA_NOREF)
/* Macro para poder inicializar los atributos de la instancia del MadObject */
/* sin llamar innecesariamente a "MadObject_new" */
#define MADOBJECT_NEW(instance) \
    (instance)->name = String_new("MadObject"); \
    (instance)->pos = (Vector2){0.0f, 0.0f}; \
    (instance)->globalPos = (Vector2){0.0f, 0.0f}; \
    (instance)->size = (Vector2){0.0f, 0.0f}; \
    (instance)->scale = (Vector2){1.0f, 1.0f}; \
    (instance)->globalScale = (Vector2){1.0f, 1.0f}; \
    (instance)->globalRotation = 0.0f; \
    (instance)->rotation = 0.0f; \
    (instance)->color = (Color){255, 255, 255, 255}; \
    (instance)->globalColor = (Color){255, 255, 255, 255}; \
    (instance)->visible = true; \
    (instance)->flip = false; \
    (instance)->tick = NULL; \
    (instance)->parent = NULL; \
    (instance)->first_child = NULL_MADOBJECTREF; \
    (instance)->last_child = NULL; \
    (instance)->next_sibling = NULL_MADOBJECTREF; \
    (instance)->self_reference = LUA_NOREF; \
    (instance)->L = NULL; \
    (instance)->draw = NULL; \
    (instance)->queued = false; \
    (instance)->childCount = 0; \

/* Macro para poder inicializar la instancia dentro de lua. */
#define MADOBJECT_NEWLUA(var) \
    (var)->L = L; \
    lua_newtable(L); \
    lua_setuservalue(L, -2); \
    luaL_getmetatable(L, "weakRefs"); \
    lua_pushvalue(L, -2); \
    (var)->self_reference = luaL_ref(L, -2); \
    lua_pop(L, 1); \

extern MadObject* MadObject_new();
extern void MadObject_free(MadObject* self);
extern void MadObject_setPosition(MadObject* self, const Vector2 value);
extern void MadObject_setGlobalPosition(MadObject* self, const Vector2 value);
extern void MadObject_setScale(MadObject* self, const Vector2 value);
extern void MadObject_setGlobalScale(MadObject* self, const Vector2 value);
extern void MadObject_setRotation(MadObject* self, const double value);
extern void MadObject_setGlobalRotation(MadObject* self, const float value);
extern void MadObject_setColor(MadObject* self, const Color value);
extern void MadObject_callTick(MadObject* self, const double deltaTime);
extern void MadObject_addChild(MadObject* self, MadObjectRef newChild);
extern MadObjectRef MadObject_getChildByName(MadObject* self, String name);
extern MadObjectRef MadObject_getChildByIndex(MadObject* self, const int index);
extern void MadObject_removeChildByName(MadObject* self, String name);
extern void MadObject_removeChildByIndex(MadObject* self, const int index);
extern void MadObject_removeChildByPtr(MadObject* self, MadObject* child);
extern bool MadObject_isInsideTree(MadObject* self);
extern void MadObject_register(lua_State* L);
extern void lua_pushMadObject(lua_State* L, MadObject* obj);

#endif
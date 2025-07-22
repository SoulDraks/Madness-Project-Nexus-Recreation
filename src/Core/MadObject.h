#ifndef MADOBJECT_H
#define MADOBJECT_H
#include <SDL.h>
#include <stdio.h>
#include "types/String.h"
#include "Lua/LuaManager.h"

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
    float rotation; // Rotacion relativa en grados del objeto
    float globalRotation; // Rotacion absoluta en grados del Objeto que se usara para dibujarlo.
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

#define MADOBJECT_INDEXLUA(var) \
    if(strcmp(key, "pos") == 0) \
    { \
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2)); \
        *vec = (*(var))->pos; \
        luaL_getmetatable(L, "Vector2"); \
        lua_setmetatable(L, -2); \
    } \
    else if(strcmp(key, "globalPos") == 0) \
    { \
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2)); \
        *vec = (*(var))->globalPos; \
        luaL_getmetatable(L, "Vector2"); \
        lua_setmetatable(L, -2); \
    } \
    else if(strcmp(key, "size") == 0) \
    { \
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2)); \
        *vec = (*(var))->size; \
        luaL_getmetatable(L, "Vector2"); \
        lua_setmetatable(L, -2); \
    } \
    else if(strcmp(key, "scale") == 0) \
    { \
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2)); \
        *vec = (*(var))->scale; \
        luaL_getmetatable(L, "Vector2"); \
        lua_setmetatable(L, -2); \
    } \
    else if(strcmp(key, "globalScale") == 0) \
    { \
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2)); \
        *vec = (*(var))->globalScale; \
        luaL_getmetatable(L, "Vector2"); \
        lua_setmetatable(L, -2); \
    } \
    else if(strcmp(key, "color") == 0) \
    { \
        Color* color = (Color*)lua_newuserdata(L, sizeof(Color)); \
        *color = (*(var))->color; \
        luaL_getmetatable(L, "Color"); \
        lua_setmetatable(L, -2); \
    } \
    else if(strcmp(key, "globalColor") == 0) \
    { \
        Color* color = (Color*)lua_newuserdata(L, sizeof(Color)); \
        *color = (*(var))->globalColor; \
        luaL_getmetatable(L, "Color"); \
        lua_setmetatable(L, -2); \
    } \
    else if(strcmp(key, "rotation") == 0) \
    { \
        lua_pushnumber(L, (lua_Number)(*(var))->rotation); \
    } \
    else if(strcmp(key, "visible") == 0) \
    { \
        lua_pushboolean(L, (*(var))->visible); \
    } \
    else if(strcmp(key, "flip") == 0) \
    { \
        lua_pushboolean(L, (*(var))->flip); \
    } \
    else if(strcmp(key, "name") == 0) \
    { \
        lua_rawgeti(L, LUA_REGISTRYINDEX, (*(var))->name.ref); \
    } \
    else if(strcmp(key, "childCount") == 0) \
    { \
        lua_pushinteger(L, (lua_Integer)(*(var))->childCount); \
    } \

#define MADOBJECT_NEWINDEXLUA(var) \
    if(strcmp(key, "pos") == 0) \
    { \
        Vector2* value = (Vector2*)luaL_checkudata(L, 3, "Vector2"); \
        MadObject_setPosition((*(var)), *value); \
    } \
    else if(strcmp(key, "pos_x") == 0) \
    { \
        float value = (float)luaL_checknumber(L, 3); \
        MadObject_setPosition((*(var)), (Vector2){value, (*(var))->pos.y}); \
    } \
    else if(strcmp(key, "pos_y") == 0) \
    { \
        float value = (float)luaL_checknumber(L, 3); \
        MadObject_setPosition((*(var)), (Vector2){(*(var))->pos.x, value}); \
    } \
    else if(strcmp(key, "globalPos") == 0) \
    { \
        Vector2* value = (Vector2*)luaL_checkudata(L, 3, "Vector2"); \
        MadObject_setGlobalPosition((*(var)), *value); \
    } \
    else if(strcmp(key, "globalPos_x") == 0) \
    { \
        float value = (float)luaL_checknumber(L, 3); \
        MadObject_setGlobalPosition((*(var)), (Vector2){value, (*(var))->globalPos.y}); \
    } \
    else if(strcmp(key, "globalPos_y") == 0) \
    { \
        float value = (float)luaL_checknumber(L, 3); \
        MadObject_setGlobalPosition((*(var)), (Vector2){(*(var))->globalPos.x, value}); \
    } \
    else if(strcmp(key, "size") == 0) \
    { \
        Vector2* value = (Vector2*)luaL_checkudata(L, 3, "Vector2"); \
        (*(var))->size = *value; \
    } \
    else if(strcmp(key, "size_x") == 0) \
    { \
        float value = (float)luaL_checknumber(L, 3); \
        (*(var))->size.x = value; \
    } \
    else if(strcmp(key, "size_y") == 0) \
    { \
        float value = (float)luaL_checknumber(L, 3); \
        (*(var))->size.y = value; \
    } \
    else if(strcmp(key, "scale") == 0) \
    { \
        Vector2* value = (Vector2*)luaL_checkudata(L, 3, "Vector2"); \
        MadObject_setScale((*(var)), *value); \
    } \
    else if(strcmp(key, "scale_x") == 0) \
    { \
        float value = (float)luaL_checknumber(L, 3); \
        MadObject_setScale((*(var)), (Vector2){value, (*(var))->scale.y}); \
    } \
    else if(strcmp(key, "scale_y") == 0) \
    { \
        float value = (float)luaL_checknumber(L, 3); \
        MadObject_setScale((*(var)), (Vector2){(*(var))->scale.x, value}); \
    } \
    else if(strcmp(key, "globalScale") == 0) \
    { \
        Vector2* value = (Vector2*)luaL_checkudata(L, 3, "Vector2"); \
        MadObject_setGlobalScale((*(var)), *value); \
    } \
    else if(strcmp(key, "globalScale_x") == 0) \
    { \
        float value = (float)luaL_checknumber(L, 3); \
        MadObject_setGlobalScale((*(var)), (Vector2){value, (*(var))->globalScale.y}); \
    } \
    else if(strcmp(key, "globalScale_y") == 0) \
    { \
        float value = (float)luaL_checknumber(L, 3); \
        MadObject_setGlobalScale((*(var)), (Vector2){(*(var))->globalScale.x, value}); \
    } \
    else if(strcmp(key, "color") == 0) \
    { \
        Color* value = (Color*)luaL_checkudata(L, 3, "Color"); \
        MadObject_setColor((*(var)), *value); \
    } \
    else if(strcmp(key, "color_r") == 0) \
    { \
        int value = luaL_checkinteger(L, 3); \
        INT_TO_UINT8(value); \
        MadObject_setColor((*(var)), (Color){(Uint8)value, (*(var))->color.g, (*(var))->color.b, (*(var))->color.a}); \
    } \
    else if(strcmp(key, "color_g") == 0) \
    { \
        int value = luaL_checkinteger(L, 3); \
        INT_TO_UINT8(value); \
        MadObject_setColor((*(var)), (Color){(*(var))->color.r, (Uint8)value, (*(var))->color.b, (*(var))->color.a}); \
    } \
    else if(strcmp(key, "color_b") == 0) \
    { \
        int value = luaL_checkinteger(L, 3); \
        INT_TO_UINT8(value); \
        MadObject_setColor((*(var)), (Color){(*(var))->color.r, (*(var))->color.g, (Uint8)value, (*(var))->color.a}); \
    } \
    else if(strcmp(key, "color_a") == 0) \
    { \
        int value = luaL_checkinteger(L, 3); \
        INT_TO_UINT8(value); \
        MadObject_setColor((*(var)), (Color){(*(var))->color.r, (*(var))->color.g, (*(var))->color.b, (Uint8)value}); \
    } \
    else if(strcmp(key, "rotation") == 0) \
    { \
        float value = luaL_checknumber(L, 3); \
        MadObject_setRotation((*(var)), value); \
    } \
    else if(strcmp(key, "globalRotation") == 0) \
    { \
        float value = luaL_checknumber(L, 3); \
        MadObject_setGlobalRotation((*(var)), value); \
    } \
    else if(strcmp(key, "visible") == 0) \
    { \
        if(lua_isboolean(L, 3)) \
        { \
            int value = lua_toboolean(L, 3); \
            (*(var))->visible = value; \
        } \
    } \
    else if(strcmp(key, "flip") == 0) \
    { \
        if(lua_isboolean(L, 3)) \
        { \
            int value = lua_toboolean(L, 3); \
            (*(var))->flip = value; \
        } \
    } \
    else if(strcmp(key, "name") == 0) \
    { \
        if(lua_isstring(L, 3)) \
        { \
            String_free((*(var))->name); \
            (*(var))->name = String_newFromLua(L, 3); \
        } \
    } \

extern MadObject* MadObject_new();
extern void MadObject_free(MadObject* self);
extern void MadObject_setPosition(MadObject* self, const Vector2 value);
extern void MadObject_setGlobalPosition(MadObject* self, const Vector2 value);
extern void MadObject_setScale(MadObject* self, const Vector2 value);
extern void MadObject_setGlobalScale(MadObject* self, const Vector2 value);
extern void MadObject_setRotation(MadObject* self, const float value);
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
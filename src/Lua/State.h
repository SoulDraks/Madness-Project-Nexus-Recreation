#ifndef STATE_H
#define STATE_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "types/bool.h"
#include "types/Vector2.h"
#include "types/Color.h"

typedef int luaFuncRef;
typedef int luaObjectRef;

#define getWeakRefsTable(L) luaL_getmetatable(L, "weakRefs")
#define lua_checkVector2(L, i) (*(Vector2*)luaL_checkudata(L, (i), "Vector2"))
#define lua_checkColor(L, i) (*(Color*)luaL_checkudata(L, (i), "Color"))

// Devuelve el Estado Global de Lua 
extern lua_State* getLuaState();
// Inicializa el Estado de Lua
extern void Lua_Init();
// Llama a una funcion de lua almacenada en el ambito global y retorna la cantidad de valores devueltos por lua.
extern int callLuaFunction(lua_State* L, const char* name, int argCount, ...);
/** Llama a una funcion de lua a partir de su indice de referencia.
 * @param weakRef Indica si la funcion esta almacenada en el registro o en la tabla de referencias debiles.
 */
extern int callLuaFunction2(lua_State* L, luaFuncRef func, bool weakRef, int argCount, ...);
/** Llama a una funcion de lua almacenada en la tabla auxiliar(osea, en su uservalue) de un userdata pasando "self" automaticamente.
 * @param uDataRef Se debe pasar el indice de referencia del userdata y no un puntero directo.
 * @param weakRef se debe indicar si el userdata esta almacenada en el registro o en la tabla de referencias debiles.
 */
extern void callLuaFunction3(lua_State* L, const int uDataRef, bool weakRef, const char* name, int argCount, ...);
// Llama al __gc de un objeto en el índice dado del stack
extern void callLuagc(lua_State *L, int index);
extern void lua_executescript(lua_State* L, const char* path);
// Verifica si la instancia en la pila es de la "Clase" dada o de una derivada y devuelve NULL si no lo es. 
extern void* lua_testinstance(lua_State* L, int idx, const char* tname);
// Verifica si la instancia en la pila es de la "Clase" dada o de una derivada y lanza un error si no lo es.
extern void* lua_checkinstance(lua_State* L, int idx, const char* tname);
extern void lua_getDinamicField(lua_State *L, const int ud, const char *key);
extern void lua_setDinamicField(lua_State* L, const int ud, const char* key);
extern void lua_pushVector2(lua_State* L, Vector2 vec);
extern void lua_pushColor(lua_State* L, Color color);

#endif
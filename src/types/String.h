#ifndef STRING_H
#define STRING_H

#include "Lua/State.h"
#include "Lua/Bindings.h"

typedef struct {
    /* Atributos de solo lectura */
    const char* data; // Puntero al String.
    size_t size; // Longitud del String.
    luaObjectRef ref; // Referencia al string dentro del estado de lua.
    lua_State* L; // Referencia al estado de lua asociado al strin
} String;

#define NULLSTRING (String){NULL, 0, LUA_NOREF, NULL}
#define String_cmp(str1, str2) ((str1).data == (str2).data)
#define String_free(str) luaL_unref((str).L, LUA_REGISTRYINDEX, (str).ref)

extern String String_new(const char* str);
extern String String_newFromLua(lua_State* L, int idx);
extern String String_dup(String str);
extern String String_concat(String str1, String str2);

#endif
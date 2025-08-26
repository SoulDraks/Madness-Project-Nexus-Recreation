#include <string.h>
#include "String.h"

// Tamaño de buffer local para la concatenacion de strings
#define BUFFER_SIZE 512

String String_new(const char* str)
{
    lua_State* L = getLuaState();   
    if(str == NULL)
        return NULLSTRING;
    String newStr;
    newStr.data = lua_pushstring(L, str);
    newStr.size = strlen(str);
    newStr.ref = luaL_ref(L, LUA_REGISTRYINDEX);
    newStr.L = L;
    return newStr;
}

String String_newFromLua(lua_State* L, int idx)
{
    String newStr;
    newStr.data = luaL_checklstring(L, idx, &newStr.size);
    // Lo duplicamos y ponemos en la cima para no perderlo y quitarlo por referenciarlo
    lua_pushvalue(L, idx);
    newStr.ref = luaL_ref(L, LUA_REGISTRYINDEX);
    newStr.L = L;
    return newStr;
}

String String_dup(String str)
{
    lua_State* L = getLuaState();
    if(str.data == NULL)
        return NULLSTRING;
    String newStr;
    lua_rawgeti(L, LUA_REGISTRYINDEX, str.ref);
    newStr.data = lua_tostring(L, -1);
    newStr.size = str.size;
    newStr.ref = luaL_ref(L, LUA_REGISTRYINDEX);
    newStr.L = L;
    return newStr;
}

String String_concat(String str1, String str2)
{
    lua_State* L = getLuaState();
    String newStr;
    if(str1.size + str2.size > BUFFER_SIZE)
    {
        // Si es muy grande los dos strings, creamos el buffer en el heap.
        char* buffer = malloc(str1.size + str2.size + 1);
        memcpy(buffer, str1.data, str1.size);
        memcpy(buffer + str1.size, str2.data, str2.size);
        buffer[str1.size + str2.size] = '\0';
        // Lo ponemos en la pila de lua para que lua lo interne.
        newStr.data = lua_pushstring(L, buffer);
        newStr.size = str1.size + str2.size;
        newStr.ref = luaL_ref(L, LUA_REGISTRYINDEX);
        newStr.L = L;
        free(buffer);
    }
    else
    {
        // Si no, pues lo creamos en el stack.
        char buffer[BUFFER_SIZE + 1];
        memcpy(buffer, str1.data, str1.size);
        memcpy(buffer + str1.size, str2.data, str2.size);
        buffer[str1.size + str2.size] = '\0';
        newStr.data = lua_pushstring(L, buffer);
        newStr.size = str1.size + str2.size;
        newStr.ref = luaL_ref(L, LUA_REGISTRYINDEX);
        newStr.L = L;
    }
    return newStr;
}
#include "Vector2.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Lua/State.h"

Vector2 Vector2_add(const Vector2 v1, const Vector2 v2)
{
    return (Vector2){v1.x + v2.x, v1.y + v2.y};
}

Vector2 Vector2_sub(const Vector2 v1, const Vector2 v2)
{
    return (Vector2){v1.x - v2.x, v1.y - v2.y};
}

Vector2 Vector2_mul(const Vector2 value, const float scalar)
{
    return (Vector2){value.x * scalar, value.y * scalar};
}

Vector2 Vector2_div(const Vector2 value, const float scalar)
{
    if(scalar == 0.0f)
    {
        printf("ERROR: Division by Zero.");
        return (Vector2){0.0f, 0.0f};
    }
    return (Vector2){value.x / scalar, value.y / scalar};
}

int Vector2_eq(const Vector2 v1, const Vector2 v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

float Vector2_magnitude(const Vector2 self)
{
    return sqrt(self.x * self.x + self.y * self.y);
}

Vector2 Vector2_normalize(const Vector2 self)
{
    float mag = Vector2_magnitude(self);
    if(mag > 0.0f)
        return (Vector2){self.x / mag, self.y / mag};
    
    return (Vector2){0.0f, 0.0f};
}

const char* Vector2_tostring(const Vector2 self)
{
    int len_x = snprintf(NULL, 0, "%f", self.x);
    int len_y = snprintf(NULL, 0, "%f", self.y);
    int total_len = len_x + len_y + 5;
    char* Vector2_str = (char*)malloc(total_len);
    if (!Vector2_str) {
        return NULL;
    }
    snprintf(Vector2_str, total_len, "(%f, %f)", self.x, self.y);
    return Vector2_str;
}

/* <-------- BINDINGS --------> */

static int Vector2_newLua(lua_State* L)
{
    Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
    if(lua_isnumber(L, 1) && lua_isnumber(L, 2))
    {
        vec->x = (float)luaL_checknumber(L, 1);
        vec->y = (float)luaL_checknumber(L, 2);
    }
    else
    {
        vec->x = 0.0f;
        vec->y = 0.0f;
    }
    luaL_getmetatable(L, "Vector2");
    lua_setmetatable(L, -2);
    return 1;
}

static int Vector2_newindexLua(lua_State* L)
{
    Vector2* self = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    const char* key = luaL_checkstring(L, 2);
    if(lua_isnumber(L, 3))
    {
        float value = (float)luaL_checknumber(L, 3);
        if(key[0] == 'x')
            self->x = value;
        else if(key[0] == 'y')
            self->y = value;
    }
    return 0;
}

static int Vector2_indexLua(lua_State* L)
{
    Vector2* self = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    const char* key = luaL_checkstring(L, 2);
    if(key[0] == 'x')
        lua_pushnumber(L, (lua_Number)self->x);
    else if(key[0] == 'y')
        lua_pushnumber(L, (lua_Number)self->y);
    else if(strcmp(key, "magnitude") == 0 || strcmp(key, "normalize") == 0)
    {
        lua_getmetatable(L, 1);
        lua_getfield(L, -1, key);
        lua_remove(L, -2);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static int Vector2_addLua(lua_State* L)
{
    Vector2* v1 = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    Vector2* v2 = (Vector2*)luaL_checkudata(L, 2, "Vector2");
    Vector2* result = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
    *result = Vector2_add(*v1, *v2);
    luaL_getmetatable(L, "Vector2");
    lua_setmetatable(L, -2);
    return 1;
}

static int Vector2_subLua(lua_State* L)
{
    Vector2* v1 = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    Vector2* v2 = (Vector2*)luaL_checkudata(L, 2, "Vector2");
    Vector2* result = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
    *result = Vector2_sub(*v1, *v2);
    luaL_getmetatable(L, "Vector2");
    lua_setmetatable(L, -2);
    return 1;
}

static int Vector2_mulLua(lua_State* L)
{
    Vector2* v1 = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    if(lua_isnumber(L, 2))
    {
        float scalar = (float)luaL_checknumber(L, 2);
        Vector2* result = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *result = Vector2_mul(*v1, scalar);
        luaL_getmetatable(L, "Vector2");
        lua_setmetatable(L, -2);
        return 1;
    }
    else
        return luaL_error(L, "Error: expected a 'number' as second parameter, but received '%s'.", luaL_typename(L, 2));
}

static int Vector2_divLua(lua_State* L)
{
    Vector2* v1 = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    if(lua_isnumber(L, 2))
    {
        float scalar = (float)luaL_checknumber(L, 2);
        Vector2* result = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *result = Vector2_div(*v1, scalar);
        luaL_getmetatable(L, "Vector2");
        lua_setmetatable(L, -2);
        return 1;
    }
    else
        return luaL_error(L, "Error: expected a 'number' as second parameter, but received '%s'.", luaL_typename(L, 2));
}

static int Vector2_eqLua(lua_State* L)
{
    Vector2* v1 = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    Vector2* v2 = (Vector2*)luaL_checkudata(L, 2, "Vector2");
    int result = Vector2_eq(*v1, *v2);
    lua_pushboolean(L, result);
    return 1;
}

static int Vector2_magnitudeLua(lua_State* L)
{
    Vector2* self = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    float result = Vector2_magnitude(*self);
    lua_pushnumber(L, (lua_Number)result);
    return 1;
}

static int Vector2_normalizeLua(lua_State* L)
{
    Vector2* self = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    Vector2* result = (Vector2*)lua_newuserdata(L, sizeof(Vector2*));
    *result = Vector2_normalize(*self);
    luaL_getmetatable(L, "Vector2");
    lua_setmetatable(L, -2);
    return 1;
}

static int Vector2_tostringLua(lua_State* L)
{
    Vector2* self = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    const char* Vector2_str = Vector2_tostring(*self);
    lua_pushlstring(L, Vector2_str, strlen(Vector2_str));
    free((void*)Vector2_str);
    return 1;
}

void Vector2_register(struct lua_State* L)
{
    luaL_newmetatable(L, "Vector2");

    lua_pushcfunction(L, Vector2_newindexLua);
    lua_setfield(L, -2, "__newindex"); // Vector2.__newindex = Vector2_newindexLua

    lua_pushcfunction(L, Vector2_indexLua);
    lua_setfield(L, -2, "__index"); // Vector2.__index = Vector2_indexLua

    lua_pushcfunction(L, Vector2_addLua);
    lua_setfield(L, -2, "__add"); // Vector2.__add = Vector2_addLua

    // Vector2["__sub"] = Vector2_subLua
    lua_pushcfunction(L, Vector2_subLua);
    lua_setfield(L, -2, "__sub"); // Vector2.__sub = Vector2_subLua

    // Vector2["__mul"] = Vector2_mulLua
    lua_pushcfunction(L, Vector2_mulLua);
    lua_setfield(L, -2, "__mul"); // Vector2.__mul = Vector2_mulLua

    lua_pushcfunction(L, Vector2_divLua);
    lua_setfield(L, -2, "__div"); // Vector2.__div = Vector2_divLua

    lua_pushcfunction(L, Vector2_eqLua);
    lua_setfield(L, -2, "__eq"); // Vector2.__eq = Vector2_eqLua

    lua_pushcfunction(L, Vector2_tostringLua);
    lua_setfield(L, -2, "__tostring"); // Vector2.__tostring = Vector2_tostringLua

    lua_pushcfunction(L, Vector2_magnitudeLua);
    lua_setfield(L, -2, "magnitude"); // Vector2.magnitude = Vector2_magtinudeLua

    lua_pushcfunction(L, Vector2_normalizeLua);
    lua_setfield(L, -2, "normalize"); // Vector2.normalize = Vector2_normalizeLua

    lua_pushcfunction(L, Vector2_newLua);
    lua_setfield(L, -2, "new"); // Vector2.new = Vector2_newLua

    luaL_getmetatable(L, "Vector2");
    lua_setglobal(L, "Vector2");
    
    lua_pop(L, 1);
}
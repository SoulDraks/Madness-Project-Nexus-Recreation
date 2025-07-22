#include "types/Color.h"

static int hexToDec(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

static uint8_t parseChannel(const char* str)
{
    return (hexToDec(str[0]) << 4) | hexToDec(str[1]);
}


Color Color_add(const Color c1, const Color c2)
{
    return (Color){c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a};
}

Color Color_sub(const Color c1, const Color c2)
{
    return (Color){c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a};
}

Color Color_mul(const Color color, const float scalar)
{
    return (Color){color.r * scalar, color.g * scalar, color.b * scalar, color.a * scalar};
}

Color Color_div(const Color color, const float scalar)
{
    if(scalar == 0.0f)
    {
        printf("ERROR: Division by zero");
        return (Color){0, 0, 0, 0};
    }
    return (Color){color.r / scalar, color.g / scalar, color.b / scalar, color.a / scalar};
}

bool Color_eq(const Color c1, const Color c2)
{
    return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a);
}

const char* Color_tostring(const Color self)
{
    char* Color_str = (char*)malloc(10);
    if (!Color_str) {
        return NULL;
    }
    snprintf(Color_str, 10, "#%02X%02X%02X%02X", self.a, self.r, self.g, self.b);
    return Color_str;
}

/* <----- BINDINGS -------> */

static int Color_newLua(lua_State* L)
{
    Color* color = (Color*)lua_newuserdata(L, sizeof(Color));
    if(!lua_isnone(L, 1) && !lua_isnone(L, 2) && !lua_isnone(L, 3) && !lua_isnone(L, 4))
    {
        int r = luaL_checkinteger(L, 1);
        int g = luaL_checkinteger(L, 2);
        int b = luaL_checkinteger(L, 3);
        int a = luaL_checkinteger(L, 4);
        INT_TO_UINT8(r);
        INT_TO_UINT8(g);
        INT_TO_UINT8(b);
        INT_TO_UINT8(a);
        *color = (Color){r, g, b, a};
    }
    // Por si es un string, formatear el numero hexadecimal del color a decimal.
    else if(lua_isstring(L, 1))
    {
        const char* color_str = lua_tostring(L, 1);
        size_t str_size = strlen(color_str);
        if(str_size == 8 || str_size == 9)
        {
            if(color_str[0] == '#')
            {
                color->a = parseChannel(&color_str[1]);
                color->r = parseChannel(&color_str[3]);
                color->g = parseChannel(&color_str[5]);
                color->b = parseChannel(&color_str[7]);
            }
            else
            {
                color->a = parseChannel(&color_str[0]);
                color->r = parseChannel(&color_str[2]);
                color->g = parseChannel(&color_str[4]);
                color->b = parseChannel(&color_str[6]);
            }
        }
        else
            return luaL_error(L, "Error: Bad format of color.");
    }
    else
        *color = (Color){255, 255, 255, 255};
    
    luaL_getmetatable(L, "Color");
    lua_setmetatable(L, -2);
    return 1;
}

static int Color_indexLua(lua_State* L)
{
    Color* self = (Color*)luaL_checkudata(L, 1, "Color");
    const char* key = luaL_checkstring(L, 2);
    if(strcmp(key, "r") == 0)
        lua_pushinteger(L, (lua_Integer)self->r);
    else if(strcmp(key, "g") == 0)
        lua_pushinteger(L, (lua_Integer)self->g);
    else if(strcmp(key, "b") == 0)
        lua_pushinteger(L, (lua_Integer)self->b);
    else if(strcmp(key, "a") == 0)
        lua_pushinteger(L, (lua_Integer)self->a);
    else
        return luaL_error(L, "Error: Do not exist a property '%s' in type 'Color'.", key);
    return 1;
}

static int Color_newindexLua(lua_State* L)
{
    Color* self = (Color*)luaL_checkudata(L, 1, "Color");
    const char* key = luaL_checkstring(L, 2);
    if(strcmp(key, "r") == 0)
    {
        int value = luaL_checkinteger(L, 3);
        INT_TO_UINT8(value);
        self->r = (Uint8)value;
    }
    else if(strcmp(key, "g") == 0)
    {
        int value = luaL_checkinteger(L, 3);
        INT_TO_UINT8(value);
        self->g = (Uint8)value;
    }
    else if(strcmp(key, "b") == 0)
    {
        int value = luaL_checkinteger(L, 3);
        INT_TO_UINT8(value);
        self->b = (Uint8)value;
    }
    else if(strcmp(key, "a") == 0)
    {
        int value = luaL_checkinteger(L, 3);
        INT_TO_UINT8(value);
        self->a = (Uint8)value;
    }
    else
        return luaL_error(L, "Error: Do not exist a property '%s' in type 'Color'.", key);
    return 0;
}

static int Color_addLua(lua_State* L)
{
    Color* c1 = (Color*)luaL_checkudata(L, 1, "Color");
    Color* c2 = (Color*)luaL_checkudata(L, 2, "Color");
    Color* result = (Color*)lua_newuserdata(L, sizeof(Color));
    *result = Color_add(*c1, *c2);
    luaL_getmetatable(L, "Color");
    lua_setmetatable(L, -2);
    return 1;
}

static int Color_subLua(lua_State* L)
{
    Color* c1 = (Color*)luaL_checkudata(L, 1, "Color");
    Color* c2 = (Color*)luaL_checkudata(L, 2, "Color");
    Color* result = (Color*)lua_newuserdata(L, sizeof(Color));
    *result = Color_sub(*c1, *c2);
    luaL_getmetatable(L, "Color");
    lua_setmetatable(L, -2);
    return 1;
}

static int Color_mulLua(lua_State* L)
{
    Color* color = (Color*)luaL_checkudata(L, 1, "Color");
    float scalar = luaL_checknumber(L, 2);
    Color* result = (Color*)lua_newuserdata(L, sizeof(Color));
    *result = Color_mul(*color, scalar);
    luaL_getmetatable(L, "Color");
    lua_setmetatable(L, -2);
    return 1;
}

static int Color_divLua(lua_State* L)
{
    Color* color = (Color*)luaL_checkudata(L, 1, "Color");
    float scalar = luaL_checknumber(L, 2);
    Color* result = (Color*)lua_newuserdata(L, sizeof(Color*));
    *result = Color_div(*color, scalar);
    luaL_getmetatable(L, "Color");
    lua_setmetatable(L, -2);
    return 1;
}

static int Color_eqLua(lua_State* L)
{
    Color* c1 = (Color*)luaL_checkudata(L, 1, "Color");
    Color* c2 = (Color*)luaL_checkudata(L, 2, "Color");
    bool result = Color_eq(*c1, *c2);
    lua_pushboolean(L, result);
    return 1;
}

static int Color_tostringLua(lua_State* L)
{
    Color* self = (Color*)luaL_checkudata(L, 1, "Color");
    char Color_str[10];
    snprintf(Color_str, 10, "#%02X%02X%02X%02X", self->a, self->r, self->g, self->b);
    lua_pushstring(L, Color_str);
    return 1;
}

void Color_register(lua_State* L)
{
    luaL_newmetatable(L, "Color");

    lua_pushcfunction(L, Color_newLua);
    lua_setfield(L, -2, "new");

    lua_pushcfunction(L, Color_indexLua);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, Color_newindexLua);
    lua_setfield(L, -2, "__newindex");

    lua_pushcfunction(L, Color_addLua);
    lua_setfield(L, -2, "__add");

    lua_pushcfunction(L, Color_subLua);
    lua_setfield(L, -2, "__sub");

    lua_pushcfunction(L, Color_mulLua);
    lua_setfield(L, -2, "__mul");
    
    lua_pushcfunction(L, Color_divLua);
    lua_setfield(L, -2, "__div");

    lua_pushcfunction(L, Color_eqLua);
    lua_setfield(L, -2, "__eq");

    lua_pushcfunction(L, Color_tostringLua);
    lua_setfield(L, -2, "__tostring");

    lua_setglobal(L, "Color");
}
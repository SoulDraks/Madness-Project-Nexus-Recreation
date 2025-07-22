#include "SoulMath.h"
#include <stdlib.h>
#include <string.h>

void SoulMath_init()
{
    int* p = (int*)malloc(sizeof(int)); // Creamos un int dinamico y usamos su direccion como semilla.
    if(sizeof(p) == 4)
        srand((uint32_t)p);
    else if (sizeof(p) == 8)
        srand((uint64_t)p);
    free(p);
}

float SoulMath_calculate_rotation(Vector2 source, Vector2 target)
{
    float Xd = target.x - source.x;
    float Yd = target.y - source.y;
    float radAngle = atan2(Yd, Xd);
    float rotation_result = radAngle * 360 / 6.283185307179586 + 90;
    if(rotation_result > 180)
    {
        rotation_result = -180 + (rotation_result - 180);
    }
    return rotation_result;
}

float SoulMath_move_me2(const char* this_axis, const float this_rotation, const float this_speed)
{
    if (this_rotation > 180)
    {
        if (strcmp(this_axis, "y") == 0)
        {
            return this_speed * cos(0.017453292519943295 * this_rotation);
        }
        if (strcmp(this_axis, "x") == 0)
        {
            return -this_speed * sin(0.017453292519943295 * this_rotation);
        }
    }
    else
    {
        if (strcmp(this_axis, "y") == 0)
        {
            return -this_speed * cos(0.017453292519943295 * this_rotation);
        }
        if (strcmp(this_axis, "x") == 0)
        {
            return this_speed * sin(0.017453292519943295 * this_rotation);
        }
    }
    return 0.0f;
}

float SoulMath_get_position(Vector2 start, const float this_rotation, const float this_distance, const char* this_axis)
{
    if(strcmp(this_axis, "x") == 0)
    {
        if (this_rotation > 180)
        {
            return start.x - this_distance * sin(0.017453292519943295 * this_rotation);
        }
        return start.x + this_distance * sin(0.017453292519943295 * this_rotation);
    }
    if (strcmp(this_axis, "y") == 0)
    {
        if (this_rotation > 180)
        {
            return start.y + this_distance * cos(0.017453292519943295 * this_rotation);
        }
        return start.y - this_distance * cos(0.017453292519943295 * this_rotation);
    }
    return 0.0f;
}

float SoulMath_get_distance(const Vector2 point_1, const Vector2 point_2)
{
    float this_distance = sqrt(pow(point_2.x - point_1.x, 2) + pow(point_2.y - point_1.y, 2));
    return this_distance;
}

int SoulMath_randomNumber(const int min, const int max)
{
    return rand() % (max - min + 1) + min;
}

float SoulMath_randomNumber2(const float min, const float max)
{
    return (float)rand() / RAND_MAX * (max - min + 1) + min;
}

float SoulMath_getAxisDistance(float inStart, float inEnd)
{
    float AxisDistance = inStart - inEnd;
    if(inStart < inEnd)
    {
        AxisDistance *= -1;
    }
    return AxisDistance;
}

float SoulMath_getRotDistance(float inRot1, float inRot2)
{
    float RotDistance = 0.0f;
    if(inRot1 < inRot2 - 180)
    {
        inRot1 += 360;
    }
    if(inRot1 > inRot2 + 180)
    {
        inRot1 -= 360;
    }
    RotDistance = inRot1 - inRot2;
    if(RotDistance > 0)
    {
        RotDistance *= -1;
    }
    return RotDistance;
}

void SoulMath_returnNumberCommas(int inNum, char* formattedStr)
{
    char numStr[50];
    sprintf(numStr, "%d", inNum);

    int length = strlen(numStr);
    int count = 0;
    int index = 0;
    for (int i = length - 1; i >= 0; i--) {
        formattedStr[index++] = numStr[i];
        count++;
        if (count % 3 == 0 && i != 0) {
            formattedStr[index++] = ',';
        }
    }
    formattedStr[index] = '\0';

    int start = 0;
    int end = index - 1;
    while (start < end) {
        char temp = formattedStr[start];
        formattedStr[start] = formattedStr[end];
        formattedStr[end] = temp;
        start++;
        end--;
    }
}

bool SoulMath_isCollinding(const Vector2 rect_position_1, const Vector2 rect_position_2, const Vector2 rect_size_1, const Vector2 rect_size_2)
{
    if(rect_position_1.x + rect_size_1.x > rect_position_2.x &&
        rect_position_1.x < rect_position_2.x + rect_size_2.x &&
        rect_position_1.y + rect_size_1.y > rect_position_2.y &&
        rect_position_1.y < rect_position_2.y + rect_size_2.y)
    {
        return true;
    }
    return false;
}

bool SoulMath_isCollindingMouse(const int mouseX, const int mouseY, const Vector2 rect_position, const Vector2 rect_size)
{
    if(mouseX > rect_position.x && mouseX < rect_position.x + rect_size.x &&
        mouseY > rect_position.y && mouseY < rect_position.y + rect_size.y)
    {
        return true;
     }
    return false;
}

bool SoulMath_isOnScreen(const Vector2 object_pos, const Vector2 object_size)
{
    if (object_pos.x + object_size.x < 0 || object_pos.x > 850 || object_pos.y + object_size.y < 0 || object_pos.y > 550)
    {
        return false;
    }
    return true;
}

void SoulMath_pushInterpolableValue(lua_State* L, const InterpolableValue value)
{
    switch(value.type)
    {
        case TVECTOR2:
        {
            Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
            *vec = value.values.vec;
            luaL_getmetatable(L, "Vector2");
            lua_setmetatable(L, -2);
            break;
        }
        case TCOLOR:
        {
            Color* color = (Color*)lua_newuserdata(L, sizeof(Color));
            *color = value.values.color;
            luaL_getmetatable(L, "Color");
            lua_setmetatable(L, -2);
            break;
        }
        case TINT:
            lua_pushinteger(L, value.values.integer);
            break;
        case TFLOAT:
            lua_pushnumber(L, value.values.fl);
            break;
        default:
            break;
    }
}

InterpolableValue SoulMath_lerp(const InterpolableValue start, const InterpolableValue end, const double t)
{
    InterpolableValue final_value;
    switch(start.type)
    {
        case TVECTOR2:
            // final_value.values.vec = start + (end - start) * t
            final_value.values.vec = Vector2_add(start.values.vec, Vector2_mul(Vector2_sub(end.values.vec, start.values.vec), t));
            final_value.type = TVECTOR2;
            break;
        case TCOLOR:
            final_value.values.color = Color_add(start.values.color, Color_mul(Color_sub(end.values.color, start.values.color), t));
            final_value.type = TCOLOR;
            break;
        case TINT:
            final_value.values.integer = start.values.integer + (end.values.integer - start.values.integer) * t;
            final_value.type = TINT;
            break;
        case TFLOAT:
            final_value.values.fl = start.values.fl + (end.values.fl - start.values.fl) * t;
            final_value.type = TFLOAT;
            break;
        default:
            break;
    }
    return final_value;
}

InterpolableValue SoulMath_quadratic(const InterpolableValue start, const InterpolableValue end, const double t)
{
    InterpolableValue final_value;
    switch(start.type)
    {
        case TVECTOR2:
            // final_value.values.vec = start + (end - start) * (t * t)
            final_value.values.vec = Vector2_add(start.values.vec, Vector2_mul(Vector2_sub(end.values.vec, start.values.vec), t * t));
            final_value.type = TVECTOR2;
            break;
        case TCOLOR:
            final_value.values.color = Color_add(start.values.color, Color_mul(Color_sub(end.values.color, start.values.color), t * t));
            final_value.type = TCOLOR;
            break;
        case TINT:
            final_value.values.integer = start.values.integer + (end.values.integer - start.values.integer) * (t * t);
            final_value.type = TINT;
            break;
        case TFLOAT:
            final_value.values.fl = start.values.fl + (end.values.fl - start.values.fl) * (t * t);
            final_value.type = TFLOAT;
            break;
        default:
            break;
    }
    return final_value;
}

InterpolableValue SoulMath_cubic(const InterpolableValue start, const InterpolableValue end, const double t)
{
    InterpolableValue final_value;
    switch(start.type)
    {
        case TVECTOR2:
            // final_value.values.vec = start + (end - start) * (t * t)
            final_value.values.vec = Vector2_add(start.values.vec, Vector2_mul(Vector2_sub(end.values.vec, start.values.vec), t * t * t));
            final_value.type = TVECTOR2;
            break;
        case TCOLOR:
            final_value.values.color = Color_add(start.values.color, Color_mul(Color_sub(end.values.color, start.values.color), t * t * t));
            final_value.type = TCOLOR;
            break;
        case TINT:
            final_value.values.integer = start.values.integer + (end.values.integer - start.values.integer) * (t * t * t);
            final_value.type = TINT;
            break;
        case TFLOAT:
            final_value.values.fl = start.values.fl + (end.values.fl - start.values.fl) * (t * t * t);
            final_value.type = TFLOAT;
            break;
        default:
            break;
    }
    return final_value;
}

InterpolableValue SoulMath_sinusoidal(const InterpolableValue start, const InterpolableValue end, const double t)
{
    InterpolableValue final_value;
    float factor = (1 - cos(t * PI)) / 2;
    switch (start.type)
    {
        case TVECTOR2:
            final_value.values.vec = Vector2_add(start.values.vec, Vector2_mul(Vector2_sub(end.values.vec, start.values.vec), factor));
            final_value.type = TVECTOR2;
            break;
        case TCOLOR:
            final_value.values.color = Color_add(start.values.color, Color_mul(Color_sub(end.values.color, start.values.color), factor));
            final_value.type = TCOLOR;
            break;
        case TINT:
            final_value.values.integer = start.values.integer + (end.values.integer - start.values.integer) * factor;
            final_value.type = TINT;
            break;
        case TFLOAT:
            final_value.values.fl = start.values.fl + (end.values.fl - start.values.fl) * factor;
            final_value.type = TFLOAT;
            break;
        default:
            break;
    }
    return final_value;
}

static double easeInOut(const double t)
{
    if(t < 0.5)
        return 2 * t * t;
    else
        return -1 + (4 - 2 * t) * t;
}

InterpolableValue SoulMath_easeInOut(const InterpolableValue start, const InterpolableValue end, const double t)
{
    InterpolableValue finalValue;
    switch(start.type)
    {
        case TVECTOR2:
            finalValue.values.vec = Vector2_add(start.values.vec, Vector2_mul(Vector2_sub(end.values.vec, start.values.vec), easeInOut(t)));
            finalValue.type = TVECTOR2;
            break;
        case TCOLOR:
            finalValue.values.color = Color_add(start.values.color, Color_mul(Color_sub(end.values.color, start.values.color), easeInOut(t)));
            finalValue.type = TCOLOR;
            break;
        case TFLOAT:
            finalValue.values.fl = start.values.fl + (end.values.fl - start.values.fl) * easeInOut(t);
            finalValue.type = TFLOAT;
            break;
        case TINT:
            finalValue.values.integer = start.values.integer + (end.values.integer - start.values.integer) * easeInOut(t);
            finalValue.type = TINT;
            break;
    }
    return finalValue;
}

int SoulMath_calculate_rotationLua(lua_State* L)
{
    Vector2* source = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    Vector2* target = (Vector2*)luaL_checkudata(L, 2, "Vector2");
    float rotation_result = SoulMath_calculate_rotation(*source, *target);
    lua_pushnumber(L, (lua_Number)rotation_result);
    return 1;
}

int SoulMath_move_me2Lua(lua_State* L)
{
    const char* this_axis = luaL_checkstring(L, 1);
    float this_rotation = luaL_checknumber(L, 2);
    float this_speed = luaL_checknumber(L, 3);
    float result = SoulMath_move_me2(this_axis, this_rotation, this_speed);
    lua_pushnumber(L, (lua_Number)result);
    return 1;
}

int SoulMath_get_positionLua(lua_State* L)
{
    Vector2* start = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    float this_rotation = luaL_checknumber(L, 2);
    float this_distance = luaL_checknumber(L, 3);
    const char* this_axis = luaL_checkstring(L, 4);
    float result = SoulMath_get_position(*start, this_rotation, this_distance, this_axis);
    lua_pushnumber(L, (lua_Number)result);
    return 1;
}

int SoulMath_get_distanceLua(lua_State* L)
{
    Vector2* point_1 = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    Vector2* point_2 = (Vector2*)luaL_checkudata(L, 2, "Vector2");
    float result = SoulMath_get_distance(*point_1, *point_2);
    lua_pushnumber(L, (lua_Number)result);
    return 1;
}

int SoulMath_randomNumberLua(lua_State* L)
{
    int min = luaL_checkinteger(L, 1);
    int max = luaL_checkinteger(L, 2);
    int result = SoulMath_randomNumber(min, max);
    lua_pushnumber(L, (lua_Number)result);
    return 1;
}

int SoulMath_randomNumber2Lua(lua_State* L)
{
    float min = luaL_checknumber(L, 1);
    float max = luaL_checknumber(L, 2);
    float result = SoulMath_randomNumber2(min, max);
    lua_pushnumber(L, (lua_Number)result);
    return 1;
}

int SoulMath_getAxisDistanceLua(lua_State* L)
{
    float inStart = luaL_checknumber(L, 1);
    float inEnd = luaL_checknumber(L, 2);
    float AxisDistance = SoulMath_getAxisDistance(inStart, inEnd);
    lua_pushnumber(L, (lua_Number)AxisDistance);
    return 1;
}

int SoulMath_getRotDistanceLua(lua_State* L)
{
    float inRot1 = luaL_checknumber(L, 1);
    float inRot2 = luaL_checknumber(L, 2);
    float RotDistance = SoulMath_getRotDistance(inRot1, inRot2);
    lua_pushnumber(L, (lua_Number)RotDistance);
    return 1;
}

int SoulMath_returnNumberCommasLua(lua_State* L)
{
    int inNum = luaL_checkinteger(L, 1);
    char numStr[50];
    SoulMath_returnNumberCommas(inNum, numStr);
    lua_pushstring(L, numStr);
    return 1;
}

int SoulMath_isCollindingLua(lua_State* L)
{
    Vector2* rect_position_1 = (Vector2*)luaL_checkudata(L, 1, "Vector2");
    Vector2* rect_position_2 = (Vector2*)luaL_checkudata(L, 2, "Vector2");
    Vector2* rect_size_1 = (Vector2*)luaL_checkudata(L, 3, "Vector2");
    Vector2* rect_size_2 = (Vector2*)luaL_checkudata(L, 4, "Vector2");
    bool result = SoulMath_isCollinding(*rect_position_1, *rect_position_2, *rect_size_1, *rect_size_2);
    lua_pushboolean(L, result);
    return 1;
}

int SoulMath_isCollindingMouseLua(lua_State* L)
{
    int mouseX = luaL_checkinteger(L, 1);
    int mouseY = luaL_checkinteger(L, 2);
    Vector2* rect_position = (Vector2*)luaL_checkudata(L, 3, "Vector2");
    Vector2* rect_size = (Vector2*)luaL_checkudata(L, 4, "Vector2");
    bool result = SoulMath_isCollindingMouse(mouseX, mouseY, *rect_position, *rect_size);
    lua_pushboolean(L, result);
    return 1;
}

int SoulMath_isOnScreenLua(lua_State* L)
{
    Vector2** object_pos = (Vector2**)luaL_checkudata(L, 1, "Vector2");
    Vector2** object_size = (Vector2**)luaL_checkudata(L, 2, "Vector2");
    bool result = SoulMath_isOnScreen(**object_pos, **object_size);
    lua_pushboolean(L, result);
    return 1;
}

void SoulMath_register(lua_State* L)
{
    lua_newtable(L);

    lua_pushcfunction(L, SoulMath_calculate_rotationLua);
    lua_setfield(L, -2, "calculate_rotation");

    lua_pushcfunction(L, SoulMath_move_me2Lua);
    lua_setfield(L, -2, "move_me2");

    lua_pushcfunction(L, SoulMath_get_positionLua);
    lua_setfield(L, -2, "get_position");

    lua_pushcfunction(L, SoulMath_get_distanceLua);
    lua_setfield(L, -2, "get_distance");

    lua_pushcfunction(L, SoulMath_randomNumberLua);
    lua_setfield(L, -2, "randomNumber");

    lua_pushcfunction(L, SoulMath_randomNumber2Lua);
    lua_setfield(L, -2, "randomNumber2");

    lua_pushcfunction(L, SoulMath_getAxisDistanceLua);
    lua_setfield(L, -2, "getAxisDistance");

    lua_pushcfunction(L, SoulMath_getRotDistanceLua);
    lua_setfield(L, -2, "getRotDistance");

    lua_pushcfunction(L, SoulMath_returnNumberCommasLua);
    lua_setfield(L, -2, "returnNumberCommas");

    lua_pushcfunction(L, SoulMath_isCollindingLua);
    lua_setfield(L, -2, "isCollinding");

    lua_pushcfunction(L, SoulMath_isCollindingMouseLua);
    lua_setfield(L, -2, "isCollindingMouse");

    lua_pushcfunction(L, SoulMath_isOnScreenLua);
    lua_setfield(L, -2, "isOnScreen");

    lua_setglobal(L, "SoulMath");
}
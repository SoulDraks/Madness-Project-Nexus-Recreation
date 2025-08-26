#ifndef SOULMATH_H
#define SOULMATH_H

#include <math.h>
#include "Lua/State.h"

typedef enum {
    VECTOR2,
    COLOR,
    INT,
    FLOAT
} interpolableTypes;

union interpolableValues {
    Vector2 vec;
    Color color;
    float fl;
    int integer;
};

typedef struct {
    union interpolableValues values;
    interpolableTypes type;
} InterpolableValue;

#define is_interpolable_value(L, i) ((lua_isinteger((L), (i))) || (lua_isnumber((L), (i))) || (luaL_testudata((L), (i), "Vector2") != NULL) || (luaL_testudata((L), (i), "Color") != NULL))
#define PI 3.14159265358979323846
#define DEG2RAD(angleDegrees) ((angleDegrees) * (PI / 180.0f))

extern void SoulMath_init();
extern float SoulMath_calculate_rotation(Vector2 source, Vector2 target);
extern float SoulMath_move_me2(const char* this_axis, const float this_rotation, const float this_speed);
extern float SoulMath_get_position(Vector2 start, const float this_rotation, const float this_distance, const char* this_axis);
extern float SoulMath_get_distance(const Vector2 point_1, const Vector2 point_2);
extern int SoulMath_randomNumber(const int min, const int max);
extern float SoulMath_randomNumber2(const float min, const float max);
extern float SoulMath_getAxisDistance(float inStart, float inEnd);
extern float SoulMath_getRotDistance(float inRot1, float inRot2);
extern void SoulMath_returnNumberCommas(const int inNum, char* formattedStr);
extern bool SoulMath_isCollinding(const Vector2 rect_position_1, const Vector2 rect_position_2, const Vector2 rect_size_1, const Vector2 rect_size_2);
extern bool SoulMath_isCollindingMouse(const int mouseX, const int mouseY, const Vector2 rect_position, const Vector2 rect_size);
extern bool SoulMath_isOnScreen(const Vector2 object_pos, const Vector2 object_size);
// Empuja a la pila de Lua un InterpolableValue.
extern void SoulMath_pushInterpolableValue(lua_State* L, const InterpolableValue value);
extern InterpolableValue SoulMath_lerp(const InterpolableValue start, const InterpolableValue end, const double t);
extern InterpolableValue SoulMath_quadratic(const InterpolableValue start, const InterpolableValue end, const double t);
extern InterpolableValue SoulMath_cubic(const InterpolableValue start, const InterpolableValue end, const double t);
extern InterpolableValue SoulMath_sinusoidal(const InterpolableValue start, const InterpolableValue end, const double t);
extern InterpolableValue SoulMath_easeInOut(const InterpolableValue start, const InterpolableValue end, const double t);
extern void SoulMath_register(lua_State* L);

#endif
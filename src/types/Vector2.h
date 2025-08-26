#ifndef VECTOR2_H
#define VECTOR2_H

struct lua_State;

typedef struct {
    double x;
    double y;
} Vector2;

extern Vector2 Vector2_add(const Vector2 v1, const Vector2 v2);
extern Vector2 Vector2_sub(const Vector2 v1, const Vector2 v2);
extern Vector2 Vector2_mul(const Vector2 value, const float scalar);
extern Vector2 Vector2_div(const Vector2 value, const float scalar);
extern int Vector2_eq(const Vector2 v1, const Vector2 v2);
extern float Vector2_magnitude(const Vector2 self);
extern Vector2 Vector2_normalize(const Vector2 self);
extern const char* Vector2_tostring(const Vector2 self);
extern void Vector2_register(struct lua_State* L);

#endif
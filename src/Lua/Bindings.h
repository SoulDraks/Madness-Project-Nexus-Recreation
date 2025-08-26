#ifndef BINDINGS_H
#define BINDINGS_H

#include "State.h"
#include <stdarg.h>

typedef enum {
    FUNC,
    CONSTRUCTOR,
    DESTRUCTOR,
    EXTENDS,
    FIELD,
    FIELDREADONLY, // Indica que el campo o miembro de la "clase" solo es de lectura para Lua 
    FIELDGETTERSETTER,
    END // Indica que ahi termina el registro de la "clase".
} FieldTypeRegister;

typedef enum {
    TBOOL,
    TINT,
    TDOUBLE,
    TSTRING,
    TVECTOR2,
    TCOLOR,
    TMADOBJECTPTR,
    TMADOBJECTREF
} FieldType;

typedef struct {
    FieldType type;
    bool isSetterGetter;
    bool isReadOnly; // Indica si el campo es solo de lectura.
    bool isComponent; // Usado especificamente para los tipos "Vector2" y "Color"
    union {
        size_t offset; // Se usa en caso de que no haya setter o getter
        // Se usa en caso de que lo haya. 
        struct {
            void* getter;
            void* setter;
        } funcs;
    } data;
} FieldInfo;

extern void Lua_registerclass(const char* name, ...);
extern void Lua_registerenum(const char* name, int enums, ...);

#endif
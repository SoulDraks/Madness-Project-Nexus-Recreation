#include "Bindings.h"
#include "types/Vector2.h"
#include "types/String.h"
#include "types/bool.h"
#include "Core/MadObject.h"
#include <stddef.h>

typedef void (*intSetter)(void*, int);
typedef int (*intGetter)(void*);

typedef void (*doubleSetter)(void*, lua_Number);
typedef lua_Number (*doubleGetter)(void*);

typedef void (*StringSetter)(void*, String);
typedef String (*StringGetter)(void*);

typedef void (*Vector2Setter)(void*, Vector2);
typedef Vector2 (*Vector2Getter)(void*);

typedef void (*ColorSetter)(void*, Color);
typedef Color (*ColorGetter)(void*);

typedef void (*MadObjectPtrSetter)(void*, MadObject*);
typedef MadObject* (*MadObjectPtrGetter)(void*);

typedef void (*MadObjectRefSetter)(void*, MadObjectRef);
typedef MadObjectRef (*MadObjectRefGetter)(void*);

typedef void* (*ConstructorFunc)();
typedef void (*DestructorFunc)(void*);

static int CheckType(lua_State* L, int mt, int ud)
{
    lua_getfield(L, mt, "__name");
    const char* tname = lua_tostring(L, -1); // Obtenemos el nombre del "tipo" esperado.
    lua_pop(L, 1); // Sacamos el nombre que obtuvimos
    if(!lua_isuserdata(L, ud))
        return luaL_typeerror(L, ud, tname);
    lua_getmetatable(L, ud);
    if(!lua_rawequal(L, -1, mt)) // Es el tipo esperado?
        return luaL_typeerror(L, ud, tname);
    lua_pop(L, 1); // Sacamos la metatabla obtenida del userdata que recibimos.
    return 0;
}

// Constructor generico para cualquier "clase"
static int GenericConstructor(lua_State* L)
{
    ConstructorFunc func = (ConstructorFunc)lua_touserdata(L, lua_upvalueindex(1)); // Obtenemos el "constructor" de la "Clase".
    MadObject* instance = func();
    MadObject** instanceLua = lua_newuserdata(L, sizeof(void*));
    *instanceLua = instance;
    MADOBJECT_NEWLUA(instance);
    lua_pushvalue(L, lua_upvalueindex(2)); // Obtenemos la metatabla de la "clase".
    lua_setmetatable(L, -2);
    return 1;
}

static int DETENTE(FieldInfo* info, char* value)
{
    printf("XD");
    return 1;
}

// Funcion para lua que llama al "destructor" de la "Clase"
static int GenericDestructor(lua_State* L)
{
    DestructorFunc func = (DestructorFunc)lua_touserdata(L, lua_upvalueindex(1));
    int result = CheckType(L, lua_upvalueindex(2), 1);
    if(result != 0)
        return result;
    void** self = lua_touserdata(L, 1);
    if(*self != NULL)
    {
        func(*self);
        free(*self);
        *self = NULL;
    }
    return 0;
}

static int FieldGetter(lua_State* L)
{
    /* Verificamos si la metatabla del userdata es la que esperamos */
    int result = CheckType(L, lua_upvalueindex(1), 1);
    if(result != 0)
        return result;
    /* Una vez verificado, Obtenemos el valor que se nos solicito. */
    void** self = lua_touserdata(L, 1);
    size_t keyLen;
    const char* key = luaL_checklstring(L, 2, &keyLen);
    lua_getfield(L, lua_upvalueindex(1), "__fields"); // Obtenemos la tabla que tiene la info sobre los campos.
    lua_pushvalue(L, 2);
    int type = lua_gettable(L, -2);
    if(type == LUA_TUSERDATA)
    {
        FieldInfo* info = lua_touserdata(L, -1);
        lua_pop(L, 2);
        if(!info->isSetterGetter)
        {
            // Si no tiene getter, usamos su offset.
            char* value = *(char**)self + info->data.offset;
            switch(info->type)
            {
                case TBOOL:
                {
                    bool finalValue = *(bool*)value;
                    lua_pushboolean(L, finalValue);
                    break;
                }
                case TINT:
                {
                    int finalValue = *(int*)value;
                    lua_pushinteger(L, finalValue);
                    break;
                }
                case TDOUBLE:
                {
                    double finalValue = *(double*)value;
                    lua_pushnumber(L, finalValue);
                    break;
                }
                case TSTRING:
                {
                    String* str = (String*)value;
                    if(str->data != NULL)
                        lua_rawgeti(L, LUA_REGISTRYINDEX, str->ref);
                    else
                        lua_pushnil(L);
                    break;
                }
                case TVECTOR2:
                {
                    Vector2 finalValue = *(Vector2*)value;
                    lua_pushVector2(L, finalValue);
                    break;
                }
                case TCOLOR:
                {
                    Color finalValue = *(Color*)value;
                    lua_pushColor(L, finalValue);
                    break;
                }
                case TMADOBJECTPTR:
                {
                    MadObject* obj = *(MadObject**)value;
                    if(obj != NULL)
                    {
                        getWeakRefsTable(L);
                        lua_rawgeti(L, -1, obj->self_reference);
                        lua_remove(L, -2);
                    }
                    else
                        lua_pushnil(L);
                    break;
                }
                case TMADOBJECTREF:
                {
                    MadObjectRef ref = *(MadObjectRef*)value;
                    if(!isNullMadObjectRef(ref))
                        lua_rawgeti(L, LUA_REGISTRYINDEX, ref.ref);
                    else
                        lua_pushnil(L);
                    break;
                }
            }
        }
        else
        {
            // Sino, llamamos al getter y obtenemos su valor.
            switch(info->type)
            {
                case TINT:
                {
                    intGetter func = (intGetter)info->data.funcs.getter;
                    int value = func(*self);
                    lua_pushinteger(L, value);
                    break;
                }
                case TDOUBLE:
                {
                    doubleGetter func = (doubleGetter)info->data.funcs.getter;
                    double value = func(*self);
                    lua_pushnumber(L, value);
                    break;
                }
                case TSTRING:
                {
                    StringGetter func = (StringGetter)info->data.funcs.getter;
                    String str = func(*self);
                    if(str.data == NULL)
                        lua_pushnil(L);
                    else
                        lua_rawgeti(L, LUA_REGISTRYINDEX, str.ref);
                    break;
                }
                case TVECTOR2:
                {
                    Vector2Getter func = (Vector2Getter)info->data.funcs.getter;
                    Vector2 value = func(*self);
                    if(info->isComponent)
                    {
                        if(key[keyLen - 1] == 'x')
                            lua_pushnumber(L, value.x);
                        else
                            lua_pushnumber(L, value.y);
                    }
                    else
                        lua_pushVector2(L, value);
                    break;
                }
                case TCOLOR:
                {
                    ColorGetter func = (ColorGetter)info->data.funcs.getter;
                    Color value = func(*self);
                    if(info->isComponent)
                    {
                        size_t lastChar = keyLen - 1;
                        if(key[lastChar] == 'r')
                            lua_pushinteger(L, value.r);
                        else if(key[lastChar] == 'g')
                            lua_pushinteger(L, value.g);
                        else if(key[lastChar] == 'b')
                            lua_pushinteger(L, value.b);
                        else
                            lua_pushinteger(L, value.a);
                    }
                    else
                        lua_pushColor(L, value);
                    break;
                }
                case TMADOBJECTPTR:
                {
                    MadObjectPtrGetter func = (MadObjectPtrGetter)info->data.funcs.getter;
                    MadObject* obj = func(*self);
                    if(obj != NULL)
                    {
                        getWeakRefsTable(L);
                        lua_rawgeti(L, -1, obj->self_reference);
                        lua_remove(L, -2); // Quitamos la tabla de referencias debiles.
                    }
                    else
                        lua_pushnil(L);
                    break;
                }
                case TMADOBJECTREF:
                {
                    MadObjectRefGetter func = (MadObjectRefGetter)info->data.funcs.getter;
                    MadObjectRef obj = func(*self);
                    if(!isNullMadObjectRef(obj))
                        lua_rawgeti(L, LUA_REGISTRYINDEX, obj.ref);
                    else
                        lua_pushnil(L);
                }
            }
        }
    }
    else
    {
        lua_pop(L, 2);
        lua_getDinamicField(L, 1, key);
    }
    return 1;
}

static int FieldSetter(lua_State* L)
{
    int result = CheckType(L, lua_upvalueindex(1), 1);
    if(result != 0)
        return result;
    void** self = lua_touserdata(L, 1);
    size_t keyLen;
    const char* key = luaL_checklstring(L, 2, &keyLen);
    lua_getfield(L, lua_upvalueindex(1), "__fields");
    lua_pushvalue(L, 2);
    int type = lua_gettable(L, -2);
    if(type == LUA_TUSERDATA)
    {
        FieldInfo* info = lua_touserdata(L, -1);
        lua_pop(L, 2);
        if(info->isReadOnly)
            return luaL_error(L, "The field '%s' is read-only.", key);
        if(!info->isSetterGetter)
        {
            // Si no tiene setter, usamos su offset
            char* value = *(char**)self + info->data.offset;
            switch(info->type)
            {
                case TBOOL:
                    *(bool*)value = lua_toboolean(L, 3);
                    break;
                case TINT:
                    *(int*)value = luaL_checkinteger(L, 3);
                    break;
                case TDOUBLE:
                    *(double*)value = luaL_checknumber(L, 3);
                    break;
                case TSTRING:
                {
                    String* finalValue = (String*)value;
                    String_free(*finalValue);
                    *finalValue = String_newFromLua(L, 3);
                    break;
                }
                case TVECTOR2:
                    *(Vector2*)value = lua_checkVector2(L, 3);
                    break;
                case TCOLOR:
                    *(Color*)value = lua_checkColor(L, 3);
                    break;
                case TMADOBJECTPTR:
                    *(MadObject**)value = *(MadObject**)lua_checkinstance(L, 3, "MadObject");
                    break;
                case TMADOBJECTREF:
                {
                    MadObjectRef* finalValue = (MadObjectRef*)value;
                    if(!isNullMadObjectRef(*finalValue))
                        luaL_unref(L, LUA_REGISTRYINDEX, finalValue->ref);
                    finalValue->obj = *(MadObject**)lua_checkinstance(L, 3, "MadObject");
                    lua_pushvalue(L, 3);
                    finalValue->ref = luaL_ref(L, LUA_REGISTRYINDEX);
                    break;
                }
            }
        }
        else
        {
            // Sino, llamamos al setter
            switch(info->type)
            {
                case TINT:
                {
                    intSetter func = (intSetter)info->data.funcs.setter;
                    func(*self, luaL_checkinteger(L, 3));
                    break;
                }
                case TDOUBLE:
                {
                    doubleSetter func = (doubleSetter)info->data.funcs.setter;
                    func(*self, luaL_checknumber(L, 3));
                    break;
                }
                case TSTRING:
                {
                    StringSetter func = (StringSetter)info->data.funcs.setter;
                    func(*self, String_newFromLua(L, 3));
                    break;
                }
                case TVECTOR2:
                {
                    Vector2Setter func = (Vector2Setter)info->data.funcs.setter;
                    if(info->isComponent)
                    {
                        size_t lastChar = keyLen - 1;
                        Vector2Getter funcGetter = (Vector2Getter)info->data.funcs.getter;
                        Vector2 value = funcGetter(*self);
                        if(key[lastChar] == 'x')
                            func(*self, (Vector2){luaL_checknumber(L, 3), value.y});
                        else
                            func(*self, (Vector2){value.x, luaL_checknumber(L, 3)});
                    }
                    else
                        func(*self, lua_checkVector2(L, 3));
                    break;
                }
                case TCOLOR:
                {
                    ColorSetter func = (ColorSetter)info->data.funcs.setter;
                    if(info->isComponent)
                    {
                        size_t lastChar = keyLen - 1;
                        ColorGetter funcGetter = (ColorGetter)info->data.funcs.getter;
                        Color value = funcGetter(*self);
                        if(key[lastChar] == 'r')
                            func(*self, (Color){luaL_checkinteger(L, 3), value.g, value.b, value.a});
                        else if(key[lastChar] == 'g')
                            func(*self, (Color){value.r, luaL_checkinteger(L, 3), value.b, value.a});
                        else if(key[lastChar] == 'b')
                            func(*self, (Color){value.r, value.g, luaL_checkinteger(L, 3), value.a});
                        else
                            func(*self, (Color){value.r, value.g, value.b, luaL_checkinteger(L, 3)});
                    }
                    else
                        func(*self, lua_checkColor(L, 3));
                    break;
                }
                case TMADOBJECTPTR:
                {
                    MadObjectPtrSetter func = (MadObjectPtrSetter)info->data.funcs.setter;
                    func(*self, *(MadObject**)lua_checkinstance(L, 3, "MadObject"));
                    break;
                }
                case TMADOBJECTREF:
                {
                    MadObjectRefSetter func = (MadObjectRefSetter)info->data.funcs.setter;
                    MadObjectRef ref;
                    ref.obj = *(MadObject**)lua_checkinstance(L, 3, "MadObject");
                    lua_pushvalue(L, 3);
                    ref.ref = luaL_ref(L, LUA_REGISTRYINDEX);
                    func(*self, ref);
                    break;
                }
            }
        }
    }
    else
    {
        lua_pop(L, 2);
        lua_setDinamicField(L, 1, key);
    }
    return 0;
}

/* 
    Crea unos FieldInfo extra para el campo indicado para los componentes de Vector2
    para los campos "name_x" y "'name'_y"
*/
static inline void CreateVector2Components(lua_State* L, int tfield, const char* name, FieldInfo* info)
{
    tfield = lua_absindex(L, tfield);
    size_t nameLen = strlen(name);
    char nameVec[nameLen + 3];
    snprintf(nameVec, sizeof(nameVec), "%s_x", name);
    FieldInfo infoX;
    FieldInfo infoY;
    infoX.isReadOnly = info->isReadOnly;
    infoX.isSetterGetter = false;
    infoX.isComponent = true;
    infoX.type = TDOUBLE;
    infoX.data.offset = info->data.offset + offsetof(Vector2, x);
    infoY.isReadOnly = info->isReadOnly;
    infoY.isSetterGetter = false;
    infoY.isComponent = true;
    infoY.type = TDOUBLE;
    infoY.data.offset = info->data.offset + offsetof(Vector2, y);
    FieldInfo* infoXLua = (FieldInfo*)lua_newuserdata(L, sizeof(FieldInfo));
    *infoXLua = infoX;
    lua_setfield(L, tfield, nameVec);
    nameVec[nameLen + 1] = 'y'; // Cambiamos la ultima letra de "x" a "y"
    FieldInfo* infoYLua = (FieldInfo*)lua_newuserdata(L, sizeof(FieldInfo));
    *infoYLua = infoY;
    lua_setfield(L, tfield, nameVec);
}

static inline void CreateColorComponents(lua_State* L, int tfield, const char* name, FieldInfo* info)
{
    tfield = lua_absindex(L, tfield);
    size_t nameLen = strlen(name);
    char nameColor[nameLen + 3];
    snprintf(nameColor, sizeof(nameColor), "%s_r", name);
    FieldInfo infoR;
    FieldInfo infoG;
    FieldInfo infoB;
    FieldInfo infoA;
    infoR.isReadOnly = info->isReadOnly;
    infoR.isSetterGetter = false;
    infoR.isComponent = true;
    infoR.type = TINT;
    infoR.data.offset = info->data.offset + offsetof(Color, r);
    infoG.isReadOnly = info->isReadOnly;
    infoG.isSetterGetter = false;
    infoG.isComponent = true;
    infoG.type = TINT;
    infoG.data.offset = info->data.offset + offsetof(Color, g);
    infoB.isReadOnly = info->isReadOnly;
    infoB.isSetterGetter = false;
    infoB.isComponent = true;
    infoB.type = TINT;
    infoB.data.offset = info->data.offset + offsetof(Color, b);
    infoA.isReadOnly = info->isReadOnly;
    infoA.isSetterGetter = false;
    infoA.isComponent = true;
    infoA.type = TINT;
    infoA.data.offset = info->data.offset + offsetof(Color, a);
    FieldInfo* infoRLua = (FieldInfo*)lua_newuserdata(L, sizeof(FieldInfo));
    *infoRLua = infoR;
    lua_setfield(L, tfield, nameColor);
    nameColor[nameLen + 1] = 'g'; // Cambiamos la ultima letra de "r" a "g"
    FieldInfo* infoGLua = (FieldInfo*)lua_newuserdata(L, sizeof(FieldInfo));
    *infoGLua = infoG;
    lua_setfield(L, tfield, nameColor);
    nameColor[nameLen + 1] = 'b';
    FieldInfo* infoBLua = (FieldInfo*)lua_newuserdata(L, sizeof(FieldInfo));
    *infoBLua = infoB;
    lua_setfield(L, tfield, nameColor);
    nameColor[nameLen + 1] = 'a';
    FieldInfo* infoALua = (FieldInfo*)lua_newuserdata(L, sizeof(FieldInfo));
    *infoALua = infoA;
    lua_setfield(L, tfield, nameColor);
}

static inline void CreateVector2ComponentsGetterSetter(lua_State* L, int tfield, const char* name, FieldInfo* info)
{
    tfield = lua_absindex(L, tfield);
    size_t nameLen = strlen(name);
    char nameVec[nameLen + 3];
    snprintf(nameVec, sizeof(nameVec), "%s_x", name);
    FieldInfo* infoX_Y = lua_newuserdata(L, sizeof(FieldInfo));
    *infoX_Y = *info;
    infoX_Y->isComponent = true;
    lua_pushvalue(L, -1);
    lua_setfield(L, tfield, nameVec);
    nameVec[nameLen + 1] = 'y';
    lua_setfield(L, tfield, nameVec);
}

static inline void CreateColorComponentsGetterSetter(lua_State* L, int tfield, const char* name, FieldInfo* info)
{
    tfield = lua_absindex(L, tfield);
    size_t nameLen = strlen(name);
    char nameColor[nameLen + 3];
    snprintf(nameColor, sizeof(nameColor), "%s_r", name);
    FieldInfo* infoR_G_B_A = (FieldInfo*)lua_newuserdata(L, sizeof(FieldInfo));
    *infoR_G_B_A = *info;
    infoR_G_B_A->isComponent = true;
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, tfield, nameColor);
    nameColor[nameLen + 1] = 'g';
    lua_setfield(L, tfield, nameColor);
    nameColor[nameLen + 1] = 'b';
    lua_setfield(L, tfield, nameColor);
    nameColor[nameLen + 1] = 'a';
    lua_setfield(L, tfield, nameColor);
}

void Lua_registerclass(const char* name, ...)
{
    lua_State* L = getLuaState();
    va_list args;
    va_start(args, name);
    luaL_newmetatable(L, name);
    lua_createtable(L, 0, 5); // Tabla que almacenara los datos sobre los atributos de la "clase".
    for(;;)
    {
        FieldTypeRegister registerType = va_arg(args, FieldTypeRegister);
        switch(registerType)
        {
            case FUNC:
            {
                const char* fieldname = va_arg(args, const char*);
                lua_CFunction func = va_arg(args, lua_CFunction);
                lua_pushcfunction(L, func);
                lua_setfield(L, -3, fieldname);
                break;
            }
            case CONSTRUCTOR:
            {
                ConstructorFunc func = va_arg(args, ConstructorFunc);
                lua_pushlightuserdata(L, func);
                lua_pushvalue(L, -3);
                lua_pushcclosure(L, GenericConstructor, 2);
                lua_setfield(L, -3, "new");
                break;
            }
            case DESTRUCTOR:
            {
                DestructorFunc func = va_arg(args, DestructorFunc);
                lua_pushlightuserdata(L, func);
                lua_pushvalue(L, -3);
                lua_pushcclosure(L, GenericDestructor, 2);
                lua_setfield(L, -3, "__gc");
                break;
            }
            case EXTENDS:
            {
                const char* class = va_arg(args, const char*);
                luaL_getmetatable(L, class);
                lua_getfield(L, -1, "__fields");
                if(!lua_isnil(L, -1))
                {
                    // Duplicamos la tabla que almacena la informacion de los campos de la "clase" base.
                    int idxclass = lua_absindex(L, -3); // Pasamos su indice a absoluto para la iteracion en la tabla
                    int idxbase = lua_absindex(L, -1); // Indice absoluto de la tabla de campos de la "clase" base
                    lua_pushnil(L);
                    while(lua_next(L, idxbase) != 0)
                    {
                        lua_pushvalue(L, -2); // Duplicamos la clave
                        lua_pushvalue(L, -2); // Duplicamos el valor
                        lua_settable(L, idxclass);
                        lua_pop(L, 1);
                    }
                }
                lua_pop(L, 1); // Quitamos de la pila la tabla de campos de la "clase" base.
                lua_setfield(L, -3, "__extends");
                break;
            }
            case FIELD: case FIELDREADONLY:
            {
                const char* fieldname = va_arg(args, const char*);
                FieldInfo info;
                info.isReadOnly = registerType == FIELDREADONLY;
                info.isSetterGetter = false;
                info.isComponent = false;
                info.type = va_arg(args, FieldType);
                info.data.offset = va_arg(args, size_t);
                if(info.type == TVECTOR2)
                    CreateVector2Components(L, -1, fieldname, &info);
                else if(info.type == TCOLOR)
                    CreateColorComponents(L, -1, fieldname, &info);
                FieldInfo* infoLua = (FieldInfo*)lua_newuserdata(L, sizeof(FieldInfo));
                *infoLua = info;
                lua_setfield(L, -2, fieldname);
                break;
            }
            case FIELDGETTERSETTER:
            {
                const char* fieldname = va_arg(args, const char*);
                FieldInfo info;
                info.isSetterGetter = true;
                info.isComponent = false;
                info.type = va_arg(args, FieldType);
                info.data.funcs.getter = va_arg(args, void*);
                info.data.funcs.setter = va_arg(args, void*);
                if(info.data.funcs.getter == NULL)
                    printf("Warning: The field '%s' of the class '%s' has no getter function.\n", fieldname, name);
                if(info.data.funcs.setter == NULL)
                    info.isReadOnly = true;
                else
                    info.isReadOnly = false;
                if(info.type == TVECTOR2)
                    CreateVector2ComponentsGetterSetter(L, -1, fieldname, &info);
                else if(info.type == TCOLOR)
                    CreateColorComponentsGetterSetter(L, -1, fieldname, &info);
                FieldInfo* infoLua = (FieldInfo*)lua_newuserdata(L, sizeof(FieldInfo));
                *infoLua = info;
                lua_setfield(L, -2, fieldname);
                break;
            }
            case END:
                goto breakall;
                break;
        }
    }
    breakall:
    va_end(args);
    lua_setfield(L, -2, "__fields");
    lua_pushvalue(L, -1);
    lua_pushcclosure(L, FieldGetter, 1);
    lua_setfield(L, -2, "__index");
    lua_pushvalue(L, -1);
    lua_pushcclosure(L, FieldSetter, 1);
    lua_setfield(L, -2, "__newindex");
    lua_setglobal(L, name);
}

void Lua_registerenum(const char* name, int enums, ...)
{
    lua_State* L = getLuaState(L);
    va_list args;
    va_start(args, enums);
    lua_createtable(L, 0, enums);
    for(int i = 0; i < enums; i++)
    {
        const char* enumName = va_arg(args, const char*);
        int enumValue = va_arg(args, int);
        lua_pushinteger(L, enumValue);
        lua_setfield(L, -2, enumName);
    }
    lua_setglobal(L, name);
}
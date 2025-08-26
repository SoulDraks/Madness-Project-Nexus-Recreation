#include "State.h"
#include "Math/SoulMath.h"
#include "Core/MadObject.h"
#include "Core/UI/Panel.h"
#include "Core/UI/Image.h"
#include "Core/UI/Button.h"
#include "Core/UI/Label.h"
#include "Core/UI/Gif.h"
#include "Core/Animation/Animation.h"
#include "Render/Window.h"
#include "Engine/Engine.h"

// Estado Global de Lua
static lua_State* gL = NULL;

// Funcion auxiliar que registra todas las "clases" a lua.
static void initialize_classes()
{
    Vector2_register(gL);
    Color_register(gL);
    SoulMath_register(gL);
    MadObject_register(gL);
    Panel_register(gL);
    Image_register(gL);
    Button_register(gL);
    Label_register(gL);
    Gif_register(gL);
    Animation_register(gL);
    Window_register(gL);
    Engine_register(gL);
}

lua_State* getLuaState()
{
    return gL;
}

// Funcion auxiliar para lua para lanzar un error en caso de que se quiera realizar una operacion con un objeto ya liberado.
static int freedObjectMethodError(lua_State* L)
{
    return luaL_error(L, "Error: Attempt to use an object that has already been released.");
    abort();
}

// Funcion auxiliar para lua para que devuelva nil en caso de querer acceder a el.
static int freedObjectMethodIndex(lua_State* L)
{
    lua_pushnil(L);
    return 1;
}

static int freedObjectMethodToString(lua_State* L)
{
    lua_pushstring(L, "nil");
    return 1;
}

static int freedObjectMethodEq(lua_State* L)
{
    if(lua_isnil(L, 2))
        lua_pushboolean(L, true);
    else
        lua_pushboolean(L, false);
    return 1;
}

// Funcion auxiliar que procesa los argumentos extra que se pasaron y los pone en la pila.
static void processExtraArguments(lua_State *L, const int argCount, va_list args)
{
    if(argCount <= 0)
        return;
    for (int i = 0; i < argCount; i++)
    {
        int type = va_arg(args, int);
        switch (type)
        {
        case LUA_TNUMBER:
            lua_pushnumber(L, va_arg(args, double));
            break;
        case LUA_TBOOLEAN:
            lua_pushboolean(L, va_arg(args, int));
            break;
        case LUA_TSTRING:
            lua_pushstring(L, va_arg(args, const char*));
            break;
        case LUA_TNIL:
            lua_pushnil(L);
            break;
        case LUA_TUSERDATA: case LUA_TTABLE:
        {
            int refID = va_arg(args, int);
            bool isWeakRef = va_arg(args, bool);
            if(isWeakRef)
            {
                getWeakRefsTable(L);
                lua_rawgeti(L, -1, refID);
                lua_remove(L, -2);
            }
            else
                lua_rawgeti(L, LUA_REGISTRYINDEX, refID);
            break;
        }
        default:
            printf("Tipo de argumento no soportado: %d\n", type);
            break;
        }
    }
    va_end(args);
}

void Lua_Init()
{
    gL = luaL_newstate();
    // Crear la tabla de referencias debiles.
    luaL_newmetatable(gL, "weakRefs");

    lua_pushstring(gL, "v");
    lua_setfield(gL, -2, "__mode"); // Hacer que los valores de la tabla solo puedan almacenar referencias debiles.

    lua_pushvalue(gL, -1);
    lua_setmetatable(gL, -2);

    lua_pop(gL, 1);
    // Crear la metatabla "freedObject" para los objetos liberados
    luaL_newmetatable(gL, "freedObject");

    lua_pushcfunction(gL, freedObjectMethodError);
    lua_setfield(gL, -2, "__call");

    lua_pushcfunction(gL, freedObjectMethodIndex);
    lua_setfield(gL, -2, "__index");

    lua_pushcfunction(gL, freedObjectMethodError);
    lua_setfield(gL, -2, "__newindex");

    lua_pushcfunction(gL, freedObjectMethodToString);
    lua_setfield(gL, -2, "__tostring");

    lua_pushcfunction(gL, freedObjectMethodEq);
    lua_setfield(gL, -2, "__eq");

    lua_pop(gL, 1);

    luaL_openlibs(gL);
    initialize_classes();
}

int callLuaFunction(lua_State *L, const char *name, int argCount, ...)
{
    int base = lua_gettop(L); // Guardamos el estado actual de la pila
    lua_getglobal(L, name);
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 1); // Sacar el valor nulo o basura que obtuvimos.
        return 0;
    }
    va_list args;
    va_start(args, argCount);
    processExtraArguments(L, argCount, args);
    if (lua_pcall(L, argCount, LUA_MULTRET, 0) != LUA_OK)
    {
        printf("Error in function '%s': %s\n", name, lua_tostring(L, -1));
        lua_pop(L, 1); // Limpiar mensaje de error
        return 0;
    }
    // Calcular la cantidad de valores retornados de la funcion de lua.
    int nret = lua_gettop(L) - base;
    return nret;
}

int callLuaFunction2(lua_State *L, luaFuncRef func, bool weakRef, int argCount, ...)
{
    int base = lua_gettop(L);
    if (weakRef)
    {
        getWeakRefsTable(L);
        lua_rawgeti(L, -1, func);
        lua_remove(L, -2); // Eliminar la tabla "weakRefs" de la pila.
    }
    else
        lua_rawgeti(L, LUA_REGISTRYINDEX, func);

    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 1); // Sacamos el valor basura de la pila.
        return 0;
    }
    va_list args;
    va_start(args, argCount);
    processExtraArguments(L, argCount, args);
    if (lua_pcall(L, argCount, 0, 0) != LUA_OK)
    {
        // Si fallo, mostrar el error.
        lua_Debug ar;
        const char* funcName = NULL;
        // Obtener el nombre de la funcion si es que tiene.
        if(lua_getstack(L, 1, &ar))
        {
            lua_getinfo(L, "n", &ar);
            funcName = ar.name;
        }
        if(funcName != NULL)
            printf("Error to execute the function '%s': %s\n", funcName, lua_tostring(L, -1));
        else
            printf("Error to execute a function: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1); // Limpiar mensaje de error
        return 0;
    }
    int nret = lua_gettop(L) - base;
    return nret;
}

void callLuaFunction3(lua_State *L, const int uDataRef, bool weakRef, const char *name, int argCount, ...)
{
    if(weakRef) // es una referencia debil?
    {
        // Buscarla en la tabla de referencias debiles si lo es.
        getWeakRefsTable(L);
        lua_rawgeti(L, -1, uDataRef);
        lua_remove(L, -2); // Eliminar la tabla "weakRefs" de la pila.
    }
    else
        lua_rawgeti(L, LUA_REGISTRYINDEX, uDataRef); // Buscarla en el registro global si no lo es.

    lua_getuservalue(L, -1);
    lua_getfield(L, -1, name);
    if(lua_isfunction(L, -1))
    {
        // Si es una funcion, llamarla.
        lua_pushvalue(L, -3); // Poner a "self" en la cima.
        va_list args;
        va_start(args, argCount);
        processExtraArguments(L, argCount, args);
        if (lua_pcall(L, 1 + argCount, 0, 0) != LUA_OK) // + 1 por el parametro "self".
        {
            printf("Error in function '%s': %s\n", name, lua_tostring(L, -1));
            lua_pop(L, 1); // Limpiar mensaje de error
        }
        lua_pop(L, 2); // Limpiar "self" y el uservalue de la pila.
    }
    else
        lua_pop(L, 3); // Si no lo es, limpiar todo y no hacer nada.
}

void callLuagc(lua_State *L, int index)
{
    index = lua_absindex(L, index);
    if(!lua_getmetatable(L, index)) // Tiene metatabla?
        return;
    lua_getfield(L, -1, "__gc");
    if(lua_isfunction(L, -1))
    {
        lua_pushvalue(L, index); // pasar obj como argumento
        lua_call(L, 1, 0); // llamar a __gc(obj)
    }
    else
        lua_pop(L, 1); // sacar __gc si no es función

    lua_pop(L, 1); // sacar metatabla
}

void lua_executescript(lua_State* L, const char* path)
{
    if(luaL_dofile(L, path) != LUA_OK)
    {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // Limpiar el error de la pila
    }
    lua_gc(L, LUA_GCCOLLECT, 0); // Recolectar basura por seguridad.
    lua_gc(L, LUA_GCCOLLECT, 0); // Otra mas por si quedo algo.
}

void* lua_testinstance(lua_State *L, int idx, const char *tname)
{
    idx = lua_absindex(L, idx);
    void* instance = lua_touserdata(L, idx);
    bool clean = true; // Bandera para saber si debo limpiar XD.
    if(instance == NULL) // ¿No es un userdata?
        return NULL;
    luaL_getmetatable(L, tname);   // Obtener la metatabla dada.
    if(!lua_getmetatable(L, idx)) // No tiene una metatabla?
    {
        lua_pop(L, 1); // Limpiar la metatabla dada que quedo.
        return NULL;
    }
    while(!lua_rawequal(L, -1, -2))
    {
        lua_getfield(L, -1, "__extends");
        if(!lua_isnil(L, -1)) // posee una "Clase" base?
            // Si la posee, sacamos la metatabla anterior y seguimos comparando.
            lua_remove(L, -2);
        else
        {
            // Si no posee, quiere decir que no hereda de ella.
            lua_pop(L, 3);
            instance = NULL;
            clean = false;
            break;
        }
    }
    if(clean)
        lua_pop(L, 2);
        
    return instance;
}

void* lua_checkinstance(lua_State *L, int idx, const char *tname)
{
    void *instance = lua_testinstance(L, idx, tname);
    luaL_argexpected(L, instance != NULL, idx, tname);
    return instance;
}

void lua_getDinamicField(lua_State *L, const int ud, const char *key)
{
    // Buscamos en la tabla auxiliar
    lua_getuservalue(L, ud);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1); // quitar nil u otro valor incorrecto
        lua_pushnil(L);
        return;
    }
    lua_getfield(L, -1, key); // pila: ..., tabla, valor?
    lua_remove(L, -2); // quitar tabla

    if (!lua_isnil(L, -1))
        return; // lo encontró en la tabla auxiliar

    // Sino esta, buscamos en la metatabla y sus "clases" base
    lua_pop(L, 1); // quitar el nil
    if (!lua_getmetatable(L, ud))
    {
        lua_pushnil(L);
        return;
    }
    for(;;)
    {
        lua_getfield(L, -1, key);
        if (!lua_isnil(L, -1))
        {
            lua_remove(L, -2); // quitar metatable
            return;
        }
        lua_pop(L, 1); // quitar nil
        lua_getfield(L, -1, "__extends");
        if(!lua_istable(L, -1))
        {
            lua_pop(L, 2); // quitar metatable y "clase" base (si era nil u otra cosa)
            lua_pushnil(L);
            return;
        }
        lua_remove(L, -2); // quitar metatable, quedando la nueva "clase" base
    }
}

void lua_setDinamicField(lua_State* L, const int ud, const char* key)
{
    // Almacenamos la nueva clave en su tabla auxiliar.
    lua_getuservalue(L, ud);
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, key);
    lua_pop(L, 2); // Sacamos la tabla auxiliar de la pila y el valor.
}

void lua_pushVector2(lua_State* L, Vector2 vec)
{
    Vector2* vecLua = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
    *vecLua = vec;
    luaL_getmetatable(L, "Vector2");
    lua_setmetatable(L, -2);
}

void lua_pushColor(lua_State* L, Color color)
{
    Color* colorLua = (Color*)lua_newuserdata(L, sizeof(Color));
    *colorLua = color;
    luaL_getmetatable(L, "Color");
    lua_setmetatable(L, -2);
}
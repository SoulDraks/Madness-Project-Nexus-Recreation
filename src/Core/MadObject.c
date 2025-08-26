#include "Core/MadObject.h"
#include "Math/SoulMath.h"
#include "Engine/Engine.h"
#include "Lua/Bindings.h"

// Actualiza la posicion, escala y rotacion global del objeto en cuestion.
static inline void updateTransform(MadObject* self)
{
    if(self->parent != NULL)
    {
        // Escala local
        Vector2 scaled = {
            self->pos.x * self->parent->globalScale.x,
            self->pos.y * self->parent->globalScale.y
        };

        // Rotar la posición escalada con rotación global del padre
        float angle = DEG2RAD(self->parent->globalRotation); // en radianes
        Vector2 rotated = {
            cosf(angle) * scaled.x - sinf(angle) * scaled.y,
            sinf(angle) * scaled.x + cosf(angle) * scaled.y
        };

        // Finalmente, Sumamos la posición del padre
        self->globalPos = Vector2_add(self->parent->globalPos, rotated);

        // Escala global
        self->globalScale.x = self->scale.x * self->parent->globalScale.x;
        self->globalScale.y = self->scale.y * self->parent->globalScale.y;
        // Rotacion global
        self->globalRotation = self->parent->globalRotation + self->rotation;
    }
    else
    {
        // Si no tiene padre, su posicion, escala y rotacion global son iguales a la relativa.
        self->globalPos = self->pos;
        self->globalScale = self->scale;
        self->globalRotation = self->rotation;
    }
    // Llamar recursivamente en los hijos
    if(!isNullMadObjectRef(self->first_child))
        updateTransform(self->first_child.obj);
    // Llamar en los hermanos.
    if(!isNullMadObjectRef(self->next_sibling))
        updateTransform(self->next_sibling.obj);
}

static inline void updateGlobalColor(MadObject* self)
{
    // Si tiene padre, calculamos su color global modulado por su padre
    if(self->parent != NULL)
    {
        self->globalColor.r = (self->color.r * self->parent->globalColor.r) / 255;
        self->globalColor.g = (self->color.g * self->parent->globalColor.g) / 255;
        self->globalColor.b = (self->color.b * self->parent->globalColor.b) / 255;
        self->globalColor.a = (self->color.a * self->parent->globalColor.a) / 255;
    }
    else
        // Si no tiene padre, el tiene su propio color.
        self->globalColor = self->color;
    
    if(!isNullMadObjectRef(self->first_child))
        updateGlobalColor(self->first_child.obj);
    if(!isNullMadObjectRef(self->next_sibling))
        updateGlobalColor(self->next_sibling.obj);
}

static inline void detachFromParent(MadObject* self)
{
    MadObject* parent = self->parent;
    if(parent->first_child.obj == self) // ¿El nodo actual es su primer hijo? 
    {
        // Si lo es, pues darle que su primer hijo sea ahora su proximo hermano.
        parent->first_child = self->next_sibling;
        parent->last_child = self->first_child.obj;
    }
    else
    {
        // Si no, Buscar al hermano anterior para desconectar al hijo actual
        MadObjectRef sibling = parent->first_child;
        while(!isNullMadObjectRef(sibling))
        {
            if (sibling.obj->next_sibling.obj == self)
            {
                sibling.obj->next_sibling = self->next_sibling;
                if(self == parent->last_child)
                    parent->last_child = sibling.obj;

                break;
            }
            sibling = sibling.obj->next_sibling;
        }
    }

    // Desconectar al hijo completamente
    self->parent = NULL;
    self->next_sibling = NULL_MADOBJECTREF;
}

static inline bool isAncestorOf(MadObject* parent, MadObject* child)
{
    while (child != NULL)
    {
        if (child == parent)
            return true; // ¡Se encontró el ancestro!
        child = child->parent;
    }
    return false; // No se encontró el ancestro
}

static inline void removeChild(MadObject* parent, MadObjectRef prev, MadObjectRef child)
{
    if(isNullMadObjectRef(prev))
    {
        parent->first_child = child.obj->next_sibling;
        parent->last_child = parent->first_child.obj;
    }
    else
    {
        prev.obj->next_sibling = child.obj->next_sibling;
        if(child.obj == parent->last_child)
            parent->last_child = prev.obj;
    }

    luaL_unref(parent->L, LUA_REGISTRYINDEX, child.ref);
    child.obj->parent = NULL;
    child.obj->next_sibling = NULL_MADOBJECTREF;
    parent->childCount--;
    return;
}

MadObject* MadObject_new()
{
    MadObject* instance = (MadObject*)malloc(sizeof(MadObject));
    MADOBJECT_NEW(instance);
    return instance;
}

void MadObject_free(MadObject* self)
{
    printf("Se esta liberando un MadObject con nombre: %s\n", self->name.data);
    if(self->L != NULL)
    {
        if(self->self_reference != LUA_NOREF)
        {
            luaL_getmetatable(self->L, "weakRefs");
            luaL_unref(self->L, -1, self->self_reference);
            lua_pop(self->L, 1);
        }
    }
    String_free(self->name);
    /* Si tiene un padre, lo desvinculamos de el. */
    if(self->parent != NULL)
        detachFromParent(self);
    
    /* Recorrer y desvincular todos los hijos */
    MadObjectRef child = self->first_child;
    while(!isNullMadObjectRef(child))
    {
        luaL_unref(self->L, LUA_REGISTRYINDEX, child.ref);
        child.obj->parent = NULL;
        child = child.obj->next_sibling;
    }
}

void MadObject_setPosition(MadObject* self, const Vector2 value)
{
    self->pos = value;
    updateTransform(self);
}

static Vector2 MadObject_getPosition(MadObject* self)
{
    return self->pos;
}

void MadObject_setGlobalPosition(MadObject* self, const Vector2 value)
{
    if(self->parent != NULL)
        // Calcular la nueva posicion relativa respecto a su nodo padre.
        self->pos = Vector2_sub(value, self->parent->globalPos);
    else
        // Si no tiene padre, su posicion global es la misma que la relativa.
        self->pos = value;
    updateTransform(self);
}

static Vector2 MadObject_getGlobalPosition(MadObject* self)
{
    return self->globalPos;
}

void MadObject_setScale(MadObject* self, const Vector2 value)
{
    self->scale = value;
    updateTransform(self);
}

static Vector2 MadObject_getScale(MadObject* self)
{
    return self->scale;
}

void MadObject_setGlobalScale(MadObject* self, const Vector2 value)
{
    if(self->parent != NULL)
    {
        // Si tiene padre, recalcular su escala relativa a partir de la de su padre.
        self->scale.x = value.x / self->parent->globalScale.x;
        self->scale.y = value.y / self->parent->globalScale.y;
    }
    else
        self->scale = value;
    updateTransform(self);
}

static Vector2 MadObject_getGlobalScale(MadObject* self)
{
    return self->globalScale;
}

void MadObject_setRotation(MadObject* self, const double value)
{
    self->rotation = value;
    updateTransform(self);
}

static double MadObject_getRotation(MadObject* self)
{
    return self->rotation;
}

void MadObject_setGlobalRotation(MadObject* self, const float value)
{
    if(self->parent != NULL)
        self->rotation = value - self->parent->globalRotation;
    else
        self->rotation = value;
    updateTransform(self);
}

static double MadObject_getGlobalRotation(MadObject* self)
{
    return self->globalRotation;
}

void MadObject_setColor(MadObject* self, const Color value)
{
    self->color = value;
    updateGlobalColor(self);
}

static Color MadObject_getColor(MadObject* self)
{
    return self->globalColor;
}

void MadObject_callTick(MadObject* self, const double deltaTime)
{
    /* Llamar a "tick" de C. */
    if(self->tick != NULL)
        self->tick(self, deltaTime);
    /* Llamar a "tick" de Lua. */
    callLuaFunction3(self->L, self->self_reference, true, "tick", 1,
        LUA_TNUMBER, deltaTime
    );
}

void MadObject_addChild(MadObject* self, MadObjectRef newChild)
{
    if(isNullMadObjectRef(self->first_child))
    {
        // Añadir como primer hijo si no tiene hijos.
        self->first_child = newChild;
        self->last_child = newChild.obj;
    }
    else
    {
        // Añadirlo como ultimo hijo si hay mas
        self->last_child->next_sibling = newChild;
        self->last_child = newChild.obj;
    }
    newChild.obj->parent = self;
    self->childCount++;
    updateTransform(newChild.obj);
    updateGlobalColor(newChild.obj);
}

MadObjectRef MadObject_getChildByName(MadObject* self, String name)
{
    if(isNullMadObjectRef(self->first_child))
        return NULL_MADOBJECTREF;
    // Recorrer toda la lista de hijos y comparar sus nombres.
    MadObjectRef child = self->first_child;
    while(!isNullMadObjectRef(child))
    {
        if(String_cmp(child.obj->name, name))
            return child;
        child = child.obj->next_sibling;
    }
    return NULL_MADOBJECTREF;
}

MadObjectRef MadObject_getChildByIndex(MadObject* self, const int index)
{
    if(index < 0 || isNullMadObjectRef(self->first_child) || index > (self->childCount - 1))
        return NULL_MADOBJECTREF;
    MadObjectRef child = self->first_child;
    for(int i = 0; i < index; i++)
    {
        if(isNullMadObjectRef(child.obj->next_sibling))
            return NULL_MADOBJECTREF;
        child = child.obj->next_sibling;
    }
    return child;
}

void MadObject_removeChildByName(MadObject* self, String name)
{
    if(isNullMadObjectRef(self->first_child))
        return;
    MadObjectRef prev = NULL_MADOBJECTREF;
    MadObjectRef current = self->first_child;
    while(!isNullMadObjectRef(current))
    {
        if(String_cmp(current.obj->name, name))
        {
            removeChild(self, prev, current);
            return;
        }
        prev = current;
        current = current.obj->next_sibling;
    }
}

void MadObject_removeChildByIndex(MadObject* self, const int index)
{
    if(index + 1 > self->childCount)
        return;
    MadObjectRef prev = NULL_MADOBJECTREF;
    MadObjectRef current = self->first_child;
    for(int i = 0; i < index; i++)
    {
        prev = current;
        current = current.obj->next_sibling;
    }
    removeChild(self, prev, current);
}

void MadObject_removeChildByPtr(MadObject* self, MadObject* child)
{
    if(isNullMadObjectRef(self->first_child))
        return;
    MadObjectRef prev = NULL_MADOBJECTREF;
    MadObjectRef current = self->first_child;
    while(!isNullMadObjectRef(current))
    {
        if(current.obj == child) // Comparamos por direccion de memoria.
        {
            removeChild(self, prev, current);
            return;
        }
        prev = current;
        current = current.obj->next_sibling;
    }
}

bool MadObject_isInsideTree(MadObject* self)
{
    MadObject* _root = Engine_getInstance()->root; // Obtenemos el nodo raiz.
    if(self->parent == NULL)
        return false;
    // Viajamos hasta el fondo de la jerarquia para ver si su nodo final es el nodo raiz
    MadObject* parent = self->parent;
    while(parent->parent != NULL)
        parent = parent->parent;
    if(parent == _root)
        // Si lo es, significa que esta dentro del arbol.
        return true;
    return false;
}

void lua_pushMadObject(lua_State* L, MadObject* obj)
{
    if(obj->L == NULL || obj->self_reference == LUA_NOREF)
    {
        // Si no fue creado desde lua o no tiene referencia a ella, le creamos una.
        MadObject** instanceLua = (MadObject**)lua_newuserdata(L, sizeof(MadObject*));
        *instanceLua = obj;
        MADOBJECT_NEWLUA(obj);

        luaL_getmetatable(L, "MadObject");
        lua_setmetatable(L, -2);
    }
    else
    {
        // Si tiene, recuperamos su referencia desde la tabla de referencias debiles.
        getWeakRefsTable(L);
        lua_rawgeti(L, -1, obj->self_reference);
        lua_remove(L, -2);
    }
}

/* <----- BINDINGS -----> */

static int MadObject_getParentLua(lua_State* L)
{
    MadObject** self = (MadObject**)lua_checkinstance(L, 1, "MadObject");
    if((*self)->parent != NULL)
        lua_pushMadObject(L, (*self)->parent);
    else
        lua_pushnil(L);
    
    return 1;
}

static int MadObject_addChildLua(lua_State* L)
{
    MadObject** self = (MadObject**)lua_checkinstance(L, 1, "MadObject");
    MadObject** child = (MadObject**)lua_checkinstance(L, 2, "MadObject");
    if(*self == *child)
        return luaL_error(L, "Error: A MadObject cannot add itself as a child.");
    if(isAncestorOf(*self, *child))
        return luaL_error(L, "Error: the node '%s' is ancestor of: '%s'.", (*self)->name.data, (*child)->name.data);

    lua_pushvalue(L, 2); /* Poner la instancia que añadir como hijo en la cima para referenciarlo */
    MadObjectRef newChild = lua_refMadObject(L, LUA_REGISTRYINDEX);
    MadObject_addChild(*self, newChild);
    return 0;
}

static int MadObject_getChildLua(lua_State* L)
{
    MadObject** self = (MadObject**)lua_checkinstance(L, 1, "MadObject");
    MadObjectRef child;
    if(lua_type(L, 2) == LUA_TSTRING) // ¿es un string?
    {
        String name = String_newFromLua(L, 2);
        child = MadObject_getChildByName(*self, name);
        String_free(name);
    }
    else if(lua_type(L, 2) == LUA_TNUMBER) // Es un numero?
    {
        int index = lua_tointeger(L, 2);
        if(index <= 0)
            return luaL_error(L, "expected index greater than zero for argument #2 (got %i)", index);
        child = MadObject_getChildByIndex(*self, --index);
    }
    else
        return luaL_error(L, "bad argument #2: expected string or number, got %s", luaL_typename(L, 2));
    
    if(!isNullMadObjectRef(child))
        lua_rawgeti(L, LUA_REGISTRYINDEX, child.ref);
    else
        lua_pushnil(L);

    return 1;
}

static int MadObject_getChildrenLua(lua_State* L)
{
    MadObject** self = (MadObject**)lua_checkinstance(L, 1, "MadObject");
    if((*self)->childCount == 0)
        lua_pushnil(L);
    else
    {
        lua_createtable(L, (int)(*self)->childCount, 0);
        MadObjectRef child = (*self)->first_child;
        int index = 1;
        while(!isNullMadObjectRef(child))
        {
            lua_rawgeti(L, LUA_REGISTRYINDEX, child.ref);
            lua_rawseti(L, -2, index);
            index++;
            child = child.obj->next_sibling;
        }
    }
    return 1;
}

static int MadObject_removeChildLua(lua_State* L)
{
    MadObject** self = (MadObject**)lua_checkinstance(L, 1, "MadObject");
    if(lua_isstring(L, 2))
    {
        String name = String_newFromLua(L, 2);
        MadObject_removeChildByName(*self, name);
        String_free(name);
    }
    else if(lua_isnumber(L, 2))
    {
        int index = lua_tointeger(L, 2);
        if(index <= 0)
            return luaL_error(L, "Error: expected index greater than zero to argument #2. index: %i", index);
        MadObject_removeChildByIndex(*self, index);
    }
    else if(lua_testinstance(L, 2, "MadObject") != NULL)
    {
        MadObject** child = (MadObject**)lua_touserdata(L, 2);
        MadObject_removeChildByPtr(*self, *child);
    }
    else
        return luaL_error(L, "Error: expected string or number to #2 argument but was received %s.", luaL_typename(L, 2));
        
    return 0;
}

static int MadObject_isInsideTreeLua(lua_State* L)
{
    MadObject** self = (MadObject**)lua_checkinstance(L, 1, "MadObject");
    bool isInsideTree = MadObject_isInsideTree(*self);
    lua_pushboolean(L, isInsideTree);
    return 1;
}

static int MadObject_queueFreeLua(lua_State* L)
{
    MadObject** self = (MadObject**)lua_checkinstance(L, 1, "MadObject");
    if((*self)->queued)
        return 0;
    bool freeAllChilds = false;
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    freeAllChilds = (bool)lua_toboolean(L, 2);
        
    Engine_addObjectQueue(*self, freeAllChilds);
    (*self)->queued = true;
    return 0;
}

void MadObject_register(lua_State* L)
{
    Lua_registerclass("MadObject",
        CONSTRUCTOR, MadObject_new,
        DESTRUCTOR, MadObject_free,
        FIELD, "name", TSTRING, offsetof(MadObject, name),
        FIELDGETTERSETTER, "pos", TVECTOR2, MadObject_getPosition, MadObject_setPosition,
        FIELDGETTERSETTER, "globalPos", TVECTOR2, MadObject_getGlobalPosition, MadObject_setGlobalPosition,
        FIELD, "size", TVECTOR2, offsetof(MadObject, size),
        FIELDGETTERSETTER, "scale", TVECTOR2, MadObject_getScale, MadObject_setScale,
        FIELDGETTERSETTER, "globalScale", TVECTOR2, MadObject_getGlobalScale, MadObject_setGlobalScale,
        FIELDGETTERSETTER, "rotation", TDOUBLE, MadObject_getRotation, MadObject_setRotation,
        FIELDGETTERSETTER, "globalRotation", TDOUBLE, MadObject_getGlobalRotation, MadObject_setGlobalRotation,
        FIELDGETTERSETTER, "color", TCOLOR, MadObject_getColor, MadObject_setColor,
        FIELD, "visible", TBOOL, offsetof(MadObject, visible),
        FIELD, "flip", TBOOL, offsetof(MadObject, flip),
        FIELDREADONLY, "childCount", TINT, offsetof(MadObject, childCount),
        FUNC, "getParent", MadObject_getParentLua,
        FUNC, "addChild", MadObject_addChildLua,
        FUNC, "getChild", MadObject_getChildLua,
        FUNC, "getChildren", MadObject_getChildrenLua,
        FUNC, "removeChild", MadObject_removeChildLua,
        FUNC, "isInsideTree", MadObject_isInsideTreeLua,
        FUNC, "queueFree", MadObject_queueFreeLua,
        END
    );
}
#include "Button.h"
#include "Input/Mouse.h"
#include "Math/SoulMath.h"
#include "Lua/LuaManager.h"

// Funcion auxiliar que revisa si alguno de sus ancestros no es visible.
static bool amVisible(Button* self)
{
    if(!self->base.visible)
        return false;
    MadObject* parent = self->base.parent;
    while(parent != NULL)
    {
        if(!parent->visible)
            return false;
        parent = parent->parent;
    }
    return true;
}

Button* Button_new()
{
    Button* instance = (Button*)malloc(sizeof(Button));
    MADOBJECT_NEW((MadObject*)instance);
    instance->base.tick = Button_tick;
    instance->base.draw = Button_draw;
    instance->surface = NULL_MADOBJECTREF;
    instance->onPressed = NULL;
    instance->onReleased = NULL;
    instance->onHover = NULL;
    instance->onLeave = NULL;
    instance->hasBeen_onPressed_Called = false;
    instance->hasBeen_onReleased_Called = false;
    instance->hasBeen_onHover_Called = false;
    instance->disabled = false;
    return instance;
}

void Button_free(Button* self)
{
    MadObject_free((MadObject*)self);
    if(!isNullMadObjectRef(self->surface))
        luaL_unref(self->base.L, LUA_REGISTRYINDEX, self->surface.ref);
    self->surface = NULL_MADOBJECTREF;
}

void Button_tick(void* self, double delta)
{
    Button* button = (Button*)self;
    MouseState* Mouse = getMouseState();
    Vector2 final_size = {button->base.size.x * button->base.globalScale.x, button->base.size.y * button->base.globalScale.y};
    if(!button->disabled && amVisible(button) && SoulMath_isCollindingMouse(Mouse->x, Mouse->y, button->base.globalPos, final_size))
    {
        if(!button->hasBeen_onHover_Called)
        {
            button->hasBeen_onHover_Called = true;
            if(button->onHover != NULL)
                button->onHover(button);

            callLuaFunction3(button->base.L, button->base.self_reference, true, "onHover", 0);
        }
        if(Mouse->isLeftClickPressed)
        {
            if(!button->hasBeen_onPressed_Called)
            {
                button->hasBeen_onPressed_Called = true;
                button->hasBeen_onReleased_Called = false;
                if(button->onPressed != NULL)
                    button->onPressed(button);
                callLuaFunction3(button->base.L, button->base.self_reference, true, "onPressed", 0);
            }
        }
        else
        {
            if(button->hasBeen_onPressed_Called && !button->hasBeen_onReleased_Called)
            {
                button->hasBeen_onPressed_Called = false;
                button->hasBeen_onReleased_Called = true;
                if(button->onReleased != NULL)
                    button->onReleased(button);

                callLuaFunction3(button->base.L, button->base.self_reference, true, "onReleased", 0);
            }
        }
    }
    else
    {
        if(button->hasBeen_onHover_Called)
        {
            if(button->onLeave != NULL)
                button->onLeave(button);
                
            callLuaFunction3(button->base.L, button->base.self_reference, true, "onLeave", 0);
        }
        button->hasBeen_onHover_Called = false;
        button->hasBeen_onPressed_Called = false;
    }
}

void Button_draw(void* self, SDL_Renderer* renderer)
{
    Button* button = (Button*)self;
    if(!isNullMadObjectRef(button->surface) && button->surface.obj->draw != NULL)
    {
        MadObject* surface = button->surface.obj;
        surface->globalPos = button->base.globalPos;
        surface->globalScale = button->base.globalScale;
        surface->globalRotation = button->base.globalRotation;
        surface->globalColor = button->base.globalColor;
        surface->size = button->base.size;
        surface->rotation = button->base.rotation;
        surface->draw(surface, renderer);
    }
}

int Button_newLua(lua_State* L)
{
    Button* instance = Button_new();
    Button** instanceLua = (Button**)lua_newuserdata(L, sizeof(Button*));
    *instanceLua = instance;
    MADOBJECT_NEWLUA((MadObject*)instance);

    luaL_getmetatable(L, "Button");
    lua_setmetatable(L, -2);

    return 1;
}

int Button_indexLua(lua_State* L)
{
    Button** self = (Button**)lua_checkinstance(L, 1, "Button");
    const char* key = luaL_checkstring(L, 2);
    MADOBJECT_INDEXLUA((MadObject**)self)
    else if(strcmp(key, "surface") == 0)
    {
        if(!isNullMadObjectRef((*self)->surface))
            lua_rawgeti(L, LUA_REGISTRYINDEX, (*self)->surface.ref);
        else
            lua_pushnil(L);
    }
    else if(strcmp(key, "disabled") == 0)
        lua_pushboolean(L, (int)(*self)->disabled);
    else
        lua_getDinamicField(L, 1, key);
        
    return 1;
}

int Button_newindexLua(lua_State* L)
{
    Button** self = (Button**)lua_checkinstance(L, 1, "Button");
    const char* key = luaL_checkstring(L, 2);
    MADOBJECT_NEWINDEXLUA((MadObject**)self)
    else if(strcmp(key, "surface") == 0)
    {
        if(lua_testinstance(L, 3, "MadObject") != NULL) // Es un MadObject o derivado?
        {
            if(lua_testinstance(L, 3, "Button") != NULL) // es un boton?
                return luaL_error(L, "Error: Cannot assign a button as a surface to a button.");

            if(!isNullMadObjectRef((*self)->surface))
                luaL_unref(L, LUA_REGISTRYINDEX, (*self)->surface.ref);
            MadObject** object = (MadObject**)lua_touserdata(L, 3);
            (*self)->surface.obj = *object;
            lua_pushvalue(L, 3);
            (*self)->surface.ref = luaL_ref(L, LUA_REGISTRYINDEX);
        }
    }
    else if(strcmp(key, "disabled") == 0)
        (*self)->disabled = (bool)lua_toboolean(L, 3);
    else
        lua_setDinamicField(L, 1, key);

    return 0;
}

int Button_gc(lua_State* L)
{
    Button** self = (Button**)lua_checkinstance(L, 1, "Button");
    if(*self != NULL)
    {
        Button_free(*self);
        free(*self);
        *self = NULL;
    }
    return 0;
}

void Button_register(lua_State* L)
{
    luaL_newmetatable(L, "Button");

    lua_pushcfunction(L, Button_indexLua);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, Button_newindexLua);
    lua_setfield(L, -2, "__newindex");

    lua_pushcfunction(L, Button_gc);
    lua_setfield(L, -2, "__gc");

    lua_pushcfunction(L, Button_newLua);
    lua_setfield(L, -2, "new");

    luaL_getmetatable(L, "MadObject");
    lua_setfield(L, -2, "__extends");

    lua_setglobal(L, "Button");
}
#include "Panel.h"

Panel* Panel_new()
{
    Panel* instance = (Panel*)malloc(sizeof(Panel));
    MADOBJECT_NEW((MadObject*)instance);
    instance->base.draw = Panel_draw;
    instance->borderColor = (Color){0xFF, 0xFF, 0xFF, 0xFF};
    instance->borderSize = 0;
    return instance;
}

void Panel_draw(void* self, SDL_Renderer* renderer)
{
    Panel* panel = (Panel*)self;
    // Dibujamos el rectangulo
    SDL_Rect rect;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, panel->base.globalColor.r, panel->base.globalColor.g, panel->base.globalColor.b, panel->base.globalColor.a);
    rect.x = (int)(panel->base.globalPos.x);
    rect.y = (int)(panel->base.globalPos.y);
    rect.w = (int)(panel->base.size.x * panel->base.globalScale.x);
    rect.h = (int)(panel->base.size.y * panel->base.globalScale.y);
    SDL_RenderFillRect(renderer, &rect);
    // Dibujamos su bordeado si tiene.
    if(panel->borderSize > 0)
    {
        SDL_SetRenderDrawColor(renderer, panel->borderColor.r, panel->borderColor.g, panel->borderColor.b, panel->borderColor.a);
        for(int i = 0; i < panel->borderSize; ++i)
        {
            SDL_Rect border = {
                rect.x + i,
                rect.y + i,
                rect.w - 2 * i,
                rect.h - 2 * i
            };
            SDL_RenderDrawRect(renderer, &border);
        }
    }
}

/* <------- BINDINGS ------->*/

static int Panel_newLua(lua_State* L)
{
    Panel* instance = Panel_new(); 
    Panel** instance_lua = (Panel**)lua_newuserdata(L, sizeof(Panel*));
    *instance_lua = instance;
    MADOBJECT_NEWLUA((MadObject*)instance);

    luaL_getmetatable(L, "Panel");
    lua_setmetatable(L, -2);
    return 1;
}

static int Panel_indexLua(lua_State* L)
{
    Panel** self = (Panel**)lua_checkinstance(L, 1, "Panel");
    const char* key = luaL_checkstring(L, 2);
    MADOBJECT_INDEXLUA((MadObject**)self)
    else if(strcmp(key, "borderColor") == 0)
        lua_pushColor(L, (*self)->borderColor);
    else if(strcmp(key, "borderSize") == 0)
        lua_pushinteger(L, (*self)->borderSize);
    else
        lua_getDinamicField(L, 1, key);
    
    return 1;
}

static int Panel_newindexLua(lua_State* L)
{
    Panel** self = (Panel**)lua_checkinstance(L, 1, "Panel");
    const char* key = luaL_checkstring(L, 2);
    MADOBJECT_NEWINDEXLUA((MadObject**)self)
    else if(strcmp(key, "borderColor_r") == 0)
        (*self)->borderColor.r = lua_checkColor(L, 3).r;
    else if(strcmp(key, "borderColor_g") == 0)
        (*self)->borderColor.g = lua_checkColor(L, 3).g;
    else if(strcmp(key, "borderColor_b") == 0)
        (*self)->borderColor.b = lua_checkColor(L, 3).b;
    else if(strcmp(key, "borderColor_a") == 0)
        (*self)->borderColor.r = lua_checkColor(L, 3).a;
    else if(strcmp(key, "borderColor") == 0)
        (*self)->borderColor = lua_checkColor(L, 3);
    else if(strcmp(key, "borderSize") == 0)
        (*self)->borderSize = luaL_checkinteger(L, 3);
    else
        lua_setDinamicField(L, 1, key);
    
    return 0;
}

void Panel_register(lua_State* L)
{
    luaL_newmetatable(L, "Panel");

    lua_pushcfunction(L, Panel_newLua);
    lua_setfield(L, -2, "new");

    lua_pushcfunction(L, Panel_indexLua);
    lua_setfield(L, -2, "__index"); // Panel.__index = MadObject.__index

    lua_pushcfunction(L, Panel_newindexLua);
    lua_setfield(L, -2, "__newindex"); // Panel.__newindex = MadObject.__newindex

    luaL_getmetatable(L, "MadObject");
    lua_getfield(L, -1, "__gc");
    lua_setfield(L, -3, "__gc"); // Panel.__gc = MadObject.__gc

    lua_setfield(L, -2, "__extends"); // Indicamos que la "Clase" Panel "hereda" de "MadObject".

    lua_setglobal(L, "Panel");
}
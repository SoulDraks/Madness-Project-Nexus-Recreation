#include "Panel.h"
#include "Lua/Bindings.h"

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

void Panel_register(lua_State* L)
{
    /*
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
    */
   Lua_registerclass("Panel", 
        CONSTRUCTOR, Panel_new,
        DESTRUCTOR, MadObject_free,
        EXTENDS, "MadObject",
        FIELD, "borderColor", TCOLOR, offsetof(Panel, borderColor),
        FIELD, "borderSize", TINT, offsetof(Panel, borderSize),
        END
    );
}
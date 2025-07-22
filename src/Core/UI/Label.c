#include "Label.h"
#include "Render/Renderer.h"
#include "Core/Graphics/SurfaceEffects.h"

#define INIT_TEXTLENGTH 128 // Tamaño inicial del buffer de texto del Label.

#define renderLabel(self) TTF_RenderText_Blended_Wrapped((self)->fontBuffer, (self)->text.data, (SDL_Color){255, 255, 255, 255}, 0)

// Rerenderiza todo el label, actualizando su informacion.
static inline void rerenderLabel(Label* self)
{
    // Renderizamos el texto y los filtros de brillo y sombra si lo necesita.
    SDL_Renderer* renderer = GetRenderer();
    SDL_Surface* renderedTextSurface = renderLabel(self);
    if(self->renderedText != NULL)
        SDL_DestroyTexture(self->renderedText);
    self->renderedText = SDL_CreateTextureFromSurface(renderer, renderedTextSurface);
    self->base.size = (Vector2){(double)(renderedTextSurface->w), (double)(renderedTextSurface->h)};
    if(self->hasGlowFilter)
    {
        SDL_Surface* glowFilterSurface = SurfaceEffects.makeGlowBorder(renderedTextSurface, (Color){255, 255, 255, 255}, self->glowIntensity, self->glowSize);
        if(self->glowText != NULL)
            SDL_DestroyTexture(self->glowText);
        self->glowText = SDL_CreateTextureFromSurface(renderer, glowFilterSurface);
        self->glowTextSize = (Vector2){(double)glowFilterSurface->w, (double)glowFilterSurface->h};
        SDL_FreeSurface(glowFilterSurface);
    }
    if(self->hasShadowFilter)
    {
        SDL_Surface* shadowFilterSurface = renderLabel(self);
        SurfaceEffects.applyBlur(shadowFilterSurface, self->blurRadius, self->sigmaEffect);
        if(self->shadowText != NULL)
            SDL_DestroyTexture(self->shadowText);
        self->shadowText = SDL_CreateTextureFromSurface(renderer, shadowFilterSurface);
        SDL_FreeSurface(shadowFilterSurface);
    }
    if(self->hasOutline && self->outlineSize > 0)
    {
        TTF_SetFontOutline(self->fontBuffer, self->outlineSize);
        SDL_Surface* outlineSurface = renderLabel(self);
        if(self->outlineText != NULL)
            SDL_DestroyTexture(self->outlineText);
        self->outlineText = SDL_CreateTextureFromSurface(renderer, outlineSurface);
        self->outlineTextSize = (Vector2){(double)outlineSurface->w, (double)outlineSurface->h};
        SDL_FreeSurface(outlineSurface);
        TTF_SetFontOutline(self->fontBuffer, 0);
    }
    self->mustBeRerendered = false;
    SDL_FreeSurface(renderedTextSurface);
}

Label* Label_new()
{
    Label* instance = (Label*)malloc(sizeof(Label));
    MADOBJECT_NEW((MadObject*)instance);
    instance->base.draw = Label_draw;
    instance->text = String_new("Label");
    instance->fontPath = NULLSTRING;
    instance->fontSize = 0;
    instance->mustBeRerendered = false;
    instance->renderedText = NULL;
    instance->hasShadowFilter = false;
    instance->shadowText = NULL;
    instance->shadowTextColor = (Color){0, 0, 0, 0};
    instance->blurRadius = 0;
    instance->shadowTextDistance = 0.0;
    instance->sigmaEffect = 0.0;
    instance->hasGlowFilter = false;
    instance->glowText = NULL;
    instance->glowTextColor = (Color){0, 0, 0, 0};
    instance->glowIntensity = 0;
    instance->glowSize = 0;
    instance->glowTextSize = (Vector2){0.0, 0.0};
    instance->hasOutline = false;
    instance->outlineText = NULL;
    instance->outlineColor = (Color){0, 0, 0, 0};
    instance->outlineSize = 0;
    instance->outlineTextSize = (Vector2){0.0, 0.0};
    instance->fontBuffer = NULL;
    return instance;
}

void Label_free(Label* self)
{
    MadObject_free((MadObject*)self);
    String_free(self->text);
    if(self->fontPath.data != NULL)
        String_free(self->fontPath);
    if(self->renderedText != NULL)
        SDL_DestroyTexture(self->renderedText);
    if(self->shadowText != NULL)
        SDL_DestroyTexture(self->shadowText);
    if(self->glowText != NULL)
        SDL_DestroyTexture(self->glowText);
    if(self->outlineText != NULL)
        SDL_DestroyTexture(self->outlineText);
    if(self->fontBuffer != NULL)
        TTF_CloseFont(self->fontBuffer);
}

void Label_draw(void* self, SDL_Renderer* renderer)
{
    Label* label = (Label*)self;
    if(label->fontBuffer == NULL)
        return;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if(label->base.flip)
        flip = SDL_FLIP_HORIZONTAL;
    if(label->mustBeRerendered)
        rerenderLabel(label);
    // Dibujamos el filtro de sombra.
    if(label->hasShadowFilter)
    {
        SDL_SetTextureColorMod(label->shadowText, label->shadowTextColor.r, label->shadowTextColor.g, label->shadowTextColor.b);
        SDL_SetTextureAlphaMod(label->shadowText, label->shadowTextColor.a);
        // Calculamos la posicion de la sombra a partir de la distancia asignada.
        Vector2 shadowDistance = {label->shadowTextDistance, label->shadowTextDistance};
        if(label->base.flip)
            shadowDistance.x *= -1;
        shadowDistance.x = shadowDistance.x * label->base.globalScale.x;
        shadowDistance.y = shadowDistance.y * label->base.globalScale.y;
        Vector2 shadowPos = Vector2_add(label->base.globalPos, shadowDistance);
        // Lo asignamos a su rectangulo de destino
        SDL_Rect dstRectShadow;
        dstRectShadow.x = (int)shadowPos.x;
        dstRectShadow.y = (int)shadowPos.y;
        dstRectShadow.w = (int)(label->base.size.x * label->base.globalScale.x);
        dstRectShadow.h = (int)(label->base.size.y * label->base.globalScale.y);
        SDL_Point centerShadow = {(int)(dstRectShadow.w / 2), (int)(dstRectShadow.h / 2)};
        SDL_RenderCopyEx(renderer, label->shadowText, NULL, &dstRectShadow, label->base.globalRotation, &centerShadow, flip);
    }
    // Dibujamos el filtro de brillo.
    if(label->hasGlowFilter)
    {
        SDL_Rect dstRectGlow;
        SDL_SetTextureColorMod(label->glowText, label->glowTextColor.r, label->glowTextColor.g, label->glowTextColor.b);
        SDL_SetTextureAlphaMod(label->glowText, label->glowTextColor.a);
        // Calculamos la posicion del brillo para que este en el centro del texto.
        // glowPos = label->base.globalPos + (label->base.size - label->glowTextSize) / 2
        Vector2 glowPos = Vector2_add(label->base.globalPos, Vector2_div(Vector2_sub(label->base.size, label->glowTextSize), 2));
        dstRectGlow.x = (int)glowPos.x;
        dstRectGlow.y = (int)glowPos.y;
        dstRectGlow.w = (int)(label->glowTextSize.x * label->base.globalScale.x);
        dstRectGlow.h = (int)(label->glowTextSize.y * label->base.globalScale.y);
        SDL_Point centerGlow = {(int)(dstRectGlow.w / 2), (int)(dstRectGlow.h / 2)};
        SDL_RenderCopyEx(renderer, label->glowText, NULL, &dstRectGlow, label->base.globalRotation, &centerGlow, flip);
    }
    // Dibujamos el bordeado del texto o 'outline'.
    if(label->hasOutline)
    {
        SDL_SetTextureColorMod(label->outlineText, label->outlineColor.r, label->outlineColor.g, label->outlineColor.b);
        SDL_SetTextureAlphaMod(label->outlineText, label->outlineColor.a);
        // Calculamos la posicion del outline o bordeado para que este en el centro del texto.
        Vector2 outlinePos = Vector2_add(label->base.globalPos, Vector2_div(Vector2_sub(label->base.size, label->outlineTextSize), 2));
        SDL_Rect dstRectOutline;
        dstRectOutline.x = (int)outlinePos.x;
        dstRectOutline.y = (int)outlinePos.y;
        dstRectOutline.w = (int)(label->outlineTextSize.x * label->base.globalScale.x);
        dstRectOutline.h = (int)(label->outlineTextSize.y * label->base.globalScale.y);
        SDL_Point centerOutline = {(int)(dstRectOutline.w / 2), (int)(dstRectOutline.h / 2)};
        SDL_RenderCopyEx(renderer, label->outlineText, NULL, &dstRectOutline, label->base.globalRotation, &centerOutline, flip);
    }
    // Dibujamos el texto renderizado.
    SDL_Rect dstRect;
    SDL_SetTextureColorMod(label->renderedText, label->base.globalColor.r, label->base.globalColor.g, label->base.globalColor.b);
    SDL_SetTextureAlphaMod(label->renderedText, label->base.globalColor.a);
    dstRect.x = (int)label->base.globalPos.x;
    dstRect.y = (int)label->base.globalPos.y;
    dstRect.w = (int)(label->base.size.x * label->base.globalScale.x);
    dstRect.h = (int)(label->base.size.y * label->base.globalScale.y);
    SDL_Point center = {(int)(dstRect.w / 2), (int)(dstRect.h / 2)};
    SDL_RenderCopyEx(renderer, label->renderedText, NULL, &dstRect, label->base.globalRotation, &center, flip);
}

Vector2 Label_getSize(Label* self)
{
    if(self->mustBeRerendered)
        rerenderLabel(self);
    return self->base.size;
}

void Label_setFontSize(Label* self, const int size)
{
    if(self->fontBuffer == NULL)
    {
        SDL_Log("Cannot change font size because no font has not been loaded.");
        return;
    }
    TTF_CloseFont(self->fontBuffer);
    self->fontBuffer = TTF_OpenFont(self->fontPath.data, size);
    self->fontSize = size;
    self->mustBeRerendered = true;
}

void Label_loadFont(Label* self, String fontPath, const int fontSize)
{
    SDL_Renderer* renderer = GetRenderer();
    if(self->fontBuffer != NULL) // ya tiene una fuente?
        // Liberamos y destruir su fuente
        TTF_CloseFont(self->fontBuffer);
    self->fontBuffer = TTF_OpenFont(fontPath.data, fontSize);
    if(self->fontBuffer == NULL) // fallo?
    {
        SDL_Log("Failed to load font '%s': %s", fontPath, SDL_GetError());
        return;
    }
    self->fontSize = fontSize;
    if(self->fontPath.data != NULL)
        String_free(self->fontPath);
    self->fontPath = String_dup(fontPath);
    self->mustBeRerendered = true;
}

void Label_applyGlowFilter(Label* self, Color glowColor, int intensity, int glowSize)
{
    self->mustBeRerendered = true;
    self->hasGlowFilter = true;
    self->glowTextColor = glowColor;
    self->glowIntensity = intensity;
    self->glowSize = glowSize;
}

void Label_applyShadowFilter(Label* self, Color shadowColor, int blurRadius, double shadowDistance, double sigma)
{
    self->mustBeRerendered = true; // Marcamos que debe ser renderizado o rerenderizado.
    self->hasShadowFilter = true;
    self->shadowTextColor = shadowColor;
    self->blurRadius = blurRadius;
    self->shadowTextDistance = shadowDistance;
    self->sigmaEffect = sigma;
    self->shadowTextDistance = shadowDistance;
}

void Label_applyOutline(Label* self, Color outlineColor, int outlineSize)
{
    self->mustBeRerendered = true;
    self->hasOutline = true;
    self->outlineColor = outlineColor;
    self->outlineSize = outlineSize;
}

/* <------- BINDINGS ------->*/

static int Label_newLua(lua_State* L)
{
    Label* instance = Label_new();
    Label** instanceLua = (Label**)lua_newuserdata(L, sizeof(Label*));
    *instanceLua = instance;
    MADOBJECT_NEWLUA((MadObject*)instance);

    luaL_getmetatable(L, "Label");
    lua_setmetatable(L, -2);

    return 1;
}

static int Label_indexLua(lua_State* L)
{
    Label** self = (Label**)lua_checkinstance(L, 1, "Label");
    const char* key = luaL_checkstring(L, 2);
    if(strcmp(key, "pos") == 0)
    {
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *vec = (*self)->base.pos;
        luaL_getmetatable(L, "Vector2");
        lua_setmetatable(L, -2);
    }
    else if(strcmp(key, "globalPos") == 0)
    {
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *vec = (*self)->base.globalPos;
        luaL_getmetatable(L, "Vector2");
        lua_setmetatable(L, -2);
    }
    else if(strcmp(key, "size") == 0)
    {
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *vec = Label_getSize(*self);
        luaL_getmetatable(L, "Vector2");
        lua_setmetatable(L, -2);
    }
    else if(strcmp(key, "scale") == 0)
    {
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *vec = (*self)->base.scale;
        luaL_getmetatable(L, "Vector2");
        lua_setmetatable(L, -2);
    }
    else if(strcmp(key, "globalScale") == 0)
    {
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *vec = (*self)->base.globalScale;
        luaL_getmetatable(L, "Vector2");
        lua_setmetatable(L, -2);
    }
    else if(strcmp(key, "color") == 0)
    {
        Color* color = (Color*)lua_newuserdata(L, sizeof(Color));
        *color = (*self)->base.color;
        luaL_getmetatable(L, "Color");
        lua_setmetatable(L, -2);
    }
    else if(strcmp(key, "rotation") == 0)
        lua_pushnumber(L, (lua_Number)(*self)->base.rotation);
    else if(strcmp(key, "visible") == 0)
        lua_pushboolean(L, (*self)->base.visible);
    else if(strcmp(key, "flip") == 0)
        lua_pushboolean(L, (*self)->base.flip);
    else if(strcmp(key, "name") == 0)
        lua_rawgeti(L, LUA_REGISTRYINDEX, (*self)->base.name.ref);
    else if(strcmp(key, "text") == 0)
        lua_rawgeti(L, LUA_REGISTRYINDEX, (*self)->text.ref);
    else if(strcmp(key, "fontPath") == 0)
        lua_rawgeti(L, LUA_REGISTRYINDEX, (*self)->fontPath.ref);
    else if(strcmp(key, "fontSize") == 0)
        lua_pushinteger(L, (*self)->fontSize);
    else
        lua_getDinamicField(L, 1, key);

    return 1;
}

static int Label_newindexLua(lua_State* L)
{
    Label** self = (Label**)lua_checkinstance(L, 1, "Label");
    const char* key = luaL_checkstring(L, 2);
    if(strcmp(key, "pos") == 0)
    {
        Vector2* value = (Vector2*)luaL_checkudata(L, 3, "Vector2");
        MadObject_setPosition((MadObject*)(*self), *value);
    }
    else if(strcmp(key, "pos_x") == 0)
    {
        float value = (float)luaL_checknumber(L, 3);
        MadObject_setPosition((MadObject*)(*self), (Vector2){value, (*self)->base.pos.y});
    }
    else if(strcmp(key, "pos_y") == 0) \
    {
        float value = (float)luaL_checknumber(L, 3);
        MadObject_setPosition((MadObject*)(*self), (Vector2){(*self)->base.pos.x, value});
    }
    else if(strcmp(key, "globalPos") == 0)
    {
        Vector2* value = (Vector2*)luaL_checkudata(L, 3, "Vector2");
        MadObject_setGlobalPosition((MadObject*)(*self), *value); \
    } \
    else if(strcmp(key, "globalPos_x") == 0)
    {
        float value = (float)luaL_checknumber(L, 3);
        MadObject_setGlobalPosition((MadObject*)(*self), (Vector2){value, (*self)->base.globalPos.y});
    }
    else if(strcmp(key, "globalPos_y") == 0)
    {
        float value = (float)luaL_checknumber(L, 3);
        MadObject_setGlobalPosition((MadObject*)(*self), (Vector2){(*self)->base.globalPos.x, value}); \
    }
    else if(strcmp(key, "size") == 0)
    {
        Vector2* value = (Vector2*)luaL_checkudata(L, 3, "Vector2");
        (*self)->base.size = *value;
    }
    else if(strcmp(key, "scale") == 0)
    {
        Vector2* value = (Vector2*)luaL_checkudata(L, 3, "Vector2"); \
        (*self)->base.scale = *value; \
    }
    else if(strcmp(key, "scale_x") == 0)
    {
        float value = (float)luaL_checknumber(L, 3);
        MadObject_setScale((MadObject*)(*self), (Vector2){value, (*self)->base.scale.y});
    }
    else if(strcmp(key, "scale_y") == 0)
    {
        float value = (float)luaL_checknumber(L, 3);
        MadObject_setScale((MadObject*)(*self), (Vector2){(*self)->base.scale.x, value});
    }
    else if(strcmp(key, "color") == 0)
    {
        Color* value = (Color*)luaL_checkudata(L, 3, "Color");
        MadObject_setColor((MadObject*)(*self), *value);
    }
    else if(strcmp(key, "color_r") == 0)
    {
        int value = luaL_checkinteger(L, 3);
        INT_TO_UINT8(value);
        MadObject_setColor((MadObject*)(*self), (Color){(Uint8)value, (*self)->base.color.g, (*self)->base.color.b, (*self)->base.color.a});
        (*self)->mustBeRerendered = true;
    }
    else if(strcmp(key, "color_g") == 0)
    {
        int value = luaL_checkinteger(L, 3);
        INT_TO_UINT8(value);
        MadObject_setColor((MadObject*)(*self), (Color){(*self)->base.color.r, (Uint8)value, (*self)->base.color.b, (*self)->base.color.a});
        (*self)->mustBeRerendered = true;
    }
    else if(strcmp(key, "color_b") == 0)
    {
        int value = luaL_checkinteger(L, 3);
        INT_TO_UINT8(value);
        MadObject_setColor((MadObject*)(*self), (Color){(*self)->base.color.r, (*self)->base.color.g, (Uint8)value, (*self)->base.color.a});
        (*self)->mustBeRerendered = true;
    }
    else if(strcmp(key, "color_a") == 0)
    {
        int value = luaL_checkinteger(L, 3);
        INT_TO_UINT8(value);
        MadObject_setColor((MadObject*)(*self), (Color){(*self)->base.color.r, (*self)->base.color.g, (*self)->base.color.b, (Uint8)value});
        (*self)->mustBeRerendered = true;
    }
    else if(strcmp(key, "rotation") == 0)
    {
        float value = luaL_checknumber(L, 3);
        (*self)->base.rotation = value;
    }
    else if(strcmp(key, "visible") == 0)
    {
        if(lua_isboolean(L, 3))
        {
            int value = lua_toboolean(L, 3);
            (*self)->base.visible = value;
        }
    }
    else if(strcmp(key, "flip") == 0)
    {
        if(lua_isboolean(L, 3))
        {
            int value = lua_toboolean(L, 3);
            (*self)->base.flip = value;
        }
    }
    else if(strcmp(key, "name") == 0)
    {
        if(lua_isstring(L, 3))
        { 
            String_free((*self)->base.name);
            (*self)->base.name = String_newFromLua(L, 3);
        }
    }
    else if(strcmp(key, "text") == 0)
    {
        if(lua_isstring(L, 3))
        {
            String_free((*self)->text);
            (*self)->text = String_newFromLua(L, 3);
        }
    }
    else if(strcmp(key, "fontSize") == 0)
        Label_setFontSize(*self, luaL_checkinteger(L, 3));
    else
        lua_setDinamicField(L, 1, key);
    
    return 0;
}

static int Label_gc(lua_State* L)
{
    Label** self = (Label**)lua_checkinstance(L, 1, "Label");
    if(*self != NULL)
    {
        Label_free(*self);
        free(*self);
        *self = NULL;
    }
    return 0;
}

static int Label_loadFontLua(lua_State* L)
{
    Label** self = (Label**)lua_checkinstance(L, 1, "Label");
    String fontPath = String_newFromLua(L, 2);
    int fontSize = luaL_checkinteger(L, 3);
    Label_loadFont(*self, fontPath, fontSize);
    String_free(fontPath);
    return 0;
}

static int Label_applyGlowFilterLua(lua_State* L)
{
    Label** self = (Label**)lua_checkinstance(L, 1, "Label");
    Color* glowColor = (Color*)luaL_checkudata(L, 2, "Color");
    int intensity = luaL_checkinteger(L, 3);
    double glowSize = luaL_checkinteger(L, 4);
    Label_applyGlowFilter(*self, *glowColor, intensity, glowSize);
    return 0;
}

static int Label_applyShadowFilterLua(lua_State* L)
{
    Label** self = (Label**)lua_checkinstance(L, 1, "Label");
    Color* shadowColor = (Color*)luaL_checkudata(L, 2, "Color");
    int blurRadius = luaL_checkinteger(L, 3);
    double shadowDistance = luaL_checknumber(L, 4);
    double sigma = 2.0;
    if(!lua_isnone(L, 5) && !lua_isnil(L, 5))
        sigma = luaL_checknumber(L, 5);
    Label_applyShadowFilter(*self, *shadowColor, blurRadius, shadowDistance, sigma);
    return 0;
}

static int Label_applyOutlineLua(lua_State* L)
{
    Label** self = (Label**)lua_checkinstance(L, 1, "Label");
    Color* outlineColor = luaL_checkudata(L, 2, "Color");
    int outlineSize = luaL_checkinteger(L, 3);
    Label_applyOutline(*self, *outlineColor, outlineSize);
    return 0;
}

void Label_register(lua_State* L)
{
    luaL_newmetatable(L, "Label");

    lua_pushcfunction(L, Label_indexLua);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, Label_newindexLua);
    lua_setfield(L, -2, "__newindex");

    lua_pushcfunction(L, Label_gc);
    lua_setfield(L, -2, "__gc");

    lua_pushcfunction(L, Label_loadFontLua);
    lua_setfield(L, -2, "loadFont");

    lua_pushcfunction(L, Label_applyGlowFilterLua);
    lua_setfield(L, -2, "applyGlowFilter");

    lua_pushcfunction(L, Label_applyShadowFilterLua);
    lua_setfield(L, -2, "applyShadowFilter");

    lua_pushcfunction(L, Label_applyOutlineLua);
    lua_setfield(L, -2, "applyOutline");

    lua_pushcfunction(L, Label_newLua);
    lua_setfield(L, -2, "new");

    luaL_getmetatable(L, "MadObject");
    lua_setfield(L, -2, "__extends");

    lua_setglobal(L, "Label");
}
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

static void Label_setSize(Label* self, Vector2 value)
{
    self->base.size = value;
}

static int Label_getFontSize(Label* self)
{
    return self->fontSize;
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
    /*
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
    */
    Lua_registerclass("Label",
        CONSTRUCTOR, Label_new,
        DESTRUCTOR, Label_free,
        EXTENDS, "MadObject",
        FIELDGETTERSETTER, "size", TVECTOR2, Label_getSize, Label_setSize,
        FIELD, "text", TSTRING, offsetof(Label, text),
        FIELDREADONLY, "fontPath", TSTRING, offsetof(Label, fontPath),
        FIELDGETTERSETTER, "fontSize", TINT, Label_getFontSize, Label_setFontSize,
        FUNC, "loadFont", Label_loadFontLua,
        FUNC, "applyGlowFilter", Label_applyGlowFilterLua,
        FUNC, "applyShadowFilter", Label_applyShadowFilterLua,
        FUNC, "applyOutline", Label_applyOutlineLua,
        END
    );
}
#include "Image.h"
#include "Render/Renderer.h"
#include "Math/SoulMath.h"
#include "Core/Graphics/SurfaceEffects.h"
#include <SDL_image.h>

Image* Image_new(String path)
{
    SDL_Renderer* renderer = GetRenderer();
    Image* instance = (Image*)malloc(sizeof(Image));
    MADOBJECT_NEW((MadObject*)instance)
    instance->image_buffer = IMG_LoadTexture(renderer, path.data);
    if(instance->image_buffer == NULL)
    {
        SDL_Log("Failed to load image '%s': %s", path.data, SDL_GetError());
        return instance;
    }
    int width, height;
    SDL_QueryTexture(instance->image_buffer, NULL, NULL, &width, &height);
    instance->base.size.x = (float)width;
    instance->base.size.y = (float)height;
    instance->path = String_dup(path);
    instance->base.draw = Image_draw;
    return instance;
}

Image* Image_noise(Vector2 size)
{
    SDL_Surface* noiseSurface = SurfaceEffects.noise(size);
    // Creamos la instancia de Image.
    SDL_Renderer* renderer = GetRenderer();
    Image* instance = (Image*)malloc(sizeof(Image));
    MADOBJECT_NEW((MadObject*)instance);
    instance->image_buffer = SDL_CreateTextureFromSurface(renderer, noiseSurface);
    SDL_FreeSurface(noiseSurface);
    instance->path = String_new("noise.jpg");
    instance->base.size = size;
    instance->base.draw = Image_draw;
    return instance;
}

void Image_free(Image* self)
{
    MadObject_free((MadObject*)self);
    SDL_DestroyTexture(self->image_buffer);
    String_free(self->path);
}

void Image_draw(void* self, SDL_Renderer* renderer)
{
    Image* image = (Image*)self;
    SDL_Rect dstRect;
    dstRect.x = (int)image->base.globalPos.x;
    dstRect.y = (int)image->base.globalPos.y;
    dstRect.w = (int)(image->base.size.x * image->base.globalScale.x);
    dstRect.h = (int)(image->base.size.y * image->base.globalScale.y);
    SDL_Point center = {(int)(dstRect.w / 2), (int)(dstRect.h / 2)};
    SDL_SetTextureColorMod(image->image_buffer, image->base.globalColor.r, image->base.globalColor.g, image->base.globalColor.b);
    SDL_SetTextureAlphaMod(image->image_buffer, image->base.globalColor.a);
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if(image->base.flip)
        flip = SDL_FLIP_HORIZONTAL;
        
    SDL_RenderCopyEx(renderer, image->image_buffer, NULL, &dstRect, image->base.globalRotation, &center, flip);
}

Image* Image_makeGlowBorder(Image* self, const Color borderColor, const int intensity, const int borderSize)
{
    SDL_Renderer* renderer = GetRenderer();
    SDL_Surface* imageSurface = SurfaceEffects.CreateSurfaceFromTexture(renderer, self->image_buffer, self->base.size);
    SDL_Surface* glowSurface = SurfaceEffects.makeGlowBorder(imageSurface, borderColor, intensity, borderSize);
    // Creamos la instancia de "Image"
    Image* instance = (Image*)malloc(sizeof(Image));
    // Copiamos toda la informacion base de la instancia original.
    instance->base = self->base;
    // Eliminamos informacion que no debe estar.
    instance->base.childCount = 0;
    instance->base.queued = false;
    instance->base.parent = instance->base.last_child = NULL;
    instance->base.first_child = instance->base.next_sibling = NULL_MADOBJECTREF;
    instance->base.L = NULL;
    instance->base.self_reference = LUA_NOREF;
    // Iniciaizamos ahora la seccion de la instancia "Image"
    instance->base.size = (Vector2){(double)glowSurface->w, (double)glowSurface->h};
    instance->image_buffer = SDL_CreateTextureFromSurface(renderer, glowSurface);
    instance->path = String_dup(self->path);
    // Liberamos los SDL_Surface ya que ya no son utiles.
    SDL_FreeSurface(imageSurface);
    SDL_FreeSurface(glowSurface);
    return instance;
}

Image* Image_makeImageColor(Image* self, const Color colorImage)
{
    SDL_Renderer* renderer = GetRenderer();
    // Creamos un Surface a partir de la textura de la imagen original para poder editar sus pixeles
    SDL_Surface* imageSurface = SurfaceEffects.CreateSurfaceFromTexture(renderer, self->image_buffer, self->base.size);
    SurfaceEffects.applyColorOverlay(imageSurface, colorImage);
    // Creamos la instancia de Image, copiamos informacion importante y eliminamos la que no lo es.
    Image* instance = (Image*)malloc(sizeof(Image));
    instance->base = self->base;
    instance->base.childCount = 0;
    instance->base.queued = false;
    instance->base.parent = NULL;
    instance->base.first_child = instance->base.next_sibling = NULL_MADOBJECTREF;
    instance->base.L = NULL;
    instance->base.self_reference = LUA_NOREF;
    // Ahora inicializamos la instancia.
    instance->image_buffer = SDL_CreateTextureFromSurface(renderer, imageSurface);
    instance->path = String_dup(self->path);
    // Liberamos el surface porque ya no nos sirve.
    SDL_FreeSurface(imageSurface);
    return instance;
}

/* <------- BINDINGS ------->*/

static int Image_newLua(lua_State* L)
{
    String path = String_newFromLua(L, 1);
    Image* instance = Image_new(path);
    String_free(path);
    Image** instanceLua = (Image**)lua_newuserdata(L, sizeof(Image*));
    *instanceLua = instance;
    MADOBJECT_NEWLUA((MadObject*)instance)

    luaL_getmetatable(L, "Image");
    lua_setmetatable(L, -2);
    return 1;
}

static int Image_noiseLua(lua_State* L)
{
    Vector2 size;
    if(luaL_testudata(L, 1, "Vector2") != NULL)
        size = *(Vector2*)lua_touserdata(L, 1);
    else
        size = (Vector2){luaL_checknumber(L, 1), luaL_checknumber(L, 2)};
    Image* instance = Image_noise(size);
    Image** instanceLua = (Image**)lua_newuserdata(L, sizeof(Image*));
    *instanceLua = instance;
    MADOBJECT_NEWLUA((MadObject*)instance);

    luaL_getmetatable(L, "Image");
    lua_setmetatable(L, -2);
    return 1;
}

static int Image_indexLua(lua_State* L)
{
    Image** self = (Image**)lua_checkinstance(L, 1, "Image");
    const char* key = luaL_checkstring(L, 2);
    MADOBJECT_INDEXLUA((MadObject**)self)
    else if(strcmp(key, "path") == 0)
        lua_rawgeti(L, LUA_REGISTRYINDEX, (*self)->path.ref);
    else
        lua_getDinamicField(L, 1, key);

    return 1;
}

// El Metametodo __newindex no es necesario definir pues 'path' es de solo lectura para lua y 'image_buffer' no es visible para lua y tampoco no hay mas atributos.

static int Image_makeGlowBorderLua(lua_State* L)
{
    Image** self = (Image**)lua_checkinstance(L, 1, "Image");
    Color* borderColor = (Color*)luaL_checkudata(L, 2, "Color");
    int intensity = luaL_checkinteger(L, 3);
    int borderSize = luaL_checkinteger(L, 4);
    Image* instance = Image_makeGlowBorder(*self, *borderColor, intensity, borderSize);
    Image** instanceLua = lua_newuserdata(L, sizeof(Image*));
    *instanceLua = instance;
    MADOBJECT_NEWLUA((MadObject*)instance);
    luaL_getmetatable(L, "Image");
    lua_setmetatable(L, -2);
    return 1;
}

static int Image_makeImageColorLua(lua_State* L)
{
    Image** self = (Image**)lua_checkinstance(L, 1, "Image");
    Color* colorImage = (Color*)luaL_checkudata(L, 2, "Color");
    Image* instance = Image_makeImageColor(*self, *colorImage);
    Image** instanceLua = lua_newuserdata(L, sizeof(Image*));
    *instanceLua = instance;
    MADOBJECT_NEWLUA((MadObject*)instance);
    luaL_getmetatable(L, "Image");
    lua_setmetatable(L, -2);
    return 1;
}

static int Image_gc(lua_State* L)
{
    Image** self = (Image**)lua_checkinstance(L, 1, "Image");
    if(*self != NULL)
    {
        Image_free(*self);
        free(*self);
        *self = NULL;
    }
    return 0;
}

void Image_register(lua_State* L)
{
    luaL_newmetatable(L, "Image");

    lua_pushcfunction(L, Image_indexLua);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, Image_gc);
    lua_setfield(L, -2, "__gc");

    lua_pushcfunction(L, Image_newLua);
    lua_setfield(L, -2, "load");

    lua_pushcfunction(L, Image_noiseLua);
    lua_setfield(L, -2, "noise");

    lua_pushcfunction(L, Image_makeGlowBorderLua);
    lua_setfield(L, -2, "makeGlowBorder");

    lua_pushcfunction(L, Image_makeImageColorLua);
    lua_setfield(L, -2, "makeImageColor");

    luaL_getmetatable(L, "MadObject");
    lua_getfield(L, -1, "__newindex");
    lua_setfield(L, -3, "__newindex"); // Image.__newindex = MadObject.__newindex

    lua_setfield(L, -2, "__extends"); // Indicamos que la "Clase" Image "hereda" de MadObject.

    lua_setglobal(L, "Image");
}
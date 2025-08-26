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
    instance->imageBuffer = IMG_LoadTexture(renderer, path.data);
    if(instance->imageBuffer == NULL)
    {
        SDL_Log("Failed to load image '%s': %s", path.data, SDL_GetError());
        return instance;
    }
    int width, height;
    SDL_QueryTexture(instance->imageBuffer, NULL, NULL, &width, &height);
    instance->base.size.x = (double)width;
    instance->base.size.y = (double)height;
    instance->imageBufferSize = instance->base.size;
    instance->sizeMode = STRETCH;
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
    instance->imageBuffer = SDL_CreateTextureFromSurface(renderer, noiseSurface);
    SDL_FreeSurface(noiseSurface);
    instance->path = String_new("noise.jpg");
    instance->sizeMode = STRETCH;
    instance->base.size = size;
    instance->base.draw = Image_draw;
    return instance;
}

void Image_free(Image* self)
{
    MadObject_free((MadObject*)self);
    SDL_DestroyTexture(self->imageBuffer);
    String_free(self->path);
}

void Image_draw(void* self, SDL_Renderer* renderer)
{
    Image* image = (Image*)self;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    SDL_Point center;
    SDL_SetTextureColorMod(image->imageBuffer, image->base.globalColor.r, image->base.globalColor.g, image->base.globalColor.b);
    SDL_SetTextureAlphaMod(image->imageBuffer, image->base.globalColor.a);
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if(image->base.flip)
        flip = SDL_FLIP_HORIZONTAL;
    // Calculamos su tamaño final por la escala global
    Vector2 finalSize = (Vector2){
        image->base.size.x * image->base.globalScale.x,
        image->base.size.y * image->base.globalScale.y
    };
    // Calculamos el tamaño final de la imagen por la escala global
    Vector2 finalImageSize = (Vector2){
        image->imageBufferSize.x * image->base.globalScale.x,
        image->imageBufferSize.y * image->base.globalScale.y
    };
    switch(image->sizeMode)
    {
        case STRETCH:
        {
            dstRect.x = (int)image->base.globalPos.x;
            dstRect.y = (int)image->base.globalPos.y;
            dstRect.w = (int)(finalSize.x);
            dstRect.h = (int)(finalSize.y);
            center = (SDL_Point){(int)(dstRect.w / 2), (int)(dstRect.h / 2)};
            SDL_RenderCopyEx(renderer, image->imageBuffer, NULL, &dstRect, image->base.globalRotation, &center, flip);
            break;
        }
        case ZOOM:
        {
            double scaleX = finalSize.x / finalImageSize.x;
            double scaleY = finalSize.y / finalImageSize.y;
            double scale;
            // Usamos la escala mas chica
            if(scaleX < scaleY)
                scale = scaleX;
            else
                scale = scaleY;
            Vector2 scaledSize = Vector2_mul(finalImageSize, scale);
            // centeredPos = image->base.globalPos + (finalSize - scaledSize) / 2
            Vector2 centeredPos = Vector2_add(image->base.globalPos, Vector2_div(Vector2_sub(finalSize, scaledSize), 2));
            dstRect.x = (int)centeredPos.x;
            dstRect.y = (int)centeredPos.y; 
            dstRect.w = (int)scaledSize.x;
            dstRect.h = (int)scaledSize.y;
            center = (SDL_Point){(int)(dstRect.w / 2), (int)(dstRect.h / 2)};
            SDL_RenderCopyEx(renderer, image->imageBuffer, NULL, &dstRect, image->base.globalRotation, &center, flip);
            break;
        }
        case CUT:
        {
            if(finalSize.x >= finalImageSize.x && finalSize.y >= finalImageSize.y) // Tienen el mismo tamaño?
            {
                dstRect.x = (int)image->base.globalPos.x;
                dstRect.y = (int)image->base.globalPos.y;
                dstRect.w = (int)(finalSize.x);
                dstRect.h = (int)(finalSize.y);
                center = (SDL_Point){(int)(dstRect.w / 2), (int)(dstRect.h / 2)};
                SDL_RenderCopyEx(renderer, image->imageBuffer, NULL, &dstRect, image->base.globalRotation, &center, flip);
            }
            else if(finalSize.x != 0 && finalSize.y != 0)
            {
                srcRect.x = 0;
                srcRect.y = 0;
                // Recortamos la imagen al tamaño dado
                srcRect.w = (int)image->base.size.x;
                srcRect.h = (int)image->base.size.y;
                dstRect.x = (int)image->base.globalPos.x;
                dstRect.y = (int)image->base.globalPos.y;
                dstRect.w = (int)finalSize.x;
                dstRect.h = (int)finalSize.y;
                center = (SDL_Point){(int)(dstRect.w / 2), (int)(dstRect.h / 2)};
                SDL_RenderCopyEx(renderer, image->imageBuffer, &srcRect, &dstRect, image->base.globalRotation, &center, flip);
            }
            break;
        }
        default:
            break;
    }
}

Image* Image_makeGlowBorder(Image* self, const Color borderColor, const int intensity, const int borderSize)
{
    SDL_Renderer* renderer = GetRenderer();
    SDL_Surface* imageSurface = SurfaceEffects.CreateSurfaceFromTexture(renderer, self->imageBuffer, self->base.size);
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
    instance->imageBuffer = SDL_CreateTextureFromSurface(renderer, glowSurface);
    instance->sizeMode = STRETCH;
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
    SDL_Surface* imageSurface = SurfaceEffects.CreateSurfaceFromTexture(renderer, self->imageBuffer, self->base.size);
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
    instance->imageBuffer = SDL_CreateTextureFromSurface(renderer, imageSurface);
    instance->sizeMode = STRETCH;
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

void Image_register(lua_State* L)
{
    Lua_registerclass("Image",
        FUNC, "load", Image_newLua,
        FUNC, "noise", Image_noiseLua,
        DESTRUCTOR, Image_free,
        EXTENDS, "MadObject",
        FIELDREADONLY, "path", TSTRING, offsetof(Image, path),
        FIELD, "sizeMode", TINT, offsetof(Image, sizeMode),
        FUNC, "makeGlowBorder", Image_makeGlowBorderLua,
        FUNC, "makeImageColor", Image_makeImageColorLua,
        END
    );
    Lua_registerenum("SizeMode", 3,
        "STRETCH", STRETCH,
        "ZOOM", ZOOM,
        "CUT", CUT
    );
}
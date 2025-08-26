#include "Gif.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"

Gif* Gif_new(String path)
{
    SDL_Renderer* renderer = GetRenderer();
    IMG_Animation* gifFile = IMG_LoadAnimation(path.data);
    if(gifFile == NULL)
        printf("Error loading a gif: %s\n", IMG_GetError());
    int framesCount = gifFile->count;
    Gif* instance = (Gif*)malloc(sizeof(Gif) + sizeof(struct gifFrame) * framesCount);
    MADOBJECT_NEW((MadObject*)instance);
    instance->base.size = (Vector2){(double)gifFile->w, (double)gifFile->h};
    instance->base.draw = Gif_draw;
    instance->loop = true;
    instance->path = String_dup(path);
    instance->currentFrame = 1;
    instance->playing = false;
    instance->elapsedTime = 0.0f;
    instance->framesCount = framesCount;
    // Convertimos cada frame(SDL_Surface) del gif a un SDL_Texture
    for(int i = 0; i < framesCount; i++)
    {
        instance->frames[i].frameTexture = SDL_CreateTextureFromSurface(renderer, gifFile->frames[i]);
        // Convertir de centecimas de segundos a segundos.
        instance->frames[i].frameDelay = (double)gifFile->delays[i] / 1000.0;
    }
    IMG_FreeAnimation(gifFile);
    return instance;
}

void Gif_free(Gif* self)
{
    MadObject_free((MadObject*)self);
    for(int i = 0; i < self->framesCount; i++)
    {
        if(self->frames[i].frameTexture != NULL)
            SDL_DestroyTexture(self->frames[i].frameTexture);
    }
    String_free(self->path);
}

void Gif_draw(void* self, SDL_Renderer* renderer)
{
    Gif* gif = (Gif*)self;
    // Dibujar el frame actual.
    SDL_Texture* frameTexture = gif->frames[gif->currentFrame - 1].frameTexture;
    SDL_Rect dstRect;
    dstRect.x = (int)gif->base.globalPos.x;
    dstRect.y = (int)gif->base.globalPos.y;
    dstRect.w = (int)(gif->base.size.x * gif->base.globalScale.x);
    dstRect.h = (int)(gif->base.size.y * gif->base.globalScale.y);
    SDL_Point center = {(int)(dstRect.w / 2), (int)(dstRect.h / 2)};
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if(gif->base.flip)
        flip = SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, frameTexture, NULL, &dstRect, gif->base.globalRotation, &center, flip);
    // Actualizar el frame que dibujar.
    float deltaTime = Engine_getInstance()->deltaTime;
    if(gif->playing)
    {
        gif->elapsedTime += deltaTime;
        if(gif->elapsedTime >= gif->frames[gif->currentFrame - 1].frameDelay)
        {
            gif->elapsedTime = 0.0f;
            gif->currentFrame++;
            if(gif->currentFrame >= gif->framesCount)
            {
                gif->currentFrame = 1;
                if(!gif->loop)
                    gif->playing = false;
            }
        }
    }
}

Image* Gif_getFrame(Gif* self, const int frame)
{
    if(frame > 0 && frame < self->framesCount)
    {
        // Duplicamos la textura.
        SDL_Texture* frameDuplicated = Gif_DuplicateTexture(self->frames[frame].frameTexture);
        // Creamos una "instancia" nueva de Image y llamamos a su "constructor".
        Image* image = (Image*)malloc(sizeof(Image));
        MADOBJECT_NEW((MadObject*)self);
        image->imageBuffer = frameDuplicated;
        image->path = String_dup(self->path);
        int width, height;
        SDL_QueryTexture(frameDuplicated, NULL, NULL, &width, &height);
        image->base.size.x = width;
        image->base.size.y = height;
        return image;
    }
    else
    {
        SDL_Log("'frame' cannot be zero, negative or greater than the frames count.\n");
        return NULL;
    }
}

void Gif_gotoAndStop(Gif* self, const int frame)
{
    if(frame > 0 && frame < self->framesCount)
    {
        self->currentFrame = frame;
        self->elapsedTime = 0.0f;
        if(self->playing)
            self->playing = false;
    }
    else
    {
        SDL_Log("'frame' cannot be zero, negative or greater than the frames count.\n");
    }
}

void Gif_gotoAndPlay(Gif* self, const int frame)
{
    Gif_gotoAndStop(self, frame);
    self->playing = true;
}

SDL_Texture* Gif_DuplicateTexture(SDL_Texture* texture)
{
    SDL_Renderer* renderer = GetRenderer();
    Uint32 format;
    int access;
    int width;
    int height;
    if (SDL_QueryTexture(texture, &format, &access, &width, &height) != 0) {
        SDL_Log("Error to query the texture: %s", SDL_GetError());
        return NULL;
    }
    SDL_Texture* duplicate = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, width, height);
    if (!duplicate) {
        SDL_Log("Error to create the duplicated texture: %s", SDL_GetError());
        return NULL;
    }
    SDL_SetRenderTarget(renderer, duplicate);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_SetRenderTarget(renderer, NULL);
    return duplicate;
}

/* <------- BINDINGS -------> */

static int Gif_newLua(lua_State* L)
{
    String path = String_newFromLua(L, 1);
    Gif* instance = Gif_new(path);
    String_free(path);
    if(instance == NULL)
    {
        lua_pushnil(L);
        return 1;
    }
    Gif** instanceLua = (Gif**)lua_newuserdata(L, sizeof(Gif*));
    *instanceLua = instance;
    MADOBJECT_NEWLUA((MadObject*)instance);

    luaL_getmetatable(L, "Gif");
    lua_setmetatable(L, -2);

    return 1;
}

int Gif_getFrameLua(lua_State* L)
{
    Gif** self = (Gif**)lua_checkinstance(L, 1, "Gif");
    int frame = luaL_checkinteger(L, 2);
    Image* image = Gif_getFrame(*self, frame);
    if(image != NULL)
    {
        Image** imageLua = (Image**)lua_newuserdata(L, sizeof(Image*));
        *imageLua = image;
        MADOBJECT_NEWLUA((MadObject*)imageLua);

        luaL_getmetatable(L, "Image");
        lua_setmetatable(L, -2);
    }
    else
        lua_pushnil(L);
    return 1;
}

int Gif_gotoAndStopLua(lua_State* L)
{
    Gif** self = (Gif**)lua_checkinstance(L, 1, "Gif");
    int frame = luaL_checkinteger(L, 2);
    Gif_gotoAndStop(*self, frame);
    return 0;
}

int Gif_gotoAndPlayLua(lua_State* L)
{
    Gif** self = (Gif**)lua_checkinstance(L, 1, "Gif");
    int frame = luaL_checkinteger(L, 2);
    Gif_gotoAndPlay(*self, frame);
    return 0;
}

void Gif_register(lua_State* L)
{
    /*
    luaL_newmetatable(L, "Gif");

    lua_pushcfunction(L, Gif_newLua);
    lua_setfield(L, -2, "load");

    lua_pushcfunction(L, Gif_indexLua);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, Gif_newindexLua);
    lua_setfield(L, -2, "__newindex");

    lua_pushcfunction(L, Gif_gc);
    lua_setfield(L, -2, "__gc");

    lua_pushcfunction(L, Gif_getFrameLua);
    lua_setfield(L, -2, "getFrame");

    lua_pushcfunction(L, Gif_gotoAndStopLua);
    lua_setfield(L, -2, "gotoAndStop");

    lua_pushcfunction(L, Gif_gotoAndPlayLua);
    lua_setfield(L, -2, "gotoAndPlay");

    luaL_getmetatable(L, "MadObject");
    lua_setfield(L, -2, "__extends");

    lua_setglobal(L, "Gif");
    */
    Lua_registerclass("Gif",
        FUNC, "load", Gif_newLua,
        DESTRUCTOR, Gif_free,
        EXTENDS, "MadObject",
        FIELD, "loop", TBOOL, offsetof(Gif, loop),
        FIELDREADONLY, "path", TSTRING, offsetof(Gif, path),
        FIELDREADONLY, "currentFrame", TINT, offsetof(Gif, currentFrame),
        FIELD, "playing", TBOOL, offsetof(Gif, playing),
        FIELDREADONLY, "framesCount", TINT, offsetof(Gif, framesCount),
        FUNC, "getFrame", Gif_getFrameLua,
        FUNC, "gotoAndStop", Gif_gotoAndStopLua,
        FUNC, "gotoAndPlay", Gif_gotoAndPlayLua,
        END
    );
}
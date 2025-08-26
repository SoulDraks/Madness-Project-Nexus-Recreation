#ifndef GIF_H
#define GIF_H

#include "Image.h"
#include <SDL_image.h>

struct gifFrame {
    SDL_Texture* frameTexture;
    double frameDelay;
};

typedef struct {
    MadObject base;
    bool loop;
    String path;
    int currentFrame; // Define en que frame se encuentra.
    bool playing; // Define si se encuentra reproduciendose.
    double elapsedTime; // Tiempo transcurrido entre frames.
    int framesCount; // Cantidad de frames que posee el Gif.
    struct gifFrame frames[]; // Array de los frames del gif.
} Gif;

extern Gif* Gif_new(String path);
extern void Gif_free(Gif* self);
extern void Gif_draw(void* self, SDL_Renderer* renderer);
// Devuelve una instancia de Image del frame dado del gif.
extern Image* Gif_getFrame(Gif* self, const int frame);
// Va a al frame del GIF dado y se detiene.
extern void Gif_gotoAndStop(Gif* self, const int frame);
// Va a al frame del GIF dado y reproduce desde alli.
extern void Gif_gotoAndPlay(Gif* self, const int frame);
extern void Gif_register(lua_State* L);
extern SDL_Texture* Gif_DuplicateTexture(SDL_Texture* texture);

#endif
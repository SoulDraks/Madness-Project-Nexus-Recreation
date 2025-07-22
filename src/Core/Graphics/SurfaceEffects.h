#ifndef SURFACEEFFECTS_H
#define SURFACEEFFECTS_H

#include <SDL.h>
#include "types/Vector2.h"
#include "types/Color.h"

typedef struct {
    SDL_Surface* (*CreateSurfaceFromTexture)(SDL_Renderer* renderer, SDL_Texture* texture, Vector2 size);
    SDL_Surface* (*makeGlowBorder)(SDL_Surface* original, Color borderColor, int intensity, int borderSize);
    void (*applyColorOverlay)(SDL_Surface* surface, Color colorOverlay);
    SDL_Surface* (*noise)(Vector2 size);
    void (*applyBlur)(SDL_Surface* surface, int radius, double sigma);
} SurfaceEffectsClass;

extern const SurfaceEffectsClass SurfaceEffects;

#endif
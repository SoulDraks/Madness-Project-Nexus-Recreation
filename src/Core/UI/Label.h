#ifndef LABEL_H
#define LABEL_H

#include "Core/MadObject.h"
#include <SDL_ttf.h>

typedef struct {
    MadObject base;
    String text; 
    String fontPath; // Ruta a la fuente actual cargada. Atributo de solo lectura
    int fontSize;

    /* <--------- Parte Privada ---------> */

    bool mustBeRerendered; // Bandera que define si el label debe ser Rerenderizado
    SDL_Texture* renderedText; // Textura del texto renderizado

    /* Filtro de sombra */
    bool hasShadowFilter;
    SDL_Texture* shadowText; // filtro o textura de la sombra de texto
    Color shadowTextColor; // Color de la sombra
    int blurRadius; // Radio del efecto de blur aplicado a la sombra.
    double shadowTextDistance; // Distancia de la sombra respecto al filtro de sombra
    double sigmaEffect;

    /* Filtro de brillo */
    bool hasGlowFilter;
    SDL_Texture* glowText; // Textura del texto del filtro de brillo.
    Color glowTextColor; // Color del filtro de brillo.
    int glowIntensity; // Intensidad del color del filtro de brillo.
    int glowSize; // Tamaño del filtro de brillo.
    Vector2 glowTextSize; // Tamaño de la textura del filtro de brillo del texto.

    /* Outline o bordeado */
    bool hasOutline;
    SDL_Texture* outlineText;
    Color outlineColor;
    int outlineSize;
    Vector2 outlineTextSize; // Tamaño de la textura del outline o bordeado

    TTF_Font* fontBuffer;
} Label;

extern Label* Label_new();
extern void Label_free(Label* self);
extern void Label_draw(void* self, SDL_Renderer* renderer);
extern Vector2 Label_getSize(Label* self);
extern void Label_setFontSize(Label* self, const int size);
extern void Label_loadFont(Label* self, String fontPath, const int fontSize);
extern void Label_applyGlowFilter(Label* self, Color glowColor, int intensity, int glowSize);
extern void Label_applyShadowFilter(Label* self, Color shadowColor, int blurRadius, double shadowDistance, double sigma);
extern void Label_applyOutline(Label* self, Color outlineColor, int outlineSize);
extern void Label_register(lua_State* L);

#endif
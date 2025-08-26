#ifndef IMAGE_H
#define IMAGE_H
#include "Core/MadObject.h"

// Modos de tamaño para las imagenes.
typedef enum {
    STRETCH, // Estira la imagen en caso independiente de su tamaño
    ZOOM, // Centra y mantiene la imagen respetando su tamaño original escalandola
    CUT // La imagen mantiene siempre su tamaño original
} SizeMode;

typedef struct {
    MadObject base;
    String path; // Ruta a la imagen de la que fue cargada.
    SizeMode sizeMode;
    Vector2 imageBufferSize; // Tamaño de la imagen original.
    SDL_Texture* imageBuffer; // Almacena la textura de la imagen.
} Image;

extern Image* Image_new(String path);
extern Image* Image_noise(Vector2 size);
extern void Image_free(Image* self);
extern void Image_draw(void* self, SDL_Renderer* renderer);
// Genera una nueva imagen o textura que actua como el borde de la imagen dada.
extern Image* Image_makeGlowBorder(Image* self, const Color borderColor, const int intensity, const int borderSize);
extern Image* Image_makeImageColor(Image* self, const Color colorImage);
extern void Image_register(lua_State* L);

#endif
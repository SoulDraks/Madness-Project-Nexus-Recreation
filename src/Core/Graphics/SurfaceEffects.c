#include "SurfaceEffects.h"
#include "Math/SoulMath.h"

static SDL_Surface* CreateSurfaceFromTexture(SDL_Renderer* renderer, SDL_Texture* texture, Vector2 size)
{
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, (int)size.x, (int)size.y, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_Texture* renderTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)size.x, (int)size.y);
    SDL_Texture* oldRenderTarget = SDL_GetRenderTarget(renderer); // guardamos el render target
    SDL_SetRenderTarget(renderer, renderTarget);
    SDL_RenderClear(renderer);
    SDL_SetTextureBlendMode(renderTarget, SDL_BLENDMODE_NONE);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch);
    SDL_SetRenderTarget(renderer, oldRenderTarget); // restauramos el render target anterior
    SDL_DestroyTexture(renderTarget);
    return surface;
}

static SDL_Surface* makeGlowBorder(SDL_Surface* original, Color borderColor, int intensity, int borderSize)
{
    int w = original->w + borderSize * 2;
    int h = original->h + borderSize * 2;
    SDL_Surface* glowSurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    if(glowSurface == NULL)
    {
        printf("ERROR: %s\n", SDL_GetError());
        return NULL;
    }
    // Rellenamos con color transparente
    SDL_FillRect(glowSurface, NULL, SDL_MapRGBA(glowSurface->format, 0, 0, 0, 0));
    // Agregamos copias difuminadas alrededor de la imagen original
    SDL_Rect dst;
    dst.w = original->w;
    dst.h = original->h;
    // Múltiples offsets alrededor para simular el borde
    for(int dx = -borderSize; dx <= borderSize; dx++)
    {
        for(int dy = -borderSize; dy <= borderSize; dy++)
        {
            if(dx == 0 && dy == 0)
                continue; // evitamos el centro
            dst.x = dx + borderSize;
            dst.y = dy + borderSize;
            // Creamos copia temporal con color del brillo
            SDL_Surface* colored = SDL_ConvertSurface(original, original->format, 0);
            SDL_LockSurface(colored);
            Uint32* pixels = (Uint32*)colored->pixels;
            for(int i = 0; i < original->w * original->h; i++)
            {
                Uint8 r, g, b, a;
                SDL_GetRGBA(pixels[i], colored->format, &r, &g, &b, &a);
                // Si es transparente, lo ignoramos
                if (a == 0) {
                    pixels[i] = SDL_MapRGBA(colored->format, 0, 0, 0, 0);
                    continue;
                }

                // Aplica la intensidad al valor alpha
                int alpha = 255 * intensity / 100;  // "intensity" es un valor de 0 a 100
                // Aplicamos el color del brillo con la opacidad deseada
                pixels[i] = SDL_MapRGBA(colored->format, borderColor.r, borderColor.g, borderColor.b, alpha);
            }
            SDL_UnlockSurface(colored);
            // Dibujamos en la superficie glow
            SDL_BlitSurface(colored, NULL, glowSurface, &dst);
            SDL_FreeSurface(colored);
        }
    }
    return glowSurface;
}

static void applyColorOverlay(SDL_Surface* surface, Color colorOverlay)
{
    int pixelsCount = surface->w * surface->h;
    SDL_LockSurface(surface);
    Uint32* pixels = (Uint32*)surface->pixels;
    for(int i = 0; i < pixelsCount; i++)
    {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], surface->format, &r, &g, &b, &a);
        if(a == 0)
            // Si el pixel es transparente, lo ignoramos.
            continue;
        Uint8 finalAlpha = (Uint8)((a * colorOverlay.a) / 255); // Mezcla entre la opacidad del pixel original con el del nuevo color aplicado.
        pixels[i] = SDL_MapRGBA(surface->format, colorOverlay.r, colorOverlay.g, colorOverlay.b, finalAlpha);
    }
    SDL_UnlockSurface(surface);
}

static SDL_Surface* noise(Vector2 size)
{
    // Creamos la textura de ruido.
    SDL_Surface* noiseSurface = SDL_CreateRGBSurfaceWithFormat(0, size.x, size.y, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_LockSurface(noiseSurface);
    Uint32* pixels = (Uint32*)noiseSurface->pixels;
    size_t pixelsCount = size.x * size.y;
    for(size_t i = 0; i < pixelsCount; i++)
    {
        Uint8 intensity = (Uint8)SoulMath_randomNumber(0, 255);
        pixels[i] = SDL_MapRGBA(noiseSurface->format, intensity, intensity, intensity, 255);
    }
    SDL_UnlockSurface(noiseSurface);
    return noiseSurface;
}

static inline int clamp(int val, int min, int max) {
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}

// Genera un kernel gaussiano 1D
float* generateGaussianKernel(int radius, float sigma) {
    int size = 2 * radius + 1;
    float* kernel = (float*)malloc(size * sizeof(float));
    if(!kernel)
        return NULL;

    float sum = 0.0f;
    for(int i = -radius; i <= radius; i++)
    {
        float value = expf(-(i * i) / (2 * sigma * sigma));
        kernel[i + radius] = value;
        sum += value;
    } 
    // Normalizar
    for(int i = 0; i < size; i++)
        kernel[i] /= sum;

    return kernel;
}

// Blur gaussiano separable (X luego Y)
void applyBlur(SDL_Surface* surface, int radius, double sigma) {
    if(!surface || radius < 1 || sigma <= 0.0f)
        return;
    int w = surface->w;
    int h = surface->h;
    Uint32* src = (Uint32*)surface->pixels;
    Uint32* temp = (Uint32*)malloc(w * h * sizeof(Uint32));
    SDL_PixelFormat* fmt = surface->format;
    float* kernel = generateGaussianKernel(radius, sigma);
    if(!kernel)
    {
        free(temp);
        return;
    }
    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            float r = 0, g = 0, b = 0, a = 0;
            float weightSum = 0;
            for(int k = -radius; k <= radius; k++)
            {
                int nx = clamp(x + k, 0, w - 1);
                Uint32 pixel = src[y * w + nx];

                Uint8 pr, pg, pb, pa;
                SDL_GetRGBA(pixel, fmt, &pr, &pg, &pb, &pa);

                float weight = kernel[k + radius];
                r += pr * weight;
                g += pg * weight;
                b += pb * weight;
                a += pa * weight;
                weightSum += weight;
            }
            Uint8 fr = (Uint8)(r / weightSum);
            Uint8 fg = (Uint8)(g / weightSum);
            Uint8 fb = (Uint8)(b / weightSum);
            Uint8 fa = (Uint8)(a / weightSum);
            temp[y * w + x] = SDL_MapRGBA(fmt, fr, fg, fb, fa);
        }
    }
    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            float r = 0, g = 0, b = 0, a = 0;
            float weightSum = 0;
            for(int k = -radius; k <= radius; k++) {
                int ny = clamp(y + k, 0, h - 1);
                Uint32 pixel = temp[ny * w + x];

                Uint8 pr, pg, pb, pa;
                SDL_GetRGBA(pixel, fmt, &pr, &pg, &pb, &pa);

                float weight = kernel[k + radius];
                r += pr * weight;
                g += pg * weight;
                b += pb * weight;
                a += pa * weight;
                weightSum += weight;
            }

            Uint8 fr = (Uint8)(r / weightSum);
            Uint8 fg = (Uint8)(g / weightSum);
            Uint8 fb = (Uint8)(b / weightSum);
            Uint8 fa = (Uint8)(a / weightSum);

            src[y * w + x] = SDL_MapRGBA(fmt, fr, fg, fb, fa);
        }
    }

    free(kernel);
    free(temp);
}

const SurfaceEffectsClass SurfaceEffects = {
    .CreateSurfaceFromTexture = CreateSurfaceFromTexture,
    .makeGlowBorder = makeGlowBorder,
    .applyColorOverlay = applyColorOverlay,
    .noise = noise,
    .applyBlur = applyBlur
};
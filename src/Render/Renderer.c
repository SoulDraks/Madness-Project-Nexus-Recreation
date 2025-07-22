#include "Renderer.h"
#include "Math/SoulMath.h"
#include "Engine/Engine.h"

static SDL_Renderer* renderer = NULL;

static void callDraw(MadObject* node, SDL_Renderer* renderer)
{
    if(!node->visible)
        return;
    // Llamamos primero a "draw" del nodo actual.
    Vector2 final_size = {node->size.x * node->globalScale.x, node->size.y * node->globalScale.y};
    if(node->draw != NULL && SoulMath_isOnScreen(node->globalPos, final_size))
        node->draw(node, renderer);
    // Recorremos todos los hijos despues.
    MadObjectRef child = node->first_child;
    while(!isNullMadObjectRef(child))
    {
        callDraw(child.obj, renderer);
        child = child.obj->next_sibling;
    }
}

SDL_Renderer* GetRenderer()
{
    return renderer;
}

int initSDL()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "10");
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF;
    if (IMG_Init(imgFlags) & imgFlags != imgFlags)
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 0;
    }
    if (TTF_Init() == -1)
    {
        SDL_Log("Error initializing SDL_ttf: %s", TTF_GetError());
        SDL_Quit();
        return 0;
    }
    return 1;
}

int initRender(SDL_Window* window)
{
    if(renderer != NULL) return 1;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL)
    {
        return 0;
    }
    return 1;
}

void renderUI()
{
    // Limpiamos la pantalla.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // Dibujamos todos los objetos.
    MadObject* _root = Engine_getInstance()->root;
    MadObjectRef _rootUI = MadObject_getChildByIndex(_root, 0);
    callDraw(_rootUI.obj, renderer);
}

void renderDestroy()
{
    SDL_DestroyRenderer(renderer);
}
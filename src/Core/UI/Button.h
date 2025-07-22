#ifndef BUTTON_H
#define BUTTON_H

#include "Core/MadObject.h"

typedef void (*ButtonEvent)(void*);

typedef struct {
    MadObject base;
    MadObjectRef surface;
    ButtonEvent onPressed;
    ButtonEvent onReleased;
    ButtonEvent onHover;
    ButtonEvent onLeave;
    bool hasBeen_onPressed_Called;
    bool hasBeen_onReleased_Called;
    bool hasBeen_onHover_Called;
    bool disabled;
} Button;

extern Button* Button_new();
extern void Button_free(Button* self);
extern void Button_tick(void* self, double delta);
extern void Button_draw(void* self, SDL_Renderer* renderer);
extern void Button_register(lua_State* L);

#endif
#ifndef PANEL_H
#define PANEL_H
#include "Core/MadObject.h"

typedef struct {
   MadObject base;
   Color borderColor;
   int borderSize;
} Panel;

#define Panel_free(self) MadObject_free((MadObject*)self)

extern Panel* Panel_new();
extern void Panel_draw(void* self, SDL_Renderer* renderer);
extern void Panel_register(lua_State* L);

#endif
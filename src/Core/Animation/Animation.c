#include "AnimationManager.h"
#include "Engine/Engine.h"
#include <stdlib.h>
#include <string.h>

#define KEYFRAMES_INIT_SIZE 3
#define FUNCKEYFRAMES_INIT_SIZE 3

Animation* Animation_new()
{
    Animation* instance = (Animation*)malloc(sizeof(Animation));
    MADOBJECT_NEW((MadObject*)instance);
    instance->id = NO_ID;
    instance->loop = false;
    instance->fps = 30;
    instance->playing = false;
    instance->reverse = false;
    instance->processFrame = false;
    instance->processPendingKeyFrame = false;
    instance->currentFrame = 1;
    instance->elapsedFrames = 0;
    instance->elapsedTime = 0.0f;
    instance->frames = calloc(1, sizeof(Frame*));
    instance->framesSize = 1;
    instance->pendingKeyFrames = calloc(5, sizeof(PendingKeyFrame*));
    instance->pendingKeyFramesSize = 5;
    instance->pendingKeyFramesCount = 0;
    return instance;
}

void Animation_free(Animation* self)
{
    //printf("Se esta liberando una animacion.\n");
    MadObject_free((MadObject*)self);
    // Liberar el array de Frames.
    for(size_t i = 0; i < self->framesSize; i++)
    {
        if(self->frames[i] != NULL)
        {
            Frame* frame = self->frames[i];
            for(size_t j = 0; j < frame->keyFramesSize; j++)
            {
                struct KeyFrame* keyframe = &frame->keyFrames[j];
                if(keyframe == NULL)
                    break; // Salir del bucle porque el resto de KeyFrames seran nulos.
                Animation_releaseTargetRef(self->base.L, keyframe->target);
                free(keyframe->property);
                luaL_unref(self->base.L, LUA_REGISTRYINDEX, keyframe->value);
            }
            free(frame->keyFrames);
            for(size_t j = 0; j < frame->funcsKeyFramesSize; j++)
            {
                struct FuncKeyFrame* funcKeyFrame = &frame->funcsKeyFrames[j];
                if(funcKeyFrame->func == LUA_NOREF || funcKeyFrame->L == NULL)
                    break; // Salir del bucle porque el resto de FuncKeyFrames seran nulos.
                luaL_unref(funcKeyFrame->L, LUA_REGISTRYINDEX, funcKeyFrame->func);
            }
            free(frame->funcsKeyFrames);
            free(frame);
        }
    }
    free(self->frames);
    // Liberar el array de PendingKeyFrames.
    for(size_t i = 0; i < self->pendingKeyFramesSize; i++)
    {
        if(self->pendingKeyFrames[i] == NULL)
            break;
        free(self->pendingKeyFrames[i]);
    }
    free(self->pendingKeyFrames);
}

void Animation_setFrames(Animation* self, const int inFrames)
{
    if(inFrames > 0 && inFrames > self->framesSize)
    {
        size_t newSize = (size_t)inFrames;
        // Redimensionar el array.
        self->frames = Animation_resizeFrameArray(self->frames, self->framesSize, newSize);
        self->framesSize = newSize;
    }
}

void Animation_addKeyFrame(Animation* self, const int inFrame, struct KeyFrame* keyframe)
{
    // Restar uno al indice porque los indices de frames comienzan desde 1.
    int index = inFrame - 1;
    if(inFrame < 0)
    {
        printf("Error: 'inFrame' cannot be less than or equal to zero.\n");
        return;
    }
    if(inFrame > self->framesSize)
    {
        size_t newSize = (size_t)inFrame;
        self->frames = Animation_resizeFrameArray(self->frames, self->framesSize, newSize);
        self->framesSize = newSize;
    }
    if(self->frames[index] == NULL)
        self->frames[index] = Animation_CreateFrame();
    
    if(self->frames[index]->keyFramesCount >= self->frames[index]->keyFramesSize) // hay espacio?
    {
        // Redimensionar si no lo hay.
        size_t newSize = self->frames[index]->keyFramesSize * 2; // Duplicamos el espacio actual.
        self->frames[index]->keyFrames = Animation_resizeArray(self->frames[index]->keyFrames, newSize * sizeof(struct KeyFrame));
        struct KeyFrame* keyframes = self->frames[index]->keyFrames;
        for(size_t i = self->frames[index]->keyFramesSize; i < newSize; i++)
            keyframes[i] = NULL_KEYFRAME;
        self->frames[index]->keyFramesSize = newSize;
    }
    // Insertamos el KeyFrame al final del array finalmente.
    int newIndex = self->frames[index]->keyFramesCount++;
    self->frames[index]->keyFrames[newIndex] = *keyframe;
}

void Animation_addFunctionKeyFrame(Animation* self, const int inFrame, struct FuncKeyFrame func)
{
    int index = inFrame - 1;
    if(inFrame < 0 || inFrame > self->framesSize)
    {
        printf("Error: 'inFrame' cannot be less than or equal to zero.\n");
        return;
    }
    if(inFrame > self->framesSize)
    {
        size_t newSize = (size_t)inFrame;
        self->frames = Animation_resizeFrameArray(self->frames, self->framesSize, newSize);
        self->framesSize = newSize;
    }
    if(self->frames[index] == NULL)
        self->frames[index] = Animation_CreateFrame();
    if(self->frames[index]->funcsKeyFramesCount >= self->frames[index]->funcsKeyFramesSize)
    {
        size_t newSize = self->frames[index]->funcsKeyFramesSize * 2;
        self->frames[index]->funcsKeyFrames = Animation_resizeArray(self->frames[index]->funcsKeyFrames, newSize * sizeof(struct KeyFrame));
        struct FuncKeyFrame* funcKeyframes = self->frames[index]->funcsKeyFrames;
        for(size_t i = self->frames[index]->funcsKeyFramesSize; i < newSize; i++)
            funcKeyframes[i] = NULL_FUNCKEYFRAME;
        self->frames[index]->funcsKeyFramesSize = newSize;
    }
    int newIndex = self->frames[index]->funcsKeyFramesCount++;
    self->frames[index]->funcsKeyFrames[newIndex] = func;
}

void Animation_play(Animation* self)
{
    if(self->playing) // Si playing es true, significa que la animacion ya se encuentra en la lista.
        return;
    // Sino, pues la añadimos a la lista de animaciones.
    getWeakRefsTable(self->base.L);
    lua_rawgeti(self->base.L, -1, self->base.self_reference); // Obtener la referencia.
    lua_remove(self->base.L, -2); // Sacamos la tabla de referencias debiles.
    AnimationRef ref = {luaL_ref(self->base.L, LUA_REGISTRYINDEX), self, self->base.L};
    lua_pop(self->base.L, 1); // Sacar la tabla de referencias debiles de la pila.
    AnimationManager_addAnimation(ref);
    self->playing = true;
}

void Animation_playBackwards(Animation* self)
{
    Animation_play(self);
    self->reverse = true;
    self->currentFrame = self->framesSize;
}

void Animation_stop(Animation* self)
{
    if(!self->playing)
        return;
    AnimationManager_removeAnimation(self->id);
    self->playing = false;
    self->reverse = false;
}

void Animation_update(Animation* self, const float deltaTime)
{
    Engine* engine = Engine_getInstance();
    if(self->playing)
    {
        if(!self->processFrame && self->frames[self->currentFrame - 1] != NULL)
        {
            Frame* currentFrame = self->frames[self->currentFrame - 1];
            // Procesar KeyFrames.
            for(size_t i = 0; i < currentFrame->keyFramesCount; i++)
            {
                struct KeyFrame* keyframe = &currentFrame->keyFrames[i];
                luaObjectRef target = keyframe->target;
                luaObjectRef value = keyframe->value;
                lua_rawgeti(keyframe->L, LUA_REGISTRYINDEX, value);
                if(keyframe->interpolationType != NONE && is_interpolable_value(keyframe->L, -1))
                {
                    if(!keyframe->hasBeenProcess)
                    {
                        PendingKeyFrame* interpolatedKeyFrames = Animation_findClosestKeyFrame(self, self->currentFrame, keyframe);
                        if(interpolatedKeyFrames != NULL) // Se encontro un keyframe?
                        {
                            keyframe->hasBeenProcess = true;
                            // Añadir el PendingKeyFrame al final del array.
                            if(self->pendingKeyFramesCount >= self->pendingKeyFramesSize)
                            {
                                // Redimensionar el Array de PendingKeyFrames si no hay espacio.
                                size_t newSize = self->pendingKeyFramesSize * 2;
                                self->pendingKeyFrames = Animation_resizeArray(self->pendingKeyFrames, newSize * sizeof(PendingKeyFrame*));
                                for(size_t i = self->pendingKeyFramesSize; i < newSize; i++)
                                    self->pendingKeyFrames[i] = NULL;
                                self->pendingKeyFramesSize = newSize;
                            }
                            int index = self->pendingKeyFramesCount++;
                            self->pendingKeyFrames[index] = interpolatedKeyFrames;
                        }
                        else
                        {  
                            lua_pop(self->base.L, 1); // Sacar "value" de la pila.
                            Animation_setTargetField(self->base.L, target, keyframe->property, value);
                        }
                    }
                }
                else
                {
                    lua_pop(self->base.L, 1);
                    Animation_setTargetField(self->base.L, target, keyframe->property, value);
                }
            }
            // Procesar FuncKeyFrames.
            for(size_t i = 0; i < currentFrame->funcsKeyFramesCount; i++)
            {
                struct FuncKeyFrame funcKeyframe = currentFrame->funcsKeyFrames[i];
                if(funcKeyframe.func == LUA_NOREF || funcKeyframe.L == NULL)
                    break;
                int nret = callLuaFunction2(funcKeyframe.L, funcKeyframe.func, false, 0);
                lua_pop(funcKeyframe.L, nret); // Hacemos un lua_pop por seguridad poe si la funcion intenta retornarnos algo.
            }
            self->processFrame = true;
        }
        // Procesar e interpolar los PendingKeyFrame o los KeyFrames de Interpolacion.
        if(!self->processPendingKeyFrame)
        {
            for(size_t i = 0; i < self->pendingKeyFramesCount; i++)
            {
                PendingKeyFrame* pendingKeyFrame = self->pendingKeyFrames[i];
                if(self->currentFrame >= pendingKeyFrame->inFrame && self->currentFrame < pendingKeyFrame->finishFrame)
                {
                    double t = (double)(self->currentFrame - pendingKeyFrame->inFrame + 1) / (double)(pendingKeyFrame->duration);
                    InterpolableValue start_value = Animation_valueToInterpolableValue(self->base.L, pendingKeyFrame->prev_frame->value);
                    InterpolableValue end_value = Animation_valueToInterpolableValue(self->base.L, pendingKeyFrame->next_frame->value);
                    InterpolableValue final_value;
                    switch (pendingKeyFrame->interpolationType)
                    {
                        case LINEAR:
                            final_value = SoulMath_lerp(start_value, end_value, t);
                            break;
                        case QUADRATIC:
                            final_value = SoulMath_quadratic(start_value, end_value, t);
                            break;
                        case CUBIC:
                            final_value = SoulMath_cubic(start_value, end_value, t);
                            break;
                        case SINUSOIDAL:
                            final_value = SoulMath_sinusoidal(start_value, end_value, t);
                            break;
                        case EASEINOUT:
                            final_value = SoulMath_easeInOut(start_value, end_value, t);
                            break;
                        default:
                            break;
                    }
                    lua_rawgeti(self->base.L, LUA_REGISTRYINDEX, pendingKeyFrame->prev_frame->target); 
                    SoulMath_pushInterpolableValue(self->base.L, final_value);
                    lua_setfield(self->base.L, -2, pendingKeyFrame->prev_frame->property);
                    lua_pop(self->base.L, 1); // Sacar "target" de la pila.
                }
            }
            self->processPendingKeyFrame = true;
        }
        // Actualizar Frame.
        if(engine->fps == 0) // ¿no hay limite de fps?
        {
            self->elapsedTime += deltaTime; // Acumulamos el tiempo transcurrido.
            float frameTime = 1.0f / self->fps;  // El tiempo que debe durar un frame de la animación.
            if (self->elapsedTime >= frameTime) 
                Animation_advanceFrame(self);
        }
        else if(engine->fps != self->fps) 
        {
            int frame_skip = engine->fps / self->fps;
            self->elapsedFrames++;
            if (self->elapsedFrames >= frame_skip) 
                Animation_advanceFrame(self);
        }
        else
            Animation_advanceFrame(self);
        
        if(!self->reverse && self->currentFrame > self->framesSize)
        {
            if(self->loop)
                self->currentFrame = 1;
            else
            {
                self->currentFrame = 1;
                Animation_stop(self);
                callOnAnimationFinished(self);
            }
        }
        else if(self->reverse && self->currentFrame == 0)
        {
            if(self->loop)
                self->currentFrame = self->framesSize;
            else
            {
                self->currentFrame = 1;
                Animation_stop(self);
                callOnAnimationFinished(self);
            }
        }
    }
}

PendingKeyFrame* Animation_findClosestKeyFrame(Animation* self, const int inCurrentFrame, struct KeyFrame* inKeyFrame)
{
    // Le sumamos uno para no buscarlo en el mismo frame.
    for(size_t i = inCurrentFrame + 1; i <= self->framesSize; i++)
    {
        if(self->frames[i - 1] == NULL)
            continue;
        Frame* frame = self->frames[i - 1];
        for(size_t j = 0; j < frame->keyFramesCount; j++)
        {
            struct KeyFrame* keyframe = &frame->keyFrames[j];
            if(keyframe->target == inKeyFrame->target && strcmp(keyframe->property, inKeyFrame->property) == 0 && keyframe->interpolationType == inKeyFrame->interpolationType)
            {
                PendingKeyFrame* pendingKeyFrame = (PendingKeyFrame*)malloc(sizeof(PendingKeyFrame));
                pendingKeyFrame->prev_frame = inKeyFrame;
                pendingKeyFrame->next_frame = keyframe;
                pendingKeyFrame->inFrame = inCurrentFrame;
                pendingKeyFrame->finishFrame = i;
                pendingKeyFrame->duration = i - inCurrentFrame;
                pendingKeyFrame->interpolationType = inKeyFrame->interpolationType;
                return pendingKeyFrame;
            }
        }
    }
    return NULL;
}

void* Animation_resizeArray(void* Array, size_t newSize)
{
    void* ArrayResized = realloc(Array, newSize);
    
    if(ArrayResized == NULL)
    {
        printf("FATAL ERROR: Cannot resize an array. Requested size: %zu\n", newSize);
        exit(EXIT_FAILURE);
    }
    
    return ArrayResized;
}

Frame* Animation_CreateFrame()
{
    Frame* newFrame = (Frame*)malloc(sizeof(Frame));
    // Inicializar Array de KeyFrames.
    newFrame->keyFrames = calloc(KEYFRAMES_INIT_SIZE, sizeof(struct KeyFrame));
    for(size_t i = 0; i < KEYFRAMES_INIT_SIZE; i++)
        newFrame->keyFrames[i] = NULL_KEYFRAME;
    newFrame->keyFramesSize = KEYFRAMES_INIT_SIZE;
    newFrame->keyFramesCount = 0;
    // Inicializar Array de FuncKeyFrames.
    newFrame->funcsKeyFrames = calloc(FUNCKEYFRAMES_INIT_SIZE, sizeof(struct FuncKeyFrame));
    for(size_t i = 0; i < FUNCKEYFRAMES_INIT_SIZE; i++)
        newFrame->funcsKeyFrames[i] = NULL_FUNCKEYFRAME;
    newFrame->funcsKeyFramesSize = FUNCKEYFRAMES_INIT_SIZE;
    newFrame->funcsKeyFramesCount = 0;
    return newFrame;
}

Frame** Animation_resizeFrameArray(Frame** Array, size_t previousSize, size_t newSize)
{
    Frame** FrameArrayResized = realloc(Array, newSize * sizeof(Frame*));
    if(FrameArrayResized == NULL)
    {
        printf("FATAL ERROR: could not resize frame array.\n");
        exit(EXIT_FAILURE);
    }
    for(size_t i = previousSize; i < newSize; i++)
        FrameArrayResized[i] = NULL;
    return FrameArrayResized;
}

void Animation_setTargetField(lua_State* L, const luaObjectRef target, const char* property, const luaObjectRef value)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, target); // Ponemos al "target" en la pila
    lua_rawgeti(L, LUA_REGISTRYINDEX, value); // Ponemos el valor en la pila.
    lua_setfield(L, -2, property);
    // Sacamos el target de la pila.
    lua_pop(L, 1);
}

InterpolableValue Animation_valueToInterpolableValue(lua_State* L, luaObjectRef value)
{
    InterpolableValue converted_value;
    lua_rawgeti(L, LUA_REGISTRYINDEX, value);
    if(luaL_testudata(L, -1, "Vector2") != NULL)
    {
        converted_value.values.vec = *(Vector2*)lua_touserdata(L, -1);
        converted_value.type = VECTOR2;
    }
    if(luaL_testudata(L, -1, "Color") != NULL)
    {
        converted_value.values.color = *(Color*)lua_touserdata(L, -1);
        converted_value.type = COLOR;
    }
    else if(lua_isinteger(L, -1))
    {
        converted_value.values.integer = lua_tointeger(L, -1);
        converted_value.type = INT;
    }
    else if(lua_isnumber(L, -1))
    {
        converted_value.values.fl = lua_tonumber(L, -1);
        converted_value.type = FLOAT;
    }
    lua_pop(L, -1); // Sacar "value" de la pila.
    return converted_value;
}

void Animation_advanceFrame(Animation* self)
{
    if(!self->reverse)
        self->currentFrame++;
    else
        self->currentFrame--;
    self->elapsedFrames = 0;
    self->elapsedTime = 0.0f;
    self->processFrame = false;
    self->processPendingKeyFrame = false;
}

luaObjectRef Animation_getOrCreateTargetRef(lua_State *L, int index)
{
    // Asegurar que el índice es absoluto
    index = lua_absindex(L, index);
    luaL_getsubtable(L, LUA_REGISTRYINDEX, TARGET_TABLE_REFS);

    lua_pushvalue(L, index); // Copiar el 'target' a buscar
    lua_rawget(L, -2); // Buscar en la tabla de referencias de "Targets"

    if (lua_type(L, -1) == LUA_TNUMBER) {
        // Si ya tiene una referencia, retornarla
        int ref = (int)lua_tointeger(L, -1);
        lua_pop(L, 2);  // Sacar la referencia y la tabla
        return ref;
    }

    // Si no existe, crear una nueva referencia
    lua_pop(L, 1); // Sacar el resultado fallido
    lua_pushvalue(L, index); // Copiamos el valor nuevamente
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);

    // Guardar en la tabla de referencias
    lua_pushvalue(L, index);  // Copiar la clave (el valor mismo)
    lua_pushinteger(L, ref);  // Guardar la referencia
    lua_rawset(L, -3);        // Guardar en la tabla de referencias 

    lua_pop(L, 1); // Sacar la tabla de referencias de "Targets"
    return ref;
}

void Animation_releaseTargetRef(lua_State* L, luaObjectRef ref)
{
    luaL_getsubtable(L, LUA_REGISTRYINDEX, TARGET_TABLE_REFS);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

    if (!lua_isnil(L, -1)) {
        // Eliminar la entrada en la tabla
        lua_pushnil(L);
        lua_rawset(L, -3);
    }

    lua_pop(L, 1); // Sacar la tabla de referencias

    luaL_unref(L, LUA_REGISTRYINDEX, ref);
}

/* <------------ BINDINGS ------------> */

static int Animation_setFramesLua(lua_State* L)
{
    Animation** self = (Animation**)lua_checkinstance(L, 1, "Animation");
    int inFrames = luaL_checkinteger(L, 2);
    Animation_setFrames(*self, inFrames);
    return 0;
}

static int Animation_addKeyFrameLua(lua_State* L)
{
    Animation** self = (Animation**)lua_checkinstance(L, 1, "Animation");
    int inFrame = luaL_checkinteger(L, 2);
    if(inFrame < 0)
        return luaL_error(L, "Error: 'inFrame' cannot be less than or equal to zero.");

    if(!lua_isuserdata(L, 3) && !lua_istable(L, 3))
        return luaL_error(L, "expected table or userdata for the parameter 'target' but received %s.", luaL_typename(L, 3));

    const char* property = luaL_checkstring(L, 4);
    // el parametro "Value" puede ser cualquier tipo, asi que no es necesario comprobarlo.

    AnimationInterpolationType interpolationType = NONE;
    if(!lua_isnone(L, 6)) // se paso el tipo de interpolacion?
        interpolationType = luaL_checkinteger(L, 6);
    else if(is_interpolable_value(L, 5))
        interpolationType = LINEAR;

    lua_pushvalue(L, 3); // Poner a "target" en la cima para referenciarlo.
    struct KeyFrame keyframe;
    keyframe.target = Animation_getOrCreateTargetRef(L, -1);
    keyframe.property = strdup(property);
    lua_pushvalue(L, 5);
    keyframe.value = luaL_ref(L, LUA_REGISTRYINDEX);
    keyframe.interpolationType = interpolationType;
    keyframe.hasBeenProcess = false;
    keyframe.L = L;

    // Añadir el keyframe finalmente a la animacion.
    Animation_addKeyFrame(*self, inFrame, &keyframe);

    return 0;
}

static int Animation_addFunctionKeyFrameLua(lua_State* L)
{
    Animation** self = (Animation**)lua_checkinstance(L, 1, "Animation");
    int inFrame = luaL_checkinteger(L, 2);
    if(!lua_isfunction(L, 3))
        return luaL_error(L, "Expected function for parameter 'inFunction' but received %s", luaL_typename(L, 3));

    lua_pushvalue(L, 3);
    struct FuncKeyFrame funcKeyFrame = {luaL_ref(L, LUA_REGISTRYINDEX), L};
    Animation_addFunctionKeyFrame(*self, inFrame, funcKeyFrame);

    return 0;
}

static int Animation_playLua(lua_State* L)
{
    Animation** self = (Animation**)lua_checkinstance(L, 1, "Animation");
    Animation_play(*self);
    return 0;
}

static int Animation_playBackwardsLua(lua_State* L)
{
    Animation** self = (Animation**)lua_checkinstance(L, 1, "Animation");
    Animation_playBackwards(*self);
    return 0;
}

static int Animation_stopLua(lua_State* L)
{
    Animation** self = (Animation**)lua_checkinstance(L, 1, "Animation");
    Animation_stop(*self);
    return 0;
}

void Animation_register(lua_State* L)
{
    /*
    luaL_newmetatable(L, "Animation");

    lua_pushcfunction(L, Animation_indexLua);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, Animation_newIndexLua);
    lua_setfield(L, -2, "__newindex");

    lua_pushcfunction(L, Animation_gc);
    lua_setfield(L, -2, "__gc");

    lua_pushcfunction(L, Animation_setFramesLua);
    lua_setfield(L, -2, "setFrames");

    lua_pushcfunction(L, Animation_addKeyFrameLua);
    lua_setfield(L, -2, "addKeyFrame");

    lua_pushcfunction(L, Animation_addFunctionKeyFrameLua);
    lua_setfield(L, -2, "addFunctionKeyFrame");

    lua_pushcfunction(L, Animation_playLua);
    lua_setfield(L, -2, "play");

    lua_pushcfunction(L, Animation_playBackwardsLua);
    lua_setfield(L, -2, "playBackwards");

    lua_pushcfunction(L, Animation_stopLua);
    lua_setfield(L, -2, "stop");

    lua_pushcfunction(L, Animation_newLua);
    lua_setfield(L, -2, "new");

    luaL_getmetatable(L, "MadObject");
    lua_setfield(L, -2, "__extends");

    lua_setglobal(L, "Animation");
    */
    Lua_registerclass("Animation",
        CONSTRUCTOR, Animation_new,
        DESTRUCTOR, Animation_free,
        EXTENDS, "MadObject",
        FIELD, "loop", TBOOL, offsetof(Animation, loop),
        FIELD, "fps", TINT, offsetof(Animation, fps),
        FIELDREADONLY, "playing", TBOOL, offsetof(Animation, playing),
        FIELDREADONLY, "currentFrame", TINT, offsetof(Animation, currentFrame),
        FUNC, "setFrames", Animation_setFramesLua,
        FUNC, "addKeyFrame", Animation_addKeyFrameLua,
        FUNC, "addFunctionKeyFrame", Animation_addFunctionKeyFrameLua,
        FUNC, "play", Animation_playLua,
        FUNC, "playBackwards", Animation_playBackwardsLua,
        FUNC, "stop", Animation_stopLua,
        END
    );
    Lua_registerenum("AnimationInterpolationType", 6,
        "NONE", NONE,
        "LINEAR", LINEAR,
        "QUADRATIC", QUADRATIC,
        "CUBIC", CUBIC,
        "SINUSOIDAL", SINUSOIDAL,
        "EASEINOUT", EASEINOUT
    );
}
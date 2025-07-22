#ifndef ANIMATION_H
#define ANIMATION_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "Core/MadObject.h"
#include "Lua/LuaManager.h"
#include "Math/SoulMath.h"

typedef enum {
    NONE,
    LINEAR,
    QUADRATIC,
    CUBIC,
    SINUSOIDAL,
    EASEINOUT
} AnimationInterpolationType;

struct KeyFrame {
    luaObjectRef target; // Referencia al objeto al que sera animado.
    char* property; // Propiedad a cambiar del objeto.
    luaObjectRef value; // Valor final.
    lua_State* L;
    AnimationInterpolationType interpolationType;
    bool hasBeenProcess; // Bandera para saber si ya fue encontrado su keyframe mas cercano.
};

struct FuncKeyFrame {
    luaFuncRef func;
    lua_State* L;
};

typedef struct {
    struct KeyFrame* keyFrames; // Array de keyframes.
    size_t keyFramesSize; // Tamaño del array keyframes.
    int keyFramesCount; // Cantidad de KeyFrames.
    struct FuncKeyFrame* funcsKeyFrames; // Array de funciones KeyFrames.
    size_t funcsKeyFramesSize;
    int funcsKeyFramesCount;
} Frame;

typedef struct {
    struct KeyFrame* prev_frame; // KeyFrame de inicio.
    struct KeyFrame* next_frame; // KeyFrame final.
    int inFrame; // Indica en que frame comienza la interpolacion.
    int finishFrame; // Indica en que frame termina la interpolacion.
    int duration; // Duracion en frames de la interpolacion
    AnimationInterpolationType interpolationType;
} PendingKeyFrame;

typedef struct {
    MadObject base;
    int id; // id unico de la animacion para poder encontrarlo en la lista.
    bool loop;
    int fps; // Define a que fps ira la animacion.
    bool playing; // Bandera para saber si la animacion ya se encuentra en reproduccion.
    bool reverse; // Bandera que indica si la animacion debe reproducirse en reversa.
    bool processFrame; // Bandera para saber si ya fue procesado el frame actual.
    bool processPendingKeyFrame; 
    int currentFrame; // Define en que frames nos encontramos.
    int elapsedFrames;
    float elapsedTime; // Atributo para medir el tiempo transcurrido si no hay limite de frames,
    Frame** frames; // Array de frames.
    size_t framesSize;
    PendingKeyFrame** pendingKeyFrames; // Array de PendingKeyFrames.
    size_t pendingKeyFramesSize;
    int pendingKeyFramesCount;
} Animation;

#define NULL_KEYFRAME (struct KeyFrame){LUA_NOREF, NULL, LUA_NOREF, NULL, NONE, false}
#define NULL_FUNCKEYFRAME (struct FuncKeyFrame){LUA_NOREF, NULL}
#define is_null_keyframe(keyframe) ((keyframe).target == LUA_NOREF || (keyframe).property == NULL || (keyframe).value == LUA_NOREF || (keyframe).L == NULL)
#define is_null_funckeyframe(funckeyframe) ((funckeyframe).func == LUA_NOREF || (funckeyframe).L == NULL)
#define TARGET_TABLE_REFS "targetTableRef"
#define NO_ID -1
// Verifica si el valor en el indice indicado en la pila de lua es referenciable.
#define lua_isreferencable(L, i) (lua_istable(L, (i)) || lua_isuserdata(L, (i)) || lua_isfunction(L, (i)))
#define callOnAnimationFinished(self) callLuaFunction3((self)->base.L, (self)->base.self_reference, true, "onAnimationFinished", 0)

extern Animation* Animation_new();
extern void Animation_free(Animation* self);
// Establece la cantidad de frames que tendra la animacion 
// NOTA: no puedes ponerle menos frames de los que tiene actualmente.
extern void Animation_setFrames(Animation* self, const int inFrames);
extern void Animation_addKeyFrame(Animation* self, const int inFrame, struct KeyFrame* keyframe);
extern void Animation_addFunctionKeyFrame(Animation* self, const int inFrame, struct FuncKeyFrame func);
extern void Animation_play(Animation* self);
extern void Animation_playBackwards(Animation* self);
extern void Animation_stop(Animation* self);
// Metodo que es llamado cada frame para procesar la animacion.
extern void Animation_update(Animation* self, const float deltaTime);
extern void Animation_register(lua_State* L);

/* <-------- Parte privada --------> */

// Busca el keyframe mas cercano al dado y devuelve una instancia de "PendingKeyFrame" con todos los datos
// incluidos para la interpolacion.
extern PendingKeyFrame* Animation_findClosestKeyFrame(Animation* self, const int inCurrentFrame, struct KeyFrame* inKeyFrame);
extern void* Animation_resizeArray(void* Array, size_t newSize);
extern Frame* Animation_CreateFrame();
extern Frame** Animation_resizeFrameArray(Frame** Array, size_t previousSize, size_t newSize);
extern void Animation_setTargetField(lua_State* L, const luaObjectRef target, const char* property, const luaObjectRef value);
// Transforma la referencia a cualquier valor a un "InterpolableValue" si es posible.
extern InterpolableValue Animation_valueToInterpolableValue(lua_State* L, luaObjectRef value);
extern void Animation_advanceFrame(Animation* self);
/* Obtiene el indice de referencia del "target" si ya hay una referencia a el o 
   Lo crea si no existe.
*/
extern luaObjectRef Animation_getOrCreateTargetRef(lua_State *L, int index);
// Libera una referencia al target incluso si es nula o invalida.
extern void Animation_releaseTargetRef(lua_State* L, luaObjectRef ref);
 
#endif
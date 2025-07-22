#include "AnimationManager.h"
#include <stdlib.h>

#define ANIMATION_INIT_SIZE 15

static AnimationManager animationmanager = {NULL, 0};
static int animationIDCounter = 0;

static void freeAllAnimationQueued()
{
    if (animationmanager.head == NULL)
        return;
    AnimationNode* current = animationmanager.head;
    AnimationNode* prev = NULL;
    while (current != NULL)
    {
        AnimationNode* node_next = current->next;
        if (current->queued)
        {
            luaL_unref(current->animationRef.L, LUA_REGISTRYINDEX, current->animationRef.refLua);
            if (prev == NULL)
                animationmanager.head = node_next;
            else
                prev->next = node_next; // Saltamos el nodo eliminado
            free(current);
            animationmanager.animationsCount--;
        }
        else
            prev = current;
        current = node_next;
    }
}

void AnimationManager_addAnimation(AnimationRef ref)
{
    AnimationNode* newNode = (AnimationNode*)malloc(sizeof(AnimationNode));
    ref.animation->id = animationIDCounter++; // Darle un ID unico.
    newNode->animationRef = ref;
    newNode->next = NULL;
    newNode->queued = false;
    // Si la lista esta vacia, el nuevo nodo sera la cabeza.
    if(animationmanager.head == NULL)
        animationmanager.head = newNode;
    else
    {
        // Si la lista no está vacía, recorrerla hasta el final y agregar el nodo.
        AnimationNode* current = animationmanager.head;
        while(current->next != NULL)
            current = current->next;
        current->next = newNode;
    }
    animationmanager.animationsCount++;
}

// a -> b -> c -> d

void AnimationManager_removeAnimation(const int anim_id)
{
    if(animationmanager.head == NULL)
        return;
    for(AnimationNode* current = animationmanager.head; current != NULL; current = current->next)
    {
        if (current->animationRef.animation->id == anim_id)
        {
            current->queued = true; // Lo marcamos como en cola para eliminacion.
            animationmanager.animationsCount--;
            return; // Salimos después de eliminar.
        }
    }
}

void AnimationManager_process(const float delta)
{
    for(AnimationNode* current = animationmanager.head; current != NULL; current = current->next)
    {
        Animation_update(current->animationRef.animation, delta);
    }
    freeAllAnimationQueued();
}
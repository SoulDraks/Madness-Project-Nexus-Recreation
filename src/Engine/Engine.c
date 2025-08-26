#include "Engine.h"
#include "Input/Mouse.h"
#include "Input/KeyManager.h"
#include "Core/Animation/AnimationManager.h"
#include "Render/Renderer.h"
#include "Render/Window.h"

typedef struct {
    MadObject* obj;
    bool freeAllChilds;
} MadObjectQueued;

static Engine engine = {0.0f, 30, NULL};
static struct {
    MadObjectQueued data[QUEUE_SIZE];
    size_t size, count;
} objQueue;

// Funcion auxiliar que llama a todos los "tick" del arbol
static void callTick(MadObject* node, double deltaTime)
{
    MadObject_callTick(node, deltaTime);

    MadObjectRef child = node->first_child;
    while(!isNullMadObjectRef(child))
    {
        callTick(child.obj, deltaTime);
        child = child.obj->next_sibling;
    }
}

// Libera recursivamente todos los hijos de un MadObject
static void freeChildrenRecursive(lua_State* L, MadObject* self)
{
    MadObjectRef child = self->first_child;
    while(!isNullMadObjectRef(child))
    {
        MadObject* childObj = child.obj;
        MadObjectRef next = childObj->next_sibling;
        lua_rawgeti(L, LUA_REGISTRYINDEX, child.ref);
        // Liberamos recursivamente los hijos de este hijo
        freeChildrenRecursive(L, childObj);
        // Llamar al __gc del hijo ("destructor dinámico")
        callLuagc(L, -1);
        luaL_getmetatable(L, "freedObject");
        lua_setmetatable(L, -2);
        lua_pop(L, 1); // limpiar child_obj del stack
        child = next;
    }
}

// Libera todos los objetos de la cola y la limpia.
static void freeQueuedObjects()
{
    for(size_t i = 0; i < objQueue.count; i++)
    {
        MadObject* obj = objQueue.data[i].obj;
        lua_State* L = obj->L;
        if(objQueue.data[i].freeAllChilds)
            freeChildrenRecursive(L, obj);
        
        getWeakRefsTable(L);
        lua_rawgeti(L, -1, obj->self_reference);
        callLuagc(L, -1);
        luaL_getmetatable(L, "freedObject");
        lua_setmetatable(L, -2);
        // Sacamos la tabla de referencias debiles y el MadObject de la pila.
        lua_pop(L, 2);
        // Limpiamos el actual.
        objQueue.data[i] = (MadObjectQueued){NULL, false};
    }
    // Indicamos que el la cola ahora esta vacia.
    objQueue.count = 0;
}

Engine* Engine_getInstance()
{
    return &engine;
}

void Engine_Init()
{
    lua_State* L = getLuaState();
    // Inicializamos la lista, array o cola de objetos que deben ser liberados.
    objQueue.size = QUEUE_SIZE;
    objQueue.count = 0;
    for(size_t i = 0; i < objQueue.size; i++)
        objQueue.data[i] = (MadObjectQueued){NULL, false};

    // Creamos el nodo raiz.
    MadObject* _root = MadObject_new();
    String_free(_root->name);
    _root->name = String_new("_root");
    lua_pushMadObject(L, _root);
    // Creamos el nodo raiz para los objetos en el UI.
    MadObject* _rootUI = MadObject_new();
    String_free(_rootUI->name);
    _rootUI->name = String_new("_rootUI");
    lua_pushMadObject(L, _rootUI);
    // Ponemos en la cima de la pila a "_rootUI" para referenciarlo y finalmente añadirlo como hijo.
    lua_pushvalue(L, -1);
    MadObjectRef _rootUIRef = lua_refMadObject(L, LUA_REGISTRYINDEX);
    MadObject_addChild(_root, _rootUIRef);

    // Creamos el nodo raiz para los objetos en el World.
    MadObject* _rootWorld = MadObject_new();
    String_free(_rootWorld->name);
    _rootWorld->name = String_new("_rootWorld");
    lua_pushMadObject(L, _rootWorld);
    // Lo ponemos en la cima, lo referenciamos y lo añadimos como hijo.
    lua_pushvalue(L, -1);
    MadObjectRef _rootWorldRef = lua_refMadObject(L, LUA_REGISTRYINDEX);
    MadObject_addChild(_root, _rootWorldRef);
    /**
     *  Guardamos a "_rootUI" y a "_rootWorld" como atributos dinamicos del nodo raiz.
     *  Esto evitara que en Lua se tenga que hacer a cada rato:
     *  local _rootUI = _root:getChild("_rootUI")
     *  O que manualmente se tenga que declarar un atributo dinamico con ese nombre en el nodo raiz.
     *  Al asignamos como atributo dinamico, se hace de manera mas directa:
     *  _root.UI:addChild(hijito)
     */
    lua_setfield(L, -3, "World");
    lua_setfield(L, -2, "UI");

    // Finalmente, declaramos una variable global dentro de lua donde se guarde el nodo raiz.
    lua_setglobal(L, "_root");

    engine.root = _root;
}

void Engine_Loop()
{
    bool running = true;
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 lastTime = 0;
    while(running)
    {
        // Calcular el deltaTime.
        lastTime = now;
        now = SDL_GetPerformanceCounter();
        Uint64 freq = SDL_GetPerformanceFrequency();
        engine.deltaTime = (double)(now - lastTime) / (double)freq;

        // Procesar el resto de modulos.
        running = Window_process();
        Mouse_Update();
        AnimationManager_process(engine.deltaTime);
        // LLamar al "tick" de todos los nodos del arbol de nodos de MadObjects.
        callTick(engine.root, engine.deltaTime);
        // Renderizar la pantalla.
        renderUI();
        // Mostrar finalmente por pantalla lo dibujado.
        SDL_RenderPresent(GetRenderer());
        // Limpiamos todos los objetos de la cola finalmente.
        if(objQueue.count > 0)
            freeQueuedObjects();
        
        if(engine.fps != 0) // Hay limite de fps?
        {
            double targetFrameTime = 1.0 / (double)engine.fps;
            Uint64 frameEnd = SDL_GetPerformanceCounter();
            double frameElapsed = (double)(frameEnd - now) / (double)freq;
            if(frameElapsed < targetFrameTime)
                // Convertimos la diferencia a milisegundos y dormimos ese tiempo
                SDL_Delay((Uint32)((targetFrameTime - frameElapsed) * 1000.0));
        }
    }
}

void Engine_addObjectQueue(MadObject* obj, const bool freeAllChilds)
{
    if(objQueue.count > objQueue.size)
    {
        printf("FATAL ERROR: Queue overflow.\n");
        exit(EXIT_FAILURE);
    }
    int index = objQueue.count++;
    objQueue.data[index] = (MadObjectQueued){obj, freeAllChilds};
}

/* <------------ BINDINGS ------------> */

static int Engine_indexLua(lua_State* L)
{
    const char* key = luaL_checkstring(L, 2);
    if(strcmp(key, "fps") == 0)
        lua_pushinteger(L, (lua_Integer)engine.fps);
    else if(strcmp(key, "deltaTime") == 0)
        lua_pushnumber(L, (lua_Number)engine.deltaTime);
    else if(strcmp(key, "root") == 0)
    {
        getWeakRefsTable(L);
        lua_rawgeti(L, -1, engine.root->self_reference);
        lua_remove(L, -2);
    }
    else
        lua_rawget(L, 1);
    
    return 1;
}

static int Engine_newindexLua(lua_State* L)
{
    const char* key = luaL_checkstring(L, 2);
    if(strcmp(key, "fps") == 0)
        engine.fps = (unsigned int)luaL_checkinteger(L, 3);
    else
        lua_rawset(L, 1);
    
    return 0;
}

void Engine_register(lua_State* L)
{
    luaL_newmetatable(L, "Engine");

    lua_pushcfunction(L, Engine_indexLua);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, Engine_newindexLua);
    lua_setfield(L, -2, "__newindex");

    // Hacemos que el mismo sea su propia metatabla.
    lua_pushvalue(L, -1);
    lua_setmetatable(L, -2);

    lua_setglobal(L, "Engine");
}
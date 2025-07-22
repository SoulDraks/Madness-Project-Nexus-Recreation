#include "KeyManager.h"
#include <string.h>
#define EVENT_LISTENERS_INIT_SIZE 5

static KeyManager keymanager;

void KeyManager_Init(lua_State* L)
{
    keymanager.keyCode = -10;
    keymanager.isDown = false;
    keymanager.isUp = false;
    // Creamos los array de los "Event Listeners".
    keymanager.keyDownListeners = malloc(EVENT_LISTENERS_INIT_SIZE * sizeof(struct eventListener));
    keymanager.keyUpListeners = malloc(EVENT_LISTENERS_INIT_SIZE * sizeof(struct eventListener));
    if(keymanager.keyDownListeners == NULL || keymanager.keyUpListeners == NULL) // fallo?
    {
        printf("FATAL ERROR: Failed to create event Listeners array.\n");
        exit(EXIT_FAILURE);
    }
    // Rellenamos los arrays
    for(size_t i = 0; i < EVENT_LISTENERS_INIT_SIZE; i++)
    {
        keymanager.keyDownListeners[i].func = LUA_NOREF;
        keymanager.keyUpListeners[i].func = LUA_NOREF;
        keymanager.keyDownListeners[i].L = NULL;
        keymanager.keyUpListeners[i].L = NULL;
    }
    keymanager.keyDownListeners_size = EVENT_LISTENERS_INIT_SIZE;
    keymanager.keyUpListeners_size = EVENT_LISTENERS_INIT_SIZE;
    keymanager.keyDownListenersCount = 0;
    keymanager.keyUpListenersCount = 0;

    // Registrar "KeyManager" y sus metodos a lua.
    lua_newtable(L);

    lua_pushcfunction(L, KeyManager_isKeyPressedLua);
    lua_setfield(L, -2, "isKeyPressed");

    lua_pushcfunction(L, KeyManager_isKeyReleasedLua);
    lua_setfield(L, -2, "isKeyReleased");

    lua_pushcfunction(L, KeyManager_addEventListenerLua);
    lua_setfield(L, -2, "addEventListener");

    lua_pushcfunction(L, KeyManager_removeEventListenerLua);
    lua_setfield(L, -2, "removeEventListener");

    lua_setglobal(L, "KeyManager");

    // Crear la tabla "Key" que contenga todos los KeyCodes de las teclas.
    lua_newtable(L);
    lua_pushinteger(L, SDLK_a); lua_setfield(L, -2, "A");
    lua_pushinteger(L, SDLK_b); lua_setfield(L, -2, "B");
    lua_pushinteger(L, SDLK_c); lua_setfield(L, -2, "C");
    lua_pushinteger(L, SDLK_d); lua_setfield(L, -2, "D");
    lua_pushinteger(L, SDLK_e); lua_setfield(L, -2, "E");
    lua_pushinteger(L, SDLK_f); lua_setfield(L, -2, "F");
    lua_pushinteger(L, SDLK_g); lua_setfield(L, -2, "G");
    lua_pushinteger(L, SDLK_h); lua_setfield(L, -2, "H");
    lua_pushinteger(L, SDLK_i); lua_setfield(L, -2, "I");
    lua_pushinteger(L, SDLK_j); lua_setfield(L, -2, "J");
    lua_pushinteger(L, SDLK_k); lua_setfield(L, -2, "K");
    lua_pushinteger(L, SDLK_l); lua_setfield(L, -2, "L");
    lua_pushinteger(L, SDLK_m); lua_setfield(L, -2, "M");
    lua_pushinteger(L, SDLK_n); lua_setfield(L, -2, "N");
    lua_pushinteger(L, SDLK_o); lua_setfield(L, -2, "O");
    lua_pushinteger(L, SDLK_p); lua_setfield(L, -2, "P");
    lua_pushinteger(L, SDLK_q); lua_setfield(L, -2, "Q");
    lua_pushinteger(L, SDLK_r); lua_setfield(L, -2, "R");
    lua_pushinteger(L, SDLK_s); lua_setfield(L, -2, "S");
    lua_pushinteger(L, SDLK_t); lua_setfield(L, -2, "T");
    lua_pushinteger(L, SDLK_u); lua_setfield(L, -2, "U");
    lua_pushinteger(L, SDLK_v); lua_setfield(L, -2, "V");
    lua_pushinteger(L, SDLK_w); lua_setfield(L, -2, "W");
    lua_pushinteger(L, SDLK_x); lua_setfield(L, -2, "X");
    lua_pushinteger(L, SDLK_y); lua_setfield(L, -2, "Y");
    lua_pushinteger(L, SDLK_z); lua_setfield(L, -2, "Z");
    lua_pushinteger(L, SDLK_1); lua_setfield(L, -2, "1");
    lua_pushinteger(L, SDLK_2); lua_setfield(L, -2, "2");
    lua_pushinteger(L, SDLK_3); lua_setfield(L, -2, "3");
    lua_pushinteger(L, SDLK_4); lua_setfield(L, -2, "4");
    lua_pushinteger(L, SDLK_5); lua_setfield(L, -2, "5");
    lua_pushinteger(L, SDLK_6); lua_setfield(L, -2, "6");
    lua_pushinteger(L, SDLK_7); lua_setfield(L, -2, "7");
    lua_pushinteger(L, SDLK_8); lua_setfield(L, -2, "8");
    lua_pushinteger(L, SDLK_9); lua_setfield(L, -2, "9");
    lua_pushinteger(L, SDLK_0); lua_setfield(L, -2, "0");
    lua_pushinteger(L, SDLK_RETURN); lua_setfield(L, -2, "RETURN");
    lua_pushinteger(L, SDLK_ESCAPE); lua_setfield(L, -2, "ESCAPE");
    lua_pushinteger(L, SDLK_BACKSPACE); lua_setfield(L, -2, "BACKSPACE");
    lua_pushinteger(L, SDLK_TAB); lua_setfield(L, -2, "TAB");
    lua_pushinteger(L, SDLK_SPACE); lua_setfield(L, -2, "SPACE");
    lua_pushinteger(L, SDLK_MINUS); lua_setfield(L, -2, "MINUS");
    lua_pushinteger(L, SDLK_EQUALS); lua_setfield(L, -2, "EQUALS");
    lua_pushinteger(L, SDLK_LEFTBRACKET); lua_setfield(L, -2, "LEFTBRACKET");
    lua_pushinteger(L, SDLK_RIGHTBRACKET); lua_setfield(L, -2, "RIGHTBRACKET");
    lua_pushinteger(L, SDLK_SEMICOLON); lua_setfield(L, -2, "SEMICOLON");
    lua_pushinteger(L, SDLK_QUOTE); lua_setfield(L, -2, "QUOTE");
    lua_pushinteger(L, SDLK_COMMA); lua_setfield(L, -2, "COMMA");
    lua_pushinteger(L, SDLK_PERIOD); lua_setfield(L, -2, "PERIOD");
    lua_pushinteger(L, SDLK_SLASH); lua_setfield(L, -2, "SLASH");
    lua_pushinteger(L, SDLK_CAPSLOCK); lua_setfield(L, -2, "CAPSLOCK");
    lua_pushinteger(L, SDLK_F1); lua_setfield(L, -2, "F1");
    lua_pushinteger(L, SDLK_F2); lua_setfield(L, -2, "F2");
    lua_pushinteger(L, SDLK_F3); lua_setfield(L, -2, "F3");
    lua_pushinteger(L, SDLK_F4); lua_setfield(L, -2, "F4");
    lua_pushinteger(L, SDLK_F5); lua_setfield(L, -2, "F5");
    lua_pushinteger(L, SDLK_F6); lua_setfield(L, -2, "F6");
    lua_pushinteger(L, SDLK_F7); lua_setfield(L, -2, "F7");
    lua_pushinteger(L, SDLK_F8); lua_setfield(L, -2, "F8");
    lua_pushinteger(L, SDLK_F9); lua_setfield(L, -2, "F9");
    lua_pushinteger(L, SDLK_F10); lua_setfield(L, -2, "F10");
    lua_pushinteger(L, SDLK_F11); lua_setfield(L, -2, "F11");
    lua_pushinteger(L, SDLK_F12); lua_setfield(L, -2, "F12");
    lua_pushinteger(L, SDLK_PRINTSCREEN); lua_setfield(L, -2, "PRINTSCREEN");
    lua_pushinteger(L, SDLK_SCROLLLOCK); lua_setfield(L, -2, "SCROLLLOCK");
    lua_pushinteger(L, SDLK_PAUSE); lua_setfield(L, -2, "PAUSE");
    lua_pushinteger(L, SDLK_INSERT); lua_setfield(L, -2, "INSERT");
    lua_pushinteger(L, SDLK_HOME); lua_setfield(L, -2, "HOME");
    lua_pushinteger(L, SDLK_PAGEDOWN); lua_setfield(L, -2, "PAGEDOWN");
    lua_pushinteger(L, SDLK_RIGHT); lua_setfield(L, -2, "RIGHT");
    lua_pushinteger(L, SDLK_LEFT); lua_setfield(L, -2, "LEFT");
    lua_pushinteger(L, SDLK_DOWN); lua_setfield(L, -2, "DOWN");
    lua_pushinteger(L, SDLK_UP); lua_setfield(L, -2, "UP");
    lua_setglobal(L, "Key");
}

void KeyManager_Update(const int keyCode, const bool isPressed, const bool isReleased)
{
    keymanager.keyCode = keyCode;
    keymanager.isDown = isPressed;
    keymanager.isUp = isReleased;
    if(isPressed)
        KeyManager_callKeyDownListeners();
    if(isReleased)
        KeyManager_callKeyUpListeners();
}

int KeyManager_getKeyCode()
{
    return keymanager.keyCode;
}

void KeyManager_callKeyDownListeners()
{
    for(size_t i = 0; i < keymanager.keyDownListeners_size; i++)
    {
        struct eventListener* listener = &keymanager.keyDownListeners[i];
        if(listener->func == LUA_NOREF || listener->L == NULL)
            break; // Salir del bucle porque ya no habra listener validos.
        callLuaFunction2(listener->L, listener->func, false, 1,
            LUA_TNUMBER, (double)keymanager.keyCode
        );
    }
}

void KeyManager_callKeyUpListeners()
{
    for(size_t i = 0; i < keymanager.keyUpListeners_size; i++)
    {
        struct eventListener* listener = &keymanager.keyUpListeners[i];
        if(listener->func == LUA_NOREF || listener->L == NULL)
            break;
        callLuaFunction2(listener->L, listener->func, false, 1,
            LUA_TNUMBER, (double)keymanager.keyCode
        );
    }
}

int KeyManager_isKeyPressedLua(lua_State* L)
{
    int keyCode = luaL_checkinteger(L, 1);
    if(keymanager.keyCode == keyCode && keymanager.isDown)
        lua_pushboolean(L, true);
    else
        lua_pushboolean(L, false);
    return 1;
}

int KeyManager_isKeyReleasedLua(lua_State* L)
{
    int keyCode = luaL_checkinteger(L, 1);
    if(keymanager.keyCode == keyCode && keymanager.isUp)
        lua_pushboolean(L, true);
    else
        lua_pushboolean(L, false);
    return 1;
}

int KeyManager_addEventListenerLua(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);
    const char* inMode = luaL_checkstring(L, 2);
    if(strcmp(inMode, "DOWN") == 0)
    {
        if(keymanager.keyDownListenersCount >= keymanager.keyDownListeners_size) // hay espacio? 
        {
            // Redimensionar si no lo hay.
            size_t new_size = keymanager.keyDownListeners_size * 2;
            keymanager.keyDownListeners = realloc(keymanager.keyDownListeners, new_size * sizeof(struct eventListener));
            // Rellenar los espacios vacios.
            for(int i = keymanager.keyDownListeners_size; i < new_size; i++)
            {
                keymanager.keyDownListeners[i].L = NULL;
                keymanager.keyDownListeners[i].func = LUA_NOREF;
            }
            keymanager.keyDownListeners_size = new_size;
        }
        lua_pushvalue(L, 1); // Poner a la funcion en la cima de la pila.
        // insertar la referencia a la funcion y la referencia al estado de lua al final del array.
        int index = keymanager.keyDownListenersCount++;
        keymanager.keyDownListeners[index].func = luaL_ref(L, LUA_REGISTRYINDEX);
        keymanager.keyDownListeners[index].L = L;
    }
    else if(strcmp(inMode, "UP") == 0)
    {
        if(keymanager.keyUpListenersCount >= keymanager.keyDownListeners_size)
        {
            size_t new_size = keymanager.keyUpListeners_size * 2;
            keymanager.keyUpListeners = realloc(keymanager.keyUpListeners, new_size * sizeof(struct eventListener));
            for(int i = keymanager.keyUpListeners_size; i < new_size; i++)
            {
                keymanager.keyUpListeners[i].L = NULL;
                keymanager.keyUpListeners[i].func = LUA_NOREF;
            }
            keymanager.keyUpListeners_size = new_size;
        }
        lua_pushvalue(L, 1);
        int index = keymanager.keyUpListenersCount++;
        keymanager.keyUpListeners[index].func = luaL_ref(L, LUA_REGISTRYINDEX);
        keymanager.keyUpListeners[index].L = L;
    }
    else
        return luaL_error(L, "the parameter 'inMode' can only be 'UP' or 'DOWN' but was received: %s.\n", inMode);
    return 0;
}

int KeyManager_removeEventListenerLua(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);
    const char* inMode = luaL_checkstring(L, 2);
    if(strcmp(inMode, "DOWN") == 0)
    {
        for(size_t i = 0; i < keymanager.keyDownListeners_size; i++)
        {
            struct eventListener* listener = &keymanager.keyDownListeners[i];
            if(listener->func == LUA_NOREF || listener->L == NULL)
                break; // Salir del bucle porque el resto de referencias serán inválidas.
            lua_rawgeti(L, LUA_REGISTRYINDEX, listener->func);
            if(lua_rawequal(L, 1, -1)) // Son iguales?
            {
                // Limpiamos la referencia.
                luaL_unref(L, LUA_REGISTRYINDEX, listener->func);
                // Reemplazamos el Event Listener actual por el ultimo.
                int lastIndex = keymanager.keyDownListenersCount - 1;
                keymanager.keyDownListeners[i] = keymanager.keyDownListeners[lastIndex];
                // Reemplazar el ultimo elemento por un listener nulo.
                keymanager.keyDownListeners[lastIndex] = (struct eventListener){LUA_NOREF, NULL};
                keymanager.keyDownListenersCount--; // Ahora decrementamos la cuenta
                break;
            }
        }
    }
    else if(strcmp(inMode, "UP") == 0)
    {
        for(size_t i = 0; i < keymanager.keyUpListeners_size; i++)
        {
            struct eventListener* listener = &keymanager.keyUpListeners[i];
            if(listener->func == LUA_NOREF || listener->L == NULL)
                break;
            lua_rawgeti(L, LUA_REGISTRYINDEX, listener->func);
            if(lua_rawequal(L, 1, -1))
            {
                luaL_unref(L, LUA_REGISTRYINDEX, listener->func);
                int lastIndex = keymanager.keyUpListenersCount - 1;
                keymanager.keyUpListeners[i] = keymanager.keyUpListeners[lastIndex];
                keymanager.keyUpListeners[lastIndex] = (struct eventListener){LUA_NOREF, NULL};
                keymanager.keyUpListenersCount--;
                break;
            }
        }
    }
    else
        return luaL_error(L, "the parameter 'inMode' can only be 'UP' or 'DOWN' but was received: %s.\n", inMode);
    return 0;
}

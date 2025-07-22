#include "Sol/sol.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Invierte los bytes de un array de bytes.
static void reverse(void* ptr, size_t size)
{
    unsigned char* bytes = (unsigned char*)ptr;
    unsigned char temp;
    size_t begin = 0;
    size_t end = size - 1;

    while (begin < end)
    {
        temp = bytes[begin];
        bytes[begin] = bytes[end];
        bytes[end] = temp;
        begin++;
        end--;
    }
}

void Sol_getAttributeName(SolAttribute* attribute, char source[])
{
    memcpy(source, attribute->name, attribute->nameSize);
    source[attribute->nameSize] = '\0';
}

static void decode_SolObjectLua(lua_State* L)
{

}

static int Sol_readFile(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);
    FILE* file = fopen(path, "rb");
    // Leemos el encabezado del archivo.
    SolFileHeader header;
    fread(&header, sizeof(SolFileHeader), 1, file);
    reverse(&header.fileSize, sizeof(header.fileSize)); // Invertimos los bytes del que nos dice el tamaño del archivo porque los numeros estan en little endian.
    size_t bytes2read = header.fileSize - 0x0A;
    if(bytes2read == sizeof(SolObjectInit)) // Contiene atributos?
    {
        // Si no contiene, pues devolvemos una tabla vacia.
        fclose(file);
        lua_newtable(L);
        return 1;
    }
    unsigned char* fileData = (unsigned char*)malloc(bytes2read); 
    // Leemos ahora el archivo.
    fseek(file, 0x10, SEEK_SET);
    fread(fileData, bytes2read, 1, file);
    fclose(file); // Cerramos el archivo porque ya no nos sirve.
    SolAttribute* attribute = (SolAttribute*)(fileData + sizeof(SolObjectInit));
    size_t offset = sizeof(SolObjectInit);
    char name[255]; // Creamos un string de 255 caracteres ya que es valor maximo de un Uint8 para cada nombre de los atributos.
    lua_newtable(L); // Creamos una tabla para almacenar todos los datos del archivo .sol
    for(;;)
    {
        Sol_getAttributeName(attribute, name);
        int type = attribute->name[attribute->nameSize];
        switch(type)
        {
            case NUMBER:
            {
                SolNumber* number = (SolNumber*)&attribute->name[attribute->nameSize];
                reverse(&number->value, sizeof(number->value));
                lua_pushnumber(L, number->value);
                offset += (sizeof(SolAttribute) + attribute->nameSize) + sizeof(SolNumber);
                break;
            }
            case BOOLEAN:
            {
                SolBoolean* boolean = (SolBoolean*)&attribute->name[attribute->nameSize];
                lua_pushboolean(L, boolean->value);
                offset += (sizeof(SolAttribute) + attribute->nameSize) + sizeof(SolBoolean);
                break;
            }
            case STRING:
            {
                SolString* string = (SolString*)&attribute->name[attribute->nameSize];
                char* stringValue = (char*)malloc(string->stringSize + 1);
                memcpy(stringValue, string->value, string->stringSize);
                stringValue[string->stringSize] = '\0';
                lua_pushstring(L, stringValue);
                free(stringValue);
                offset += (sizeof(SolAttribute) + attribute->nameSize) + (sizeof(SolString) + string->stringSize);
                break;
            }
        }
        lua_setfield(L, -2, name);
        if(offset < bytes2read)
            attribute = (SolAttribute*)&fileData[offset];
        else
            break;
    }
    free(fileData);
    return 1;
}

extern void Sol_register(lua_State* L)
{
    lua_newtable(L);

    lua_pushcfunction(L, Sol_readFile);
    lua_setfield(L, -2, "read");

    lua_setglobal(L, "Sol");
}
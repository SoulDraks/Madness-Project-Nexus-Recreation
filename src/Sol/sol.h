#ifndef SOL_H
#define SOL_H

#include <stdint.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
typedef unsigned char Byte;
typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;

typedef enum {
    NUMBER,
    BOOLEAN,
    STRING,
    OBJECT,
    NuLL,
    UNDEFINED,
    ARRAY,
    XML
} SolTypes;

typedef struct __attribute__((packed)) SolFileHeader {
    Byte reserved[2]; // Dos bytes extra que ni yo mismo se que utilidad tienen pero siempre en todo archivo .sol tienen el valor 0x00BF
    Uint32 fileSize; // Tamaño del archivo.
    char TSCO[5]; // String que contiene un texto "TSCO". son 5 caracteres en total por el caracter nulo(\0) del final
    Byte reserved2[5]; // Otros bytes que tampoco se para que son.
} SolFileHeader;

typedef struct __attribute__((packed)) {
    Byte beginTag; // Una "etiqueta" que indica cuando comienza el atributo. su valor es siempre 00
    Uint8 nameSize; // Cantidad de caracteres
    char name[]; // String del nombre del atributo. NOTA: El string en el archivo sol no tiene el caracter nulo(\0)
} SolAttribute;

typedef struct __attribute__((packed)) {
    Byte type; // Tipo del dato.
    double value;
    Byte endTag; // Etiqueta que marca el final del atributo. siempre es 00
} SolNumber;

typedef struct __attribute__((packed)) {
    Byte type;
    Byte value;
    Byte endTag;
} SolBoolean;

typedef struct __attribute__((packed)) {
    Byte type;
    Uint16 stringSize;
    char value[];
    // Se supone que despues del nombre, viene el "endTag". asi que solo supon que esta :D
} SolString;

typedef struct __attribute__((packed)) {
    Byte type;
    SolAttribute items[];
} SolObject;

// El primer objeto que aparece en un archivo .sol.
typedef struct __attribute__((packed)) {
    Byte beginTag;
    Uint8 nameSize;
    char name[4];
    Byte reserved[4]; // 4 bytes que ni siquiera se para que son XD. 
} SolObjectInit;

#define SOLOBJECTENDTAG(bytes) bytes[0] = 0x00; bytes[1] = 0x09; bytes[2] = 0x00;

extern void Sol_getAttributeName(SolAttribute* attribute, char source[]);
extern void Sol_register(lua_State* L);

#endif
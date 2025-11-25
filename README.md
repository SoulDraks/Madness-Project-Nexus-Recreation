# Madness Project Nexus - Recreation

Recreación no oficial del juego "Madness Project Nexus: Classic", desarrollada en C y Lua.
## 👥 Integrantes:
- Alexis Martinez
- Juan Martinez
- Mia Rojas
- Isaias Ortega

## 💻 Bibliotecas usadas:
- **SDL2** — Renderizado, ventanas y manejo de eventos
- **SDL2_image** — Carga de imágenes y texturas adicionales
- **SDL2_ttf** — Renderizado de texto
- **Lua** — Lógica de gameplay y scripts

## 🛠 Compilacion
El proyecto utiliza SCons como sistema de build.

Para compilar el proyecto:
```Shell
scons
```
ya el script se encarga de todo de la construccion y dependencias

# 📂 Estructura del Proyecto
```
Madness Project Nexus - Recreation
├─ assets     -> Imágenes, fuentes y recursos
├─ external   -> Bibliotecas/Dependencias del proyecto
├─ scripts    -> Scripts Lua (IA, escenas, gameplay)
├─ src        -> Código en C (motor, render, físicas, etc.)
└─ SConstruct -> Archivo de build del proyecto
```

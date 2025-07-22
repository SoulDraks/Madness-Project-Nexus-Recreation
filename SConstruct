from SCons.Script import *
import os
import platform
import subprocess

env = Environment()
env.Append(CCFLAGS=["-Og", "-g"])

# Rutas de las bibliotecas usadas.
SDL_dir = "external/SDL-2.30.12"
SDL_image_dir = "external/SDL_image-2.8.8"
SDL_ttf_dir = "external/SDL_ttf-2.24.0"
lua_dir = "external/lua"

# Rutas a los include de las bibliotecas usadas
SDL_include_dir = SDL_dir + "/include"
SDL_image_include_dir = SDL_image_dir + "/include"
SDL_ttf_include_dir = SDL_ttf_dir
lua_include_dir = lua_dir

# Bibliotecas a enlazar.
libs = [
    "SDL2", "SDL2_image",
    "SDL2_mixer", "SDL2_ttf"
]

"""
def buildSDL(target, source, env):
    SDL_build_dir = os.path.join(SDL_dir, 'build')
    SDL_lib1_path = os.path.join(SDL_build_dir, 'libSDL2.a')
    SDL_lib2_path = os.path.join(SDL_build_dir, 'libSDL2main.a')
    sdlHasBeenBuilded = True
    if not os.path.exists(SDL_build_dir) or not os.path.exists(SDL_lib1_path) or not os.path.exists(SDL_lib2_path):
        sdlHasBeenBuilded = False
    if sdlHasBeenBuilded:
        # Si ya fue compilado SDL, pues nos salimos.
        return None
    # Configurar con CMake
    subprocess.run(['cmake', '..'], cwd=SDL_build_dir, check=True)
    # Compilar SDL
    subprocess.run(['cmake', '--build', '.'], cwd=SDL_build_dir, check=True)
    return None
    
def buildSDL_image(target, source, env):
    SDL_image_files = Glob(SDL_image_dir + "/*.c")
    SDL_image_lib = env.StaticLibrary("libSDL_image", SDL_image_files)
    sourceCode.append(SDL_image_lib)
    return None

def buildSDL_ttf():
    pass
"""

def buildLua():
    srcLua = [
        "lapi.c", "lauxlib.c", "lbaselib.c", "lcode.c",
        "lcorolib.c", "lctype.c", "ldblib.c", "ldebug.c",
        "ldo.c", "ldump.c", "lfunc.c", "lgc.c", "linit.c",
        "liolib.c", "llex.c", "lmathlib.c", "lmem.c",
        "loadlib.c", "lobject.c", "lopcodes.c", "loslib",
        "lparser.c", "lstate.c", "lstring.c", "lstrlib.c",
        "ltable.c", "ltablib.c", "ltests.c", "ltm.c",
        "lundump.c", "lutf8lib.c", "lvm.c", "lzio.c"
    ]
    for i in range(len(srcLua)):
        srcLua[i] = lua_dir + "/" + srcLua[i]
    libLua = env.StaticLibrary("liblua", srcLua)
    libs.append(libLua)
    
buildLua()

# Carpetas de inclusion para bibliotecas.
_platform = platform.system()
include_path = [
    lua_include_dir,
    SDL_include_dir,
    SDL_image_include_dir,
    SDL_ttf_include_dir,
    "src"
]
env.Append(CPPPATH=include_path)

env.Append(LIBS=libs)
env.Append(LIBPATH=["."])

# Carpetas donde se haya codigo.
sources_folders = [
    "Core",
    "Core/Animation",
    "Core/UI",
    "Core/Graphics",
    "Engine",
    "Input",
    "Lua",
    "Math",
    "Render",
    "Sol",
    "types",
]

# Array o lista contenedor de todos los archivos de codigo.
sourceCode: list = []

for folder in sources_folders:
    folder_path = os.path.join("src", folder)
    # Añadimos a la lista todos los archivos de codigo de esa carpeta.
    sourceCode += Glob(os.path.join(folder_path, "*.c"))
    
sourceCode += Glob("src/*.c")

Madness_Game = env.Program('Madness_Project_Nexus', sourceCode)

Default(Madness_Game)
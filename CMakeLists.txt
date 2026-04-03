cmake_minimum_required(VERSION 3.10)
project(CubeWorldModLauncher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Archivos fuente
set(SOURCES
    main.cpp
    Process.cpp
)

set(HEADERS
    main.h
    Process.h
)

# Crear ejecutable CONSOLE
add_executable(CubeWorldModLauncher ${SOURCES} ${HEADERS})

# Librerías
target_link_libraries(CubeWorldModLauncher 
    shlwapi
    user32
    kernel32
)

# Opciones de compilación para MSVC
if(MSVC)
    target_compile_options(CubeWorldModLauncher PRIVATE /W4)
    target_compile_definitions(CubeWorldModLauncher PRIVATE _CRT_SECURE_NO_WARNINGS)
    
    set_target_properties(CubeWorldModLauncher PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
        SUBSYSTEM "CONSOLE"
    )
endif()

# Salida
set_target_properties(CubeWorldModLauncher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

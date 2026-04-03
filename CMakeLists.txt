cmake_minimum_required(VERSION 3.10)
project(CubeWorldModLauncherAlpha)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Archivos fuente
set(SOURCES
    main.cpp
    Process.cpp
    DLL.cpp
    crc.cpp
    mutex.cpp
)

set(HEADERS
    main.h
    Process.h
    DLL.h
    crc.h
    mutex.h
    GenericMod.h
)

# Crear ejecutable
add_executable(CubeWorldModLauncher ${SOURCES} ${HEADERS})

# Librerías
target_link_libraries(CubeWorldModLauncher 
    shlwapi
    user32
    kernel32
)

# Opciones de compilación para MSVC
if(MSVC)
    target_compile_options(CubeWorldModLauncher PRIVATE 
        /W4 
        /utf-8
        /permissive-
        /std:c++17
    )
    
    target_compile_definitions(CubeWorldModLauncher PRIVATE 
        _CRT_SECURE_NO_WARNINGS
        _WINSOCK_DEPRECATED_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
    
    set_target_properties(CubeWorldModLauncher PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
        SUBSYSTEM "CONSOLE"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# Directorio de salida
set_target_properties(CubeWorldModLauncher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Mensaje de éxito
message(STATUS "========================================")
message(STATUS "Configuración completada exitosamente")
message(STATUS "Proyecto: Cube World Mod Launcher v2.5")
message(STATUS "Ejecutable: bin/CubeWorldModLauncher.exe")
message(STATUS "Modo: Release")
message(STATUS "Standard C++: 17")
message(STATUS "========================================")
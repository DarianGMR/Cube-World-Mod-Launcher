cmake_minimum_required(VERSION 3.10)
project(CubeWorldModLauncher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurar para Windows
set(CMAKE_SYSTEM_NAME Windows)

# Launcher Principal
add_executable(CubeWorldModLauncher main.cpp)
target_link_libraries(CubeWorldModLauncher shlwapi)

# Configurar opciones de compilación
if(MSVC)
    target_compile_options(CubeWorldModLauncher PRIVATE /W4 /WX)
    set_target_properties(CubeWorldModLauncher PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
    )
endif()

# Configurar salida
set_target_properties(CubeWorldModLauncher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo Cube World Mod Launcher - Build Script
echo ========================================
echo.

color 0A

REM ============================================
REM Detectar Visual Studio Build Tools 2019
REM ============================================
echo [1/6] Detectando Visual Studio Build Tools...
echo.

set "VS_PATH="
set "VCVARS_FILE="

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools"
    set "VCVARS_FILE=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
    goto :found_vs
)

if exist "C:\Program Files\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2019\BuildTools"
    set "VCVARS_FILE=C:\Program Files\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
    goto :found_vs
)

echo ERROR: No se encontró Visual Studio Build Tools 2019
pause
exit /b 1

:found_vs
echo [✓] Visual Studio Build Tools 2019 encontrado
echo   Ruta: !VS_PATH!
echo.

REM ============================================
REM Configurar variables de entorno
REM ============================================
echo [2/6] Configurando entorno de compilación...

call "!VCVARS_FILE!"
if errorlevel 1 (
    echo ERROR: No se pudo ejecutar vcvars32.bat
    pause
    exit /b 1
)

echo [✓] Entorno configurado
echo.

REM ============================================
REM Verificar CMake
REM ============================================
echo [3/6] Verificando CMake...

where cmake >nul 2>&1
if errorlevel 1 (
    if exist "C:\Program Files\CMake\bin\cmake.exe" (
        set "PATH=C:\Program Files\CMake\bin;!PATH!"
        echo [✓] CMake encontrado
    ) else (
        echo ERROR: CMake no está instalado
        pause
        exit /b 1
    )
) else (
    echo [✓] CMake encontrado en PATH
)
echo.

REM ============================================
REM Preparar directorio de build
REM ============================================
echo [4/6] Preparando directorio de build...

if exist build (
    rmdir /s /q build >nul 2>&1
)

mkdir build
cd build

echo [✓] Directorio build creado
echo.

REM ============================================
REM Ejecutar CMake
REM ============================================
echo [5/6] Ejecutando CMake...

cmake -G "Visual Studio 16 2019" -A Win32 ..

if errorlevel 1 (
    echo ERROR: CMake falló
    cd ..
    pause
    exit /b 1
)

echo [✓] CMake completado
echo.

REM ============================================
REM Compilar proyecto
REM ============================================
echo [6/6] Compilando proyecto...

cmake --build . --config Release

if errorlevel 1 (
    echo ERROR: La compilación falló
    cd ..
    pause
    exit /b 1
)

echo [✓] Proyecto compilado
echo.

REM ============================================
REM Copiar ejecutable
REM ============================================

if exist "bin\Release\CubeWorldModLauncher.exe" (
    copy "bin\Release\CubeWorldModLauncher.exe" "..\CubeWorldModLauncher.exe" >nul
    echo [✓] Ejecutable: CubeWorldModLauncher.exe
) else (
    echo ERROR: Ejecutable no encontrado
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo BUILD COMPLETADO ✓
echo ========================================
echo.
pause

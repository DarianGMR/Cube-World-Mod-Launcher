@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo Cube World Mod Launcher - Build Script
echo ========================================
echo.

color 0A

REM ============================================
REM Detectar Visual Studio Build Tools / Community
REM ============================================
echo [1/6] Detectando Visual Studio...

set "VS_PATH="
set "VS_VERSION="
set "VCVARS_FILE="

REM Buscar Visual Studio 2022 Community/Professional/Enterprise
for /d %%D in ("C:\Program Files*\Microsoft Visual Studio\2022\*") do (
    if exist "%%D\VC\Auxiliary\Build\vcvars32.bat" (
        set "VS_PATH=%%D"
        set "VS_VERSION=2022"
        set "VCVARS_FILE=%%D\VC\Auxiliary\Build\vcvars32.bat"
        goto :found_vs
    )
)

REM Buscar Visual Studio 2019 Community/Professional/Enterprise
for /d %%D in ("C:\Program Files*\Microsoft Visual Studio\2019\*") do (
    if exist "%%D\VC\Auxiliary\Build\vcvars32.bat" (
        set "VS_PATH=%%D"
        set "VS_VERSION=2019"
        set "VCVARS_FILE=%%D\VC\Auxiliary\Build\vcvars32.bat"
        goto :found_vs
    )
)

REM Buscar Visual Studio Build Tools 2022
for /d %%D in ("C:\Program Files*\Microsoft Visual Studio\2022\*") do (
    if exist "%%D\VC\Auxiliary\Build\vcvars32.bat" (
        set "VS_PATH=%%D"
        set "VS_VERSION=Build Tools 2022"
        set "VCVARS_FILE=%%D\VC\Auxiliary\Build\vcvars32.bat"
        goto :found_vs
    )
)

REM Buscar Visual Studio Build Tools 2019
for /d %%D in ("C:\Program Files*\Microsoft Visual Studio\2019\*") do (
    if exist "%%D\VC\Auxiliary\Build\vcvars32.bat" (
        set "VS_PATH=%%D"
        set "VS_VERSION=Build Tools 2019"
        set "VCVARS_FILE=%%D\VC\Auxiliary\Build\vcvars32.bat"
        goto :found_vs
    )
)

REM Buscar en BuildTools explícitamente
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools"
    set "VS_VERSION=Build Tools 2019"
    set "VCVARS_FILE=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
    goto :found_vs
)

if exist "C:\Program Files\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2019\BuildTools"
    set "VS_VERSION=Build Tools 2019"
    set "VCVARS_FILE=C:\Program Files\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
    goto :found_vs
)

REM Si no encontró, mostrar error
echo ERROR: No se encontró Visual Studio o Build Tools.
echo Ubicaciones buscadas:
echo - C:\Program Files\Microsoft Visual Studio\
echo - C:\Program Files (x86)\Microsoft Visual Studio\
echo.
pause
exit /b 1

:found_vs
echo [✓] !VS_VERSION! encontrado en: !VS_PATH!
echo.

REM ============================================
REM Configurar variables de entorno
REM ============================================
echo [2/6] Configurando entorno de compilación...
call "!VCVARS_FILE!"
if errorlevel 1 (
    echo ERROR: No se pudo configurar el entorno de Visual Studio.
    pause
    exit /b 1
)
echo [✓] Entorno configurado.
echo.

REM ============================================
REM Verificar CMake
REM ============================================
echo [3/6] Verificando CMake...
where cmake >nul 2>&1
if errorlevel 1 (
    echo WARNING: CMake no está en el PATH del sistema.
    echo Intentando buscar CMake instalado...
    
    if exist "C:\Program Files\CMake\bin\cmake.exe" (
        set "PATH=C:\Program Files\CMake\bin;!PATH!"
    ) else if exist "C:\Program Files (x86)\CMake\bin\cmake.exe" (
        set "PATH=C:\Program Files (x86)\CMake\bin;!PATH!"
    ) else (
        echo ERROR: CMake no se encontró.
        echo Descárgalo desde: https://cmake.org/download/
        pause
        exit /b 1
    )
)
echo [✓] CMake encontrado.
echo.

REM ============================================
REM Preparar directorio de build
REM ============================================
echo [4/6] Preparando directorio de build...
if exist build (
    echo Eliminando build anterior...
    rmdir /s /q build >nul 2>&1
)
mkdir build
cd build
echo [✓] Directorio build creado.
echo.

REM ============================================
REM Ejecutar CMake
REM ============================================
echo [5/6] Ejecutando CMake...
if "!VS_VERSION!"=="2022" (
    cmake -G "Visual Studio 17 2022" -A Win32 ..
) else if "!VS_VERSION!"=="Build Tools 2022" (
    cmake -G "Visual Studio 17 2022" -A Win32 ..
) else (
    cmake -G "Visual Studio 16 2019" -A Win32 ..
)

if errorlevel 1 (
    echo ERROR: CMake falló.
    echo Intenta ejecutar manualmente: cmake -G "Visual Studio 16 2019" -A Win32 ..
    cd ..
    pause
    exit /b 1
)
echo [✓] CMake completado.
echo.

REM ============================================
REM Compilar proyecto
REM ============================================
echo [6/6] Compilando proyecto (esto puede tardar un poco)...
cmake --build . --config Release
if errorlevel 1 (
    echo ERROR: La compilación falló.
    cd ..
    pause
    exit /b 1
)
echo [✓] Proyecto compilado exitosamente.
echo.

REM ============================================
REM Copiar ejecutable
REM ============================================
if exist bin\Release\CubeWorldModLauncher.exe (
    copy bin\Release\CubeWorldModLauncher.exe ..\CubeWorldModLauncher.exe >nul
    echo [✓] Ejecutable creado: CubeWorldModLauncher.exe
) else if exist bin\CubeWorldModLauncher.exe (
    copy bin\CubeWorldModLauncher.exe ..\CubeWorldModLauncher.exe >nul
    echo [✓] Ejecutable creado: CubeWorldModLauncher.exe
) else (
    echo ERROR: No se encontró el ejecutable compilado en:
    echo bin\Release\CubeWorldModLauncher.exe
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo BUILD COMPLETADO EXITOSAMENTE ✓
echo ========================================
echo.
echo Tu archivo ejecutable está listo:
echo -> CubeWorldModLauncher.exe
echo.
echo Instrucciones de uso:
echo 1. Coloca CubeWorldModLauncher.exe en la misma carpeta que Cube.exe
echo 2. Crea una carpeta llamada "Mods" en esa misma ubicación
echo 3. Coloca los DLLs de los mods dentro de la carpeta "Mods"
echo 4. Ejecuta CubeWorldModLauncher.exe
echo.
pause
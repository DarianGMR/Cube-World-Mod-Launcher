#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <shlwapi.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "main.h"
#include "Process.h"
#include "DLL.h"
#include "crc.h"

#pragma comment(lib, "shlwapi.lib")

using namespace std;

// Variables globales
static FILE* logFile = nullptr;

// ============================================
// FUNCIÓN DE LOGGING
// ============================================
void LogMessage(const char* message)
{
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", &timeinfo);
    
    printf("[%s] %s\n", timestamp, message);
    
    if (logFile) {
        fprintf(logFile, "[%s] %s\n", timestamp, message);
        fflush(logFile);
    }
}

// ============================================
// FUNCIONES AUXILIARES
// ============================================

bool FileExists(const char* szPath)
{
    DWORD dwAttrib = GetFileAttributesA(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool DirectoryExists(const char* szPath)
{
    DWORD dwAttrib = GetFileAttributesA(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void ShowError(const char* title, const char* message)
{
    MessageBoxA(NULL, message, title, MB_OK | MB_ICONERROR);
    LogMessage(message);
}

void ShowInfo(const char* title, const char* message)
{
    MessageBoxA(NULL, message, title, MB_OK | MB_ICONINFORMATION);
    LogMessage(message);
}

void ShowWarning(const char* title, const char* message)
{
    MessageBoxA(NULL, message, title, MB_OK | MB_ICONWARNING);
    LogMessage(message);
}

int Bail(int result, const char* message)
{
    if (message) {
        printf("Presiona ENTER para salir.\n");
        cin.ignore();
    }
    if (logFile) {
        fclose(logFile);
    }
    return result;
}

bool ValidateCubeExecutable(const char* exePath)
{
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, exePath, "rb");
    
    if (err != 0 || !file) {
        ShowError("File Error", "No se puede leer Cube.exe.\nAsegúrate de que el archivo no esté en uso.");
        return false;
    }
    
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fclose(file);

    // Validar tamaño exacto
    if (fileSize != CUBE_WORLD_SIZE) {
        char msg[512];
        sprintf_s(msg, sizeof(msg),
            "Cube.exe encontrado pero el tamaño es incorrecto.\n\n"
            "Esperado: %ld bytes\n"
            "Actual: %ld bytes\n\n"
            "Asegúrate de tener Cube World Alpha 0.1.1\n"
            "Descárgalo de: https://github.com/ChrisMiuchiz/Cube-World-Cracker",
            CUBE_WORLD_SIZE, fileSize);
        ShowError("Validación fallida", msg);
        return false;
    }

    LogMessage("Cube.exe validado correctamente (tamaño correcto)");
    return true;
}

void PrintLoadedMods(const vector<string>& mods)
{
    string modsInfo = "Mods Cargados:\n===================\n";
    
    if (mods.empty()) {
        modsInfo += "Sin mods adicionales (CallbackManager si está cargado)";
    } else {
        for (const auto& mod : mods) {
            modsInfo += "• " + mod + "\n";
        }
    }
    
    modsInfo += "\n===================\nTotal: " + to_string(mods.size()) + " mod(s)";
    
    ShowInfo("Estado de Mods", modsInfo.c_str());
    LogMessage(modsInfo.c_str());
}

// ============================================
// FUNCIÓN PRINCIPAL
// ============================================

int main()
{
    // Configurar locales y UTF-8
    setlocale(LC_ALL, "es_ES.UTF-8");
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    // Crear archivo de log
    errno_t logErr = fopen_s(&logFile, "CubeWorldLauncher.log", "a");
    if (logErr != 0) {
        printf("ADVERTENCIA: No se pudo crear archivo de log\n");
    }

    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║  Cube World Mod Launcher - Alpha 0.1.1 ║\n");
    printf("║           Versión Mejorada v2.0        ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");

    LogMessage("===== INICIANDO LAUNCHER =====");

    // [1/6] Verificar Cube.exe
    printf("[1/6] Verificando Cube.exe...\n");
    LogMessage("[1/6] Verificando Cube.exe");
    
    if (!FileExists(CUBE_EXECUTABLE)) {
        char msg[256];
        sprintf_s(msg, sizeof(msg), 
            "%s no encontrado.\n"
            "Coloca este launcher en la misma carpeta que Cube.exe",
            CUBE_EXECUTABLE);
        ShowError("Archivo no encontrado", msg);
        return Bail(1, "");
    }

    if (!ValidateCubeExecutable(CUBE_EXECUTABLE)) {
        return Bail(1, "");
    }
    printf("  [✓] Cube.exe validado correctamente\n\n");

    // [2/6] Verificar CallbackManager.dll
    printf("[2/6] Verificando CallbackManager.dll...\n");
    LogMessage("[2/6] Verificando CallbackManager.dll");
    
    if (!FileExists(MODLOADER_DLL)) {
        char msg[256];
        sprintf_s(msg, sizeof(msg), 
            "%s no encontrado.\n"
            "Este archivo es obligatorio para el funcionamiento del launcher.",
            MODLOADER_DLL);
        ShowError("Archivo no encontrado", msg);
        return Bail(1, "");
    }
    printf("  [✓] CallbackManager.dll encontrado\n\n");

    // [3/6] Crear carpeta de mods
    printf("[3/6] Preparando carpeta de mods...\n");
    LogMessage("[3/6] Preparando carpeta de mods");
    
    if (!DirectoryExists(MOD_DIRECTORY)) {
        if (CreateDirectoryA(MOD_DIRECTORY, NULL)) {
            printf("  [✓] Carpeta '%s' creada\n\n", MOD_DIRECTORY);
            LogMessage("Carpeta de mods creada");
        } else {
            ShowError("Error en directorio", "No se pudo crear la carpeta 'Mods'.");
            return Bail(1, "");
        }
    } else {
        printf("  [✓] Carpeta '%s' encontrada\n\n", MOD_DIRECTORY);
    }

    // [4/6] Crear proceso suspendido
    printf("[4/6] Iniciando Cube.exe (suspendido)...\n");
    LogMessage("[4/6] Iniciando Cube.exe");
    
    Process gameProcess(CUBE_EXECUTABLE);
    
    if (!gameProcess.Create()) {
        char msg[256];
        sprintf_s(msg, sizeof(msg), "Error al crear proceso: 0x%lX", GetLastError());
        ShowError("Error de proceso", msg);
        return Bail(1, "");
    }
    printf("  [✓] Cube.exe iniciado (suspendido)\n\n");

    vector<string> loadedMods;
    
    // Agregar CallbackManager (obligatorio)
    loadedMods.push_back(MODLOADER_DLL);

    // [5/6] Buscar y cargar mods
    printf("[5/6] Buscando mods compatibles con Alpha 0.1.1...\n");
    LogMessage("[5/6] Buscando mods en carpeta Mods");
    
    HANDLE hFind;
    WIN32_FIND_DATAA data;

    hFind = FindFirstFileA(MOD_SEARCH_PATTERN, &data);
    int modCount = 0;
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // No volver a agregar CallbackManager
            if (strcmp(data.cFileName, MODLOADER_DLL) != 0) {
                string modPath = string(MOD_DIRECTORY) + "\\" + data.cFileName;
                loadedMods.push_back(modPath);
                printf("  • Mod encontrado: %s\n", data.cFileName);
                LogMessage(("Mod encontrado: " + string(data.cFileName)).c_str());
                modCount++;
            }
        } while (FindNextFileA(hFind, &data));
        FindClose(hFind);
    }

    if (modCount == 0) {
        printf("  (No hay mods adicionales en la carpeta)\n");
        LogMessage("No se encontraron mods adicionales");
    }
    printf("\n");

    // [6/6] Inyectar DLLs
    printf("[6/6] Inyectando DLLs en proceso...\n");
    LogMessage("[6/6] Inyectando DLLs");
    
    int successCount = 0;
    int totalDLLs = loadedMods.size();

    for (const string& dllName : loadedMods) {
        if (gameProcess.InjectDLL(dllName)) {
            successCount++;
        } else {
            printf("    [✗] Error al inyectar: %s\n", dllName.c_str());
            LogMessage(("ERROR al inyectar: " + dllName).c_str());
        }
    }

    printf("\n");

    // Pequeña pausa para que la inyección se estabilice
    printf("Esperando estabilización de inyección (250ms)...\n");
    Sleep(250);

    // Reanudar proceso
    printf("Reanudando proceso principal...\n\n");
    gameProcess.Resume();
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║         Información de Inyección       ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║  DLLs Inyectados: %d/%d                  ║\n", successCount, totalDLLs);
    printf("║  Estado: %s                      ║\n", (successCount == totalDLLs) ? "✓ OK" : "⚠ PARCIAL");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");

    if (successCount < totalDLLs) {
        ShowWarning("Advertencia", 
            "Algunos mods no se inyectaron correctamente.\n"
            "El juego puede no funcionar como se espera.");
    }

    printf("Esperando a que Cube World se cierre...\n");
    printf("(Esta ventana se cerrará cuando cierres el juego)\n\n");

    LogMessage("Esperando proceso...");

    // Esperar a que el proceso termine
    gameProcess.Wait();

    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║     Cube World ha sido cerrado ✓       ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");

    LogMessage("===== LAUNCHER FINALIZADO =====");

    PrintLoadedMods(loadedMods);

    printf("Presiona ENTER para cerrar...\n");
    cin.ignore();

    if (logFile) {
        fclose(logFile);
    }

    return 0;
}

#include "main.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <fstream>
#include <ctime>
#include <shlwapi.h>
#include <cstdio>
#include "Process.h"
#include "DLL.h"
#include "ResourceManager.h"

#pragma comment(lib, "shlwapi.lib")

// Declaración de constantes
static const char CUBE_EXECUTABLE[] = "Cube.exe";
static const char MODLOADER_DLL[] = "CallbackManager.dll";
static const char MODS_DIRECTORY[] = "Mods";
static const char LOG_FILE[] = "CubeWorldLauncher.log";

// ============================================
// FUNCIONES DE LOGGING
// ============================================

void LogMessage(const char* message) {
    std::ofstream logFile(LOG_FILE, std::ios::app);
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    char timeBuffer[100];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    
    logFile << "[" << timeBuffer << "] " << message << std::endl;
    logFile.close();
}

void LogError(const char* message) {
    std::ofstream logFile(LOG_FILE, std::ios::app);
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    char timeBuffer[100];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    
    logFile << "[" << timeBuffer << "] [ERROR] " << message << std::endl;
    logFile.close();
}

void LogWarning(const char* message) {
    std::ofstream logFile(LOG_FILE, std::ios::app);
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    char timeBuffer[100];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    
    logFile << "[" << timeBuffer << "] [WARNING] " << message << std::endl;
    logFile.close();
}

void ShowError(const char* title, const char* message) {
    MessageBoxA(NULL, message, title, MB_OK | MB_ICONERROR);
    char logMsg[1024];
    sprintf_s(logMsg, sizeof(logMsg), "[ERROR] %s: %s", title, message);
    LogError(logMsg);
}

void ShowInfo(const char* title, const char* message) {
    MessageBoxA(NULL, message, title, MB_OK | MB_ICONINFORMATION);
    char logMsg[1024];
    sprintf_s(logMsg, sizeof(logMsg), "[INFO] %s: %s", title, message);
    LogMessage(logMsg);
}

void ShowWarning(const char* title, const char* message) {
    MessageBoxA(NULL, message, title, MB_OK | MB_ICONWARNING);
    char logMsg[1024];
    sprintf_s(logMsg, sizeof(logMsg), "[WARNING] %s: %s", title, message);
    LogWarning(logMsg);
}

int Bail(int result, const char* message) {
    if (message) {
        LogError(message);
        ShowError("Error", message);
    }
    printf("Presiona Enter para salir...\n");
    std::cin.ignore();
    return result;
}

// ============================================
// FUNCIONES AUXILIARES
// ============================================

bool FileExists(const char* fileName) {
    DWORD dwAttrib = GetFileAttributesA(fileName);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool DirectoryExists(const char* dirName) {
    DWORD dwAttrib = GetFileAttributesA(dirName);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool CreateModsDirectory() {
    if (!DirectoryExists(MODS_DIRECTORY)) {
        if (!CreateDirectoryA(MODS_DIRECTORY, NULL)) {
            LogError("No se pudo crear el directorio Mods");
            return false;
        }
        LogMessage("Directorio Mods creado exitosamente");
    }
    return true;
}

std::vector<std::string> LoadModsFromDirectory() {
    std::vector<std::string> mods;
    
    if (!DirectoryExists(MODS_DIRECTORY)) {
        LogWarning("Directorio de Mods no encontrado. Se creara automáticamente.");
        CreateModsDirectory();
        return mods;
    }

    WIN32_FIND_DATAA findData;
    HANDLE findHandle;
    char searchPath[MAX_PATH];
    
    sprintf_s(searchPath, sizeof(searchPath), "%s\\*.dll", MODS_DIRECTORY);
    
    findHandle = FindFirstFileA(searchPath, &findData);
    if (findHandle != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                char modPath[MAX_PATH];
                sprintf_s(modPath, sizeof(modPath), "%s\\%s", MODS_DIRECTORY, findData.cFileName);
                mods.push_back(modPath);
                
                char logMsg[512];
                sprintf_s(logMsg, sizeof(logMsg), "Mod encontrado: %s", findData.cFileName);
                LogMessage(logMsg);
                
                printf("Mod encontrado: %s\n", findData.cFileName);
            }
        } while (FindNextFileA(findHandle, &findData));
        FindClose(findHandle);
    }

    return mods;
}

bool ValidateCubeExecutable(const char* exePath) {
    if (!FileExists(exePath)) {
        LogError("Cube.exe no encontrado");
        return false;
    }

    WIN32_FILE_ATTRIBUTE_DATA fileAttr;
    if (!GetFileAttributesExA(exePath, GetFileExInfoStandard, &fileAttr)) {
        LogError("No se pudo acceder a Cube.exe");
        return false;
    }

    const long EXPECTED_SIZE = 3885568;
    const long SIZE_TOLERANCE = 100000;
    
    long actualSize = fileAttr.nFileSizeLow;
    if (actualSize < EXPECTED_SIZE - SIZE_TOLERANCE || actualSize > EXPECTED_SIZE + SIZE_TOLERANCE) {
        char msg[512];
        sprintf_s(msg, sizeof(msg), 
            "Versión incorrecta de Cube.exe\n\n"
            "Tamaño esperado: ~%ld bytes\n"
            "Tamaño actual: %ld bytes\n\n"
            "Por favor, usa Cube World Alpha 0.1.1 exacta",
            EXPECTED_SIZE, actualSize);
        ShowWarning("Validación de Version", msg);
    }

    LogMessage("Cube.exe validado correctamente");
    return true;
}

// ============================================
// FUNCIÓN - GESTIONAR CALLBACK MANAGER
// ============================================

bool GetOrExtractCallbackManager() {
    printf("[3/6] Verificando CallbackManager.dll...\n");
    LogMessage("Verificando disponibilidad de CallbackManager.dll");
    
    // Verificar si ya existe el archivo en disco
    if (FileExists(MODLOADER_DLL)) {
        printf("CallbackManager.dll encontrado en la carpeta del juego\n");
        LogMessage("CallbackManager.dll ya existe en disco");
        return true;
    }
    
    // Si no existe, intentar extraer desde recursos incrustados
    printf("CallbackManager.dll no encontrado, extrayendo desde recursos...\n");
    LogMessage("CallbackManager.dll no encontrado, intentando extraer desde recursos");
    
    if (ResourceManager::ExtractResourceByName("CALLBACKMANAGER_DLL", "BINARY", MODLOADER_DLL)) {
        LogMessage("CallbackManager.dll extraido exitosamente desde recursos");
        printf("CallbackManager.dll extraido desde recursos internos\n");
        return true;
    }
    
    // Si falla todo
    LogError("No se pudo obtener CallbackManager.dll");
    return false;
}

// ============================================
// FUNCIÓN PRINCIPAL
// ============================================

int main(int argc, char** argv) {
    DeleteFileA(LOG_FILE);
    
    LogMessage("========================================");
    LogMessage("Iniciando Cube World Mod Launcher v2.5");
    LogMessage("========================================");

    printf("\n");
    printf("========================================\n");
    printf("Iniciando Cube World Mod Launcher v2.5\n");
    printf("========================================\n\n");

    const char* cubeExe = CUBE_EXECUTABLE;
    const char* modDll = MODLOADER_DLL;

    if (argc >= 2) {
        cubeExe = argv[1];
    }

    // [1/5] Validar Cube.exe
    printf("[1/5] Validando Cube.exe...\n");
    if (!ValidateCubeExecutable(cubeExe)) {
        return Bail(1, "Cube.exe no es la version correcta (Alpha 0.1.1)");
    }
    printf("Cube.exe validado correctamente\n\n");

    // [2/5] Crear directorio de Mods
    printf("[2/5] Verificando directorio de Mods...\n");
    if (!CreateModsDirectory()) {
        return Bail(1, "No se pudo crear el directorio de Mods");
    }
    printf("Directorio de Mods listo\n\n");

    // [3/5] Obtener o extraer CallbackManager.dll
    if (!GetOrExtractCallbackManager()) {
        return Bail(1, "CallbackManager.dll no se pudo obtener ni extraer");
    }
    printf("CallbackManager.dll disponible\n\n");

    // [4/5] Cargar mods
    printf("[4/5] Cargando mods de la carpeta Mods...\n");
    std::vector<std::string> mods = LoadModsFromDirectory();
    printf("\n");

    // [4/5] Crear proceso
    LogMessage("Creando proceso de Cube.exe en estado suspendido");
    
    Process process(cubeExe);

    if (!process.Create()) {
        char msg[256];
        sprintf_s(msg, sizeof(msg), "No se pudo crear el proceso: %lu", GetLastError());
        return Bail(1, msg);
    }

    LogMessage("Proceso de Cube.exe creado correctamente");

    printf("Inyectando CallbackManager.dll...\n");
    if (!process.InjectDLL(modDll)) {
        return Bail(1, "No se pudo inyectar CallbackManager.dll");
    }
    LogMessage("CallbackManager.dll inyectado exitosamente");

    // Inyectar mods
    if (!mods.empty()) {
        printf("Inyectando mods...\n");
        
        for (const auto& mod : mods) {
            if (process.InjectDLL(mod)) {
                // Éxito
            } else {
                char msg[512];
                sprintf_s(msg, sizeof(msg), "Advertencia: No se pudo inyectar %s", mod.c_str());
                LogError(msg);
            }
        }
        
        LogMessage("Mods inyectados exitosamente");
    }

    printf("Inyeccion exitosa\n\n");

    Sleep(1000);

    // [5/5] Reanudar proceso
    printf("[5/5] Iniciando CubeWorld...\n\n");
    process.Resume();

    LogMessage("========================================");
    LogMessage("Cube World iniciado correctamente");
    LogMessage("========================================");

    process.Wait();

    LogMessage("========================================");
    LogMessage("Cube World finalizado");
    LogMessage("========================================");

    return 0;
}
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <shlwapi.h>
#include <cstdio>
#include <cstdlib>
#include "main.h"
#include "Process.h"

#pragma comment(lib, "shlwapi.lib")

using namespace std;

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
    fprintf(stderr, "ERROR: %s - %s\n", title, message);
}

void ShowInfo(const char* title, const char* message)
{
    MessageBoxA(NULL, message, title, MB_OK | MB_ICONINFORMATION);
    printf("%s: %s\n", title, message);
}

int Bail(int result, const char* message)
{
    if (message) {
        printf("Press enter to exit.\n");
        cin.ignore();
    }
    return result;
}

bool ValidateCubeExecutable(const char* exePath)
{
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, exePath, "rb");
    
    if (err != 0 || !file) {
        ShowError("File Error", "No se puede leer Cube.exe. Verifica que el archivo no esté en uso.");
        return false;
    }
    
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fclose(file);

    if (fileSize != CUBE_WORLD_SIZE) {
        char msg[512];
        sprintf_s(msg, sizeof(msg),
            "Cube.exe encontrado pero el tamaño es incorrecto.\n\n"
            "Esperado: %ld bytes\n"
            "Actual: %ld bytes\n\n"
            "Asegúrate de tener Cube World Alpha 0.1.1",
            CUBE_WORLD_SIZE, fileSize);
        ShowError("Version Check Failed", msg);
        return false;
    }

    return true;
}

int main()
{
    // Configurar idioma del sistema
    setlocale(LC_ALL, "es_ES.UTF-8");
    
    printf("====================================\n");
    printf("Cube World Mod Launcher - Alpha 0.1.1\n");
    printf("====================================\n\n");

    // [1/5] Verificar Cube.exe
    printf("[1/5] Verificando Cube.exe...\n");
    if (!FileExists(CUBE_EXECUTABLE)) {
        ShowError("Not Found", "Cube.exe no encontrado.\nAsegúrate de estar en la carpeta correcta.");
        return Bail(1);
    }

    if (!ValidateCubeExecutable(CUBE_EXECUTABLE)) {
        return Bail(1);
    }
    printf("[✓] Cube.exe validado correctamente.\n\n");

    // [2/5] Verificar CallbackManager.dll
    printf("[2/5] Verificando CallbackManager.dll...\n");
    if (!FileExists(MODLOADER_DLL)) {
        ShowError("Not Found", "CallbackManager.dll no encontrado.\nEste archivo es obligatorio.");
        return Bail(1);
    }
    printf("[✓] CallbackManager.dll encontrado.\n\n");

    // [3/5] Crear carpeta de mods si no existe
    printf("[3/5] Preparando carpeta de mods...\n");
    if (!DirectoryExists(MOD_DIRECTORY)) {
        if (CreateDirectoryA(MOD_DIRECTORY, NULL)) {
            printf("[✓] Carpeta 'Mods' creada.\n\n");
        } else {
            ShowError("Directory Error", "No se pudo crear la carpeta 'Mods'.");
            return Bail(1);
        }
    } else {
        printf("[✓] Carpeta 'Mods' encontrada.\n\n");
    }

    // [4/5] Crear proceso suspendido
    printf("[4/5] Iniciando Cube.exe...\n");
    
    Process gameProcess(CUBE_EXECUTABLE);
    
    if (!gameProcess.Create()) {
        char msg[256];
        sprintf_s(msg, sizeof(msg), "Error al crear el proceso: 0x%lX", GetLastError());
        ShowError("Process Error", msg);
        return Bail(1);
    }
    printf("[✓] Cube.exe iniciado (suspendido).\n\n");

    vector<string> modDLLs;
    
    // Agregar CallbackManager (obligatorio)
    modDLLs.push_back(string(MODLOADER_DLL));

    // [5/5] Buscar mods en la carpeta
    printf("[5/5] Buscando mods compatibles con Alpha 0.1.1...\n");
    HANDLE hFind;
    WIN32_FIND_DATAA data;

    hFind = FindFirstFileA(MOD_SEARCH_PATTERN, &data);
    int modCount = 0;
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // No volver a agregar CallbackManager
            if (strcmp(data.cFileName, MODLOADER_DLL) != 0) {
                string modPath = string(MOD_DIRECTORY) + "\\" + data.cFileName;
                modDLLs.push_back(modPath);
                printf("  Mod encontrado: %s\n", data.cFileName);
                modCount++;
            }
        } while (FindNextFileA(hFind, &data));
        FindClose(hFind);
    }

    if (modCount == 0) {
        printf("  (No hay mods adicionales)\n");
    }
    printf("\n");

    // Inyectar DLLs
    printf("Inyectando DLLs en proceso...\n\n");
    int successCount = 0;

    for (const string& dllName : modDLLs) {
        printf("  Cargando: %s\n", dllName.c_str());
        
        if (gameProcess.InjectDLL(dllName)) {
            successCount++;
            printf("    [✓] Inyectado\n");
        } else {
            printf("    [✗] Error al inyectar\n");
        }
    }

    printf("\n");

    // Pequeña pausa para que la inyección se estabilice
    Sleep(250);

    // Reanudar proceso
    gameProcess.Resume();
    
    printf("====================================\n");
    printf("DLLs inyectados: %d/%zu\n", successCount, modDLLs.size());
    printf("====================================\n\n");
    printf("Esperando a que Cube World cierre...\n\n");

    // Esperar a que el proceso termine
    gameProcess.Wait();

    printf("\nCube World ha sido cerrado.\n");
    printf("Presiona enter para salir.\n");
    cin.ignore();

    return 0;
}

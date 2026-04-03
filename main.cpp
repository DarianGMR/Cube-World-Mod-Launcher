#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

using namespace std;

// Configuración para Cube World Alpha 0.1.1
const char CUBE_EXECUTABLE[] = "Cube.exe";
const char MODLOADER_DLL[] = "CallbackManager.dll";
const char MOD_DIRECTORY[] = "Mods";
const char MOD_SEARCH_PATTERN[] = "Mods\\*.dll";

// File size validation for Cube World Alpha 0.1.1
const long CUBE_WORLD_SIZE = 3885568;

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
    printf("ERROR: %s\n", message);
}

void ShowInfo(const char* title, const char* message)
{
    MessageBoxA(NULL, message, title, MB_OK | MB_ICONINFORMATION);
    printf("%s\n", message);
}

int Bail(int result, const char* message = nullptr)
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
    setlocale(LC_ALL, "es_ES.UTF-8");
    
    printf("====================================\n");
    printf("Cube World Mod Launcher - Alpha\n");
    printf("====================================\n\n");

    // Verificar Cube.exe
    printf("[1/5] Verificando Cube.exe...\n");
    if (!FileExists(CUBE_EXECUTABLE)) {
        ShowError("Not Found", "Cube.exe no encontrado.\nAsegúrate de estar en la carpeta correcta.");
        return Bail(1);
    }

    if (!ValidateCubeExecutable(CUBE_EXECUTABLE)) {
        return Bail(1);
    }
    printf("[✓] Cube.exe validado correctamente.\n\n");

    // Verificar CallbackManager.dll
    printf("[2/5] Verificando CallbackManager.dll...\n");
    if (!FileExists(MODLOADER_DLL)) {
        ShowError("Not Found", "CallbackManager.dll no encontrado.\nEste archivo es obligatorio.");
        return Bail(1);
    }
    printf("[✓] CallbackManager.dll encontrado.\n\n");

    // Crear carpeta de mods si no existe
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

    // Crear proceso suspendido
    printf("[4/5] Iniciando Cube.exe...\n");
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(NULL,
                        (LPSTR)CUBE_EXECUTABLE,
                        NULL,
                        NULL,
                        FALSE,
                        CREATE_SUSPENDED,
                        NULL,
                        NULL,
                        &si,
                        &pi))
    {
        char msg[256];
        sprintf_s(msg, sizeof(msg), "Error al crear el proceso: 0x%lX", GetLastError());
        ShowError("Process Error", msg);
        return Bail(1);
    }
    printf("[✓] Cube.exe iniciado (suspendido).\n\n");

    vector<string> modDLLs;
    
    // Agregar CallbackManager (obligatorio)
    modDLLs.push_back(string(MODLOADER_DLL));

    // Buscar mods en la carpeta
    printf("[5/5] Buscando mods...\n");
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
    vector<HANDLE> threads;
    int successCount = 0;

    for (const string& S_DLLName : modDLLs) {
        printf("  Cargando: %s\n", S_DLLName.c_str());

        LPVOID load_library = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        if (!load_library) {
            printf("    ERROR: No se pudo obtener LoadLibraryA\n");
            continue;
        }

        size_t dllNameLen = S_DLLName.length() + 1;
        LPVOID remote_string = VirtualAllocEx(pi.hProcess, NULL, dllNameLen, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!remote_string) {
            printf("    ERROR: No se pudo asignar memoria remota\n");
            continue;
        }

        if (!WriteProcessMemory(pi.hProcess, remote_string, S_DLLName.c_str(), dllNameLen, NULL)) {
            printf("    ERROR: No se pudo escribir en memoria remota\n");
            VirtualFreeEx(pi.hProcess, remote_string, 0, MEM_RELEASE);
            continue;
        }

        HANDLE thread = CreateRemoteThread(pi.hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)load_library, remote_string, CREATE_SUSPENDED, NULL);
        if (thread) {
            threads.push_back(thread);
            ResumeThread(thread);
            successCount++;
            printf("    [✓] Inyectado\n");
        } else {
            printf("    ERROR: No se pudo crear el thread remoto\n");
            VirtualFreeEx(pi.hProcess, remote_string, 0, MEM_RELEASE);
        }
    }

    printf("\n");

    // Pequeña pausa para que la inyección se estabilice
    // Esto previene condiciones de carrera (race conditions)
    Sleep(250);

    // Reanudar proceso
    ResumeThread(pi.hThread);
    
    printf("====================================\n");
    printf("DLLs inyectados: %d/%zu\n", successCount, modDLLs.size());
    printf("====================================\n\n");
    printf("Esperando a que Cube World cierre...\n\n");

    // Esperar a que el proceso termine
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Limpiar recursos
    for (HANDLE thread : threads) {
        CloseHandle(thread);
    }
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printf("\nCube World ha sido cerrado.\n");
    printf("Presiona enter para salir.\n");
    cin.ignore();

    return 0;
}

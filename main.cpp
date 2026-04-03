#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <shlwapi.h>

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

int main()
{
    setlocale(LC_ALL, "");
    vector<string> modDLLs;

    // Cube world is obviously required
    if (!FileExists("Cube.exe")) {
        printf("ERROR: Cube World no encontrado.\n");
        printf("Asegurate de que Cube.exe este en la misma carpeta que este launcher.\n");
        Sleep(2000);
        return 1;
    }

    FILE* file = nullptr;
    errno_t err = fopen_s(&file, "Cube.exe", "rb");
    if (err != 0 || !file) {
        printf("ERROR: No se puede leer Cube.exe\n");
        Sleep(2000);
        return 1;
    }
    
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fclose(file);

    const long CUBE_SIZE = 3885568;
    if (fileSize != CUBE_SIZE) {
        printf("ERROR: Cube World encontrado, pero no es la version 0.1.1\n");
        printf("Por favor actualiza tu juego.\n");
        printf("Presiona enter para salir.\n");
        cin.ignore();
        return 1;
    }

    // The callback manager is required
    if (!FileExists("CallbackManager.dll")) {
        printf("ERROR: CallbackManager.dll no encontrado.\n");
        Sleep(2000);
        return 1;
    }

    modDLLs.push_back(string("CallbackManager.dll"));
    
    const char MOD_PATH[] = "Mods\\*.dll";
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    if (!DirectoryExists("Mods")) {
        CreateDirectoryA("Mods", NULL);
        printf("Carpeta 'Mods' creada.\n");
    }

    // Create game in suspended state
    printf("Iniciando Cube.exe...\n\n");
    if (!CreateProcessA(NULL,
                        (LPSTR)"Cube.exe",
                        NULL,
                        NULL,
                        FALSE,
                        CREATE_SUSPENDED,
                        NULL,
                        NULL,
                        &si,
                        &pi))
    {
        printf("ERROR: Fallo al crear el proceso: %lu\n", GetLastError());
        return 1;
    }
    else {
        printf("Cube.exe iniciado exitosamente.\n\n");
    }

    // Find mods
    HANDLE hFind;
    WIN32_FIND_DATAA data;

    hFind = FindFirstFileA(MOD_PATH, &data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            string modPath = string("Mods\\") + data.cFileName;
            modDLLs.push_back(modPath);
        } while (FindNextFileA(hFind, &data));
        FindClose(hFind);
    }

    // Inject DLLs
    vector<HANDLE> threads;
    for (string S_DLLName : modDLLs) {
        printf("Cargando: %s\n", S_DLLName.c_str());

        LPVOID load_library = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        if (!load_library) {
            printf("ERROR: No se pudo obtener LoadLibraryA\n");
            continue;
        }

        size_t dllNameLen = S_DLLName.length() + 1;
        LPVOID remote_string = VirtualAllocEx(pi.hProcess, NULL, dllNameLen, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!remote_string) {
            printf("ERROR: No se pudo asignar memoria en el proceso remoto\n");
            continue;
        }

        if (!WriteProcessMemory(pi.hProcess, remote_string, S_DLLName.c_str(), dllNameLen, NULL)) {
            printf("ERROR: No se pudo escribir en la memoria del proceso\n");
            VirtualFreeEx(pi.hProcess, remote_string, 0, MEM_RELEASE);
            continue;
        }

        HANDLE thread = CreateRemoteThread(pi.hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)load_library, remote_string, CREATE_SUSPENDED, NULL);
        if (thread) {
            threads.push_back(thread);
            ResumeThread(thread);
        } else {
            printf("ERROR: No se pudo crear el thread remoto\n");
            VirtualFreeEx(pi.hProcess, remote_string, 0, MEM_RELEASE);
        }
    }

    ResumeThread(pi.hThread);
    
    printf("\nTodos los mods disponibles han sido cargados.\n");
    printf("Esperando a que el juego cierre...\n");
    
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    for (HANDLE thread : threads) {
        CloseHandle(thread);
    }
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}

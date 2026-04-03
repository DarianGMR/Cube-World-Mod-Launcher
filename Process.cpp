#include "Process.h"
#include <cstring>
#include <windows.h>
#include <cstdio>

Process::Process(const std::string& executablePath)
    : executablePath(executablePath)
{
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
}

Process::~Process()
{
    Cleanup();
}

bool Process::Create()
{
    return CreateProcessA(
        NULL,
        (LPSTR)executablePath.c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &si,
        &pi
    ) != FALSE;
}

bool Process::InjectDLL(const std::string& dllName)
{
    if (pi.hProcess == NULL) {
        printf("ERROR: Process handle is NULL\n");
        return false;
    }

    // Obtener dirección de LoadLibraryA
    LPVOID load_library = (LPVOID)GetProcAddress(
        GetModuleHandleA("kernel32.dll"),
        "LoadLibraryA"
    );
    
    if (!load_library) {
        printf("ERROR: Could not get LoadLibraryA address\n");
        return false;
    }

    // Asignar memoria en el proceso remoto
    size_t dllNameLen = dllName.length() + 1;
    LPVOID remote_string = VirtualAllocEx(
        pi.hProcess,
        NULL,
        dllNameLen,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
    
    if (!remote_string) {
        printf("ERROR: Could not allocate remote memory\n");
        return false;
    }

    // Escribir el nombre de la DLL en memoria remota
    if (!WriteProcessMemory(
        pi.hProcess,
        remote_string,
        dllName.c_str(),
        dllNameLen,
        NULL
    )) {
        printf("ERROR: Could not write to remote memory\n");
        VirtualFreeEx(pi.hProcess, remote_string, 0, MEM_RELEASE);
        return false;
    }

    // Crear un thread remoto que cargue la DLL
    HANDLE thread = CreateRemoteThread(
        pi.hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)load_library,
        remote_string,
        CREATE_SUSPENDED,
        NULL
    );
    
    if (!thread) {
        printf("ERROR: Could not create remote thread\n");
        VirtualFreeEx(pi.hProcess, remote_string, 0, MEM_RELEASE);
        return false;
    }

    // Reanudar el thread
    ResumeThread(thread);
    injectedThreads.push_back(thread);

    return true;
}

void Process::Resume()
{
    if (pi.hThread != NULL) {
        ResumeThread(pi.hThread);
    }
}

void Process::Wait()
{
    if (pi.hProcess != NULL) {
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
}

void Process::WriteJMP(BYTE* location, BYTE* newFunction)
{
    DWORD dwOldProtection;
    VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    
    location[0] = 0xE9; // JMP opcode
    
    // Calcular el offset del JMP de forma correcta
    DWORD offset = (DWORD)((intptr_t)newFunction - (intptr_t)location - 5);
    *((DWORD*)(location + 1)) = offset;
    
    VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}

void Process::Cleanup()
{
    // Cerrar todos los threads inyectados
    for (HANDLE thread : injectedThreads) {
        if (thread != NULL) {
            CloseHandle(thread);
        }
    }
    injectedThreads.clear();
    
    // Cerrar handles del proceso
    if (pi.hProcess != NULL) {
        CloseHandle(pi.hProcess);
    }
    if (pi.hThread != NULL) {
        CloseHandle(pi.hThread);
    }
}

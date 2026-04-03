#include "Process.h"
#include <cstring>
#include <windows.h>
#include <cstdio>
#include <cassert>

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
    assert(!executablePath.empty());

    if (!CreateProcessA(
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
    )) {
        fprintf(stderr, "ERROR: No se pudo crear proceso de %s\n", executablePath.c_str());
        return false;
    }

    printf("  Proceso creado exitosamente: PID=%lu\n", pi.dwProcessId);
    return true;
}

bool Process::InjectDLL(const std::string& dllName)
{
    if (pi.hProcess == NULL) {
        fprintf(stderr, "ERROR: Handle del proceso es NULL\n");
        return false;
    }

    assert(!dllName.empty());

    // Obtener dirección de LoadLibraryA desde kernel32.dll
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    if (kernel32 == NULL) {
        fprintf(stderr, "ERROR: No se pudo obtener kernel32.dll\n");
        return false;
    }

    LPVOID load_library = (LPVOID)GetProcAddress(kernel32, "LoadLibraryA");
    
    if (load_library == NULL) {
        fprintf(stderr, "ERROR: No se pudo obtener dirección de LoadLibraryA\n");
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
    
    if (remote_string == NULL) {
        fprintf(stderr, "ERROR: No se pudo asignar memoria remota para: %s\n", dllName.c_str());
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
        fprintf(stderr, "ERROR: No se pudo escribir en memoria remota\n");
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
    
    if (thread == NULL) {
        fprintf(stderr, "ERROR: No se pudo crear thread remoto\n");
        VirtualFreeEx(pi.hProcess, remote_string, 0, MEM_RELEASE);
        return false;
    }

    // Reanudar el thread
    if (ResumeThread(thread) == static_cast<DWORD>(-1)) {
        fprintf(stderr, "ERROR: No se pudo reanudar thread remoto\n");
        CloseHandle(thread);
        VirtualFreeEx(pi.hProcess, remote_string, 0, MEM_RELEASE);
        return false;
    }

    injectedThreads.push_back(thread);
    printf("    [✓] %s inyectado en proceso remoto\n", dllName.c_str());

    return true;
}

void Process::Resume()
{
    if (pi.hThread != NULL) {
        DWORD result = ResumeThread(pi.hThread);
        if (result != static_cast<DWORD>(-1)) {
            printf("  Proceso reanudado exitosamente\n");
        } else {
            fprintf(stderr, "WARNING: No se pudo reanudar el proceso\n");
        }
    }
}

void Process::Wait()
{
    if (pi.hProcess != NULL) {
        DWORD result = WaitForSingleObject(pi.hProcess, INFINITE);
        if (result == WAIT_OBJECT_0) {
            printf("  Proceso finalizado\n");
        } else {
            fprintf(stderr, "WARNING: Error esperando al proceso\n");
        }
    }
}

bool Process::IsRunning() const
{
    if (pi.hProcess == NULL) {
        return false;
    }
    
    DWORD exitCode;
    if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
        return false;
    }
    return exitCode == STILL_ACTIVE;
}

void Process::WriteJMP(BYTE* location, BYTE* newFunction)
{
    assert(location != nullptr);
    assert(newFunction != nullptr);

    DWORD dwOldProtection;
    if (!VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection)) {
        fprintf(stderr, "ERROR: No se pudo cambiar protección de memoria\n");
        return;
    }
    
    location[0] = 0xE9; // JMP opcode
    
    DWORD offset = (DWORD)((intptr_t)newFunction - (intptr_t)location - 5);
    *((DWORD*)(location + 1)) = offset;
    
    DWORD dwOldProtectionRestore;
    if (!VirtualProtect(location, 5, dwOldProtection, &dwOldProtectionRestore)) {
        fprintf(stderr, "ERROR: No se pudo restaurar protección de memoria\n");
    }
}

void Process::Cleanup()
{
    for (HANDLE thread : injectedThreads) {
        if (thread != NULL) {
            CloseHandle(thread);
        }
    }
    injectedThreads.clear();
    
    if (pi.hProcess != NULL) {
        CloseHandle(pi.hProcess);
        pi.hProcess = NULL;
    }
    if (pi.hThread != NULL) {
        CloseHandle(pi.hThread);
        pi.hThread = NULL;
    }
}

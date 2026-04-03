#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <windows.h>
#include <vector>

// Clase para gestionar procesos de Cube World
class Process
{
public:
    // Constructor y destructor
    Process(const std::string& executablePath);
    virtual ~Process();

    // Crear proceso suspendido
    bool Create();
    
    // Inyectar una DLL en el proceso remoto
    bool InjectDLL(const std::string& dllName);
    
    // Reanudar el proceso principal
    void Resume();
    
    // Esperar a que el proceso termine
    void Wait();
    
    // Obtener handles del proceso
    HANDLE GetProcessHandle() const { return pi.hProcess; }
    HANDLE GetThreadHandle() const { return pi.hThread; }
    DWORD GetProcessID() const { return pi.dwProcessId; }
    
    // Verificar si el proceso está corriendo
    bool IsRunning() const;
    
    // Forzar terminar el proceso
    void Terminate(UINT exitCode = 0);

private:
    std::string executablePath;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    std::vector<HANDLE> injectedThreads;

    void WriteJMP(BYTE* location, BYTE* newFunction);
    void Cleanup();
};

#endif // PROCESS_H
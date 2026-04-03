#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <windows.h>
#include <vector>

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
    
    // Verificar si el proceso está corriendo
    bool IsRunning() const;

private:
    std::string executablePath;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    std::vector<HANDLE> injectedThreads;

    void WriteJMP(BYTE* location, BYTE* newFunction);
    void Cleanup();
};

#endif // PROCESS_H

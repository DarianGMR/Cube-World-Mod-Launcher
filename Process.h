#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <windows.h>

class Process
{
public:
    Process(const std::string& executablePath);
    virtual ~Process();

    // Crear proceso suspendido
    bool Create();
    
    // Inyectar una DLL en el proceso
    bool InjectDLL(const std::string& dllName);
    
    // Reanudar proceso principal
    void Resume();
    
    // Obtener handle del proceso
    HANDLE GetProcessHandle() const { return pi.hProcess; }
    
    // Obtener handle del thread principal
    HANDLE GetThreadHandle() const { return pi.hThread; }

private:
    std::string executablePath;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    // Función auxiliar para escribir un JMP
    void WriteJMP(BYTE* location, BYTE* newFunction);
};

#endif // PROCESS_H
#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <windows.h>
#include <vector>

class Process
{
public:
    // Constructor
    Process(const std::string& executablePath);
    
    // Destructor
    virtual ~Process();

    // Crear proceso suspendido
    bool Create();
    
    // Inyectar una DLL en el proceso
    bool InjectDLL(const std::string& dllName);
    
    // Reanudar proceso principal
    void Resume();
    
    // Esperar a que el proceso termine
    void Wait();
    
    // Obtener handle del proceso
    HANDLE GetProcessHandle() const { return pi.hProcess; }
    
    // Obtener handle del thread principal
    HANDLE GetThreadHandle() const { return pi.hThread; }

private:
    std::string executablePath;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    std::vector<HANDLE> injectedThreads;

    // Función auxiliar para escribir un JMP
    void WriteJMP(BYTE* location, BYTE* newFunction);
    
    // Limpiar recursos
    void Cleanup();
};

#endif // PROCESS_H

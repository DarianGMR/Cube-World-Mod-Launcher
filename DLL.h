#ifndef DLL_H
#define DLL_H

#include <iostream>
#include <windows.h>
#include <string>
#include "GenericMod.h"

// Clase para gestionar carga de DLLs de mods
class DLL
{
public:
    std::string fileName;
    HMODULE handle;

    // Funciones exportadas por los mods
    FARPROC ModPreInitialize;
    FARPROC ModMajorVersion;
    FARPROC ModMinorVersion;
    FARPROC MakeMod;

    GenericMod* mod;

    // Constructor y destructor
    DLL(std::string fileName);
    HMODULE Load();
    bool IsLoaded() const { return handle != nullptr; }
    virtual ~DLL();

private:
    // Privado
};

#endif // DLL_H
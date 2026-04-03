#include "DLL.h"
#include <iostream>
#include <cstdio>
#include <cassert>
#include <windows.h>

DLL::DLL(std::string fileName) 
    : fileName(std::move(fileName)), 
      handle(nullptr), 
      mod(nullptr),
      ModPreInitialize(nullptr),
      ModMajorVersion(nullptr),
      ModMinorVersion(nullptr),
      MakeMod(nullptr)
{
}

HMODULE DLL::Load() {
    assert(!fileName.empty());

    this->handle = LoadLibraryA(this->fileName.c_str());
    if (!this->handle) {
        DWORD error = GetLastError();
        char errorMsg[512];
        sprintf_s(errorMsg, sizeof(errorMsg), 
            "No se pudo cargar %s\nError: %lu\n\n"
            "Asegurate de que:\n"
            "1. El archivo está en la carpeta Mods\n"
            "2. El formato DLL es compatible\n"
            "3. Todas las dependencias están disponibles",
            this->fileName.c_str(), error);
        
        MessageBoxA(NULL, errorMsg, "Error al cargar mod", MB_OK | MB_ICONERROR);
        fprintf(stderr, "ERROR: No se pudo cargar %s: %lu\n", this->fileName.c_str(), error);
        return nullptr;
    }
    
    printf("  Mod cargado exitosamente: %s\n", this->fileName.c_str());
    return this->handle;
}

DLL::~DLL() {
    if (this->handle != nullptr) {
        if (!FreeLibrary(this->handle)) {
            fprintf(stderr, "WARNING: No se pudo liberar DLL: %s (Error: %lu)\n", 
                    this->fileName.c_str(), GetLastError());
        }
        this->handle = nullptr;
    }
    if (this->mod != nullptr) {
        delete this->mod;
        this->mod = nullptr;
    }
}
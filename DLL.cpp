#include "DLL.h"
#include <iostream>
#include <cstdio>
#include <cassert>

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
        char errorMsg[512];
        sprintf_s(errorMsg, sizeof(errorMsg), 
            "No se pudo cargar %s\nError: %ld\n\n"
            "Asegúrate de que:\n"
            "1. El archivo está en la carpeta Mods\n"
            "2. El formato DLL es compatible\n"
            "3. Todas las dependencias están disponibles",
            this->fileName.c_str(), GetLastError());
        MessageBoxA(NULL, errorMsg, "Error al cargar mod", MB_OK | MB_ICONERROR);
        fprintf(stderr, "ERROR: No se pudo cargar %s: %ld\n", this->fileName.c_str(), GetLastError());
        return nullptr;
    }
    printf("  Mod cargado exitosamente: %s\n", this->fileName.c_str());
    return this->handle;
}

DLL::~DLL() {
    if (this->handle != nullptr) {
        if (!FreeLibrary(this->handle)) {
            fprintf(stderr, "WARNING: No se pudo liberar DLL: %s\n", this->fileName.c_str());
        }
        this->handle = nullptr;
    }
    if (this->mod != nullptr) {
        delete this->mod;
        this->mod = nullptr;
    }
}
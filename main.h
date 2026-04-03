#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <windows.h>
#include <string>

// Configuración para Cube World Alpha 0.1.1
const char CUBE_EXECUTABLE[] = "Cube.exe";
const char MODLOADER_DLL[] = "CallbackManager.dll";
const char MOD_DIRECTORY[] = "Mods";
const char MOD_SEARCH_PATTERN[] = "Mods\\*.dll";

// File size validation para Cube World Alpha 0.1.1
const long CUBE_WORLD_SIZE = 3885568;

// Funciones públicas
bool FileExists(const char* szPath);
bool DirectoryExists(const char* szPath);
void ShowError(const char* title, const char* message);
void ShowInfo(const char* title, const char* message);
int Bail(int result, const char* message = nullptr);
bool ValidateCubeExecutable(const char* exePath);

#endif // MAIN_H_INCLUDED

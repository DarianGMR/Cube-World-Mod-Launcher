#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <windows.h>
#include <string>
#include <vector>
#include <cstdint>

// ============================================
// CONFIGURACIÓN PARA CUBE WORLD ALPHA 0.1.1
// ============================================

// Ejecutable y librerías
const char CUBE_EXECUTABLE[] = "Cube.exe";
const char MODLOADER_DLL[] = "CallbackManager.dll";
const char MOD_DIRECTORY[] = "Mods";
const char MOD_SEARCH_PATTERN[] = "Mods\\*.dll";

// Validación de versión
const long CUBE_WORLD_SIZE = 3885568;  // Tamaño exacto de Cube.exe Alpha 0.1.1
const uint32_t CUBE_ALPHA_CRC = 0x00000000;  // CRC placeholder para Alpha

// Configuración de mods
const int MOD_MAJOR_VERSION = 1;
const int MOD_MINOR_VERSION = 0;

// ============================================
// DECLARACIONES DE FUNCIONES
// ============================================

bool FileExists(const char* szPath);
bool DirectoryExists(const char* szPath);
void ShowError(const char* title, const char* message);
void ShowInfo(const char* title, const char* message);
void ShowWarning(const char* title, const char* message);
int Bail(int result, const char* message = nullptr);
bool ValidateCubeExecutable(const char* exePath);
void PrintLoadedMods(const std::vector<std::string>& mods);
void LogMessage(const char* message);

#endif // MAIN_H_INCLUDED

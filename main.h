#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <windows.h>
#include <string>
#include <vector>
#include <cstdint>
#include "mutex.h"

// ============================================
// CONFIGURACIÓN PARA CUBE WORLD ALPHA 0.1.1
// VERSION 2.5 - MEJORADA
// ============================================

// Configuración constante
const long CUBE_WORLD_SIZE = 3885568;
const uint32_t CUBE_ALPHA_CRC = 0x00000000;
const int MOD_MAJOR_VERSION = 2;
const int MOD_MINOR_VERSION = 5;
const DWORD DLL_INJECTION_TIMEOUT = 5000;
const int MAX_MODS = 32;
const int STABILIZATION_TIME = 1000;

// ============================================
// DECLARACIONES DE FUNCIONES
// ============================================

bool FileExists(const char* szPath);
bool DirectoryExists(const char* szPath);
bool CreateModsDirectory();
bool ValidateCubeExecutable(const char* exePath);
std::vector<std::string> LoadModsFromDirectory();
void PrintLoadedMods(const std::vector<std::string>& mods);
void ShowError(const char* title, const char* message);
void ShowInfo(const char* title, const char* message);
void ShowWarning(const char* title, const char* message);
void LogMessage(const char* message);
void LogError(const char* message);
void LogWarning(const char* message);
int Bail(int result, const char* message);

#endif // MAIN_H_INCLUDED
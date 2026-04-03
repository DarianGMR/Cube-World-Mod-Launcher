#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <windows.h>
#include <string>
#include <cstring>
#include <cstdio>

class ResourceManager {
public:
    // Extraer recurso desde el ejecutable a un archivo
    static bool ExtractResourceByName(const char* resourceName,
                                       const char* resourceType,
                                       const std::string& outputPath) {
        HMODULE hModule = GetModuleHandle(NULL);
        if (!hModule) {
            fprintf(stderr, "ERROR: No se pudo obtener el modulo del ejecutable\n");
            return false;
        }

        HRSRC hResource = FindResourceA(hModule, resourceName, resourceType);
        if (!hResource) {
            fprintf(stderr, "ERROR: Recurso '%s' no encontrado\n", resourceName);
            return false;
        }

        DWORD resourceSize = SizeofResource(hModule, hResource);
        if (resourceSize == 0) {
            fprintf(stderr, "ERROR: Tamaño del recurso es 0\n");
            return false;
        }

        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (!hGlobal) {
            fprintf(stderr, "ERROR: No se pudo cargar el recurso en memoria\n");
            return false;
        }

        LPVOID pResourceData = LockResource(hGlobal);
        if (!pResourceData) {
            fprintf(stderr, "ERROR: No se pudo obtener los datos del recurso\n");
            return false;
        }

        FILE* outputFile = NULL;
        errno_t err = fopen_s(&outputFile, outputPath.c_str(), "wb");
        if (err != 0 || !outputFile) {
            fprintf(stderr, "ERROR: No se pudo crear el archivo: %s\n", outputPath.c_str());
            return false;
        }

        size_t bytesWritten = fwrite(pResourceData, 1, resourceSize, outputFile);
        fclose(outputFile);

        if (bytesWritten != resourceSize) {
            fprintf(stderr, "ERROR: No se escribieron todos los bytes. Esperados: %lu, Escritos: %zu\n", 
                    resourceSize, bytesWritten);
            DeleteFileA(outputPath.c_str());
            return false;
        }

        return true;
    }

    // Limpiar archivos
    static bool CleanupResource(const std::string& filePath) {
        return DeleteFileA(filePath.c_str()) != FALSE;
    }
};

#endif // RESOURCEMANAGER_H
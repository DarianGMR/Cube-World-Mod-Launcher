#ifndef ZONESENERATEDHANDLER_H
#define ZONESENERATEDHANDLER_H

#include "../main.h"

extern "C" int ZoneGeneratedHandler(void* zone) {
    // Implementación para Alpha 0.1.1
    // Los mods pueden reaccionar cuando se genera una zona
    return 0;
}

void SetupZoneGeneratedHandler() {
    // Setup para Alpha 0.1.1
    // Configurar hooks de zona generada si es necesario
}

#endif // ZONESENERATEDHANDLER_H
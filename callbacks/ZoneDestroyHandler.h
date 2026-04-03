#ifndef ZONEDESTROYHANDLER_H
#define ZONEDESTROYHANDLER_H

#include "../main.h"

extern "C" int ZoneDestroyHandler(void* zone) {
    // Implementación para Alpha 0.1.1
    // Los mods pueden reaccionar cuando se destruye una zona
    return 0;
}

void SetupZoneDestroyHandler() {
    // Setup para Alpha 0.1.1
    // Configurar hooks de zona destruida si es necesario
}

#endif // ZONEDESTROYHANDLER_H
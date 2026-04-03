#ifndef CHATHANDLER_H
#define CHATHANDLER_H

#include "../main.h"

extern "C" int ChatHandler(void* msg) {
    // Implementación para Alpha 0.1.1
    // Los mods pueden procesar mensajes de chat
    return 0;
}

void SetupChatHandler() {
    // Setup para Alpha 0.1.1
    // Configurar hooks de chat si es necesario
}

#endif // CHATHANDLER_H
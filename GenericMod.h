#ifndef GENERICMOD_H
#define GENERICMOD_H

#include <windows.h>
#include <cstdint>

// Interface base para todos los mods de Cube World Alpha 0.1.1
class GenericMod {
public:
    enum Priority : uint8_t {
        VeryHighPriority = 0,
        HighPriority = 1,
        NormalPriority = 2,
        LowPriority = 3,
        VeryLowPriority = 4
    };

    virtual void Initialize() {}

    // Chat Events - Retorna true para cancelar el chat
    Priority OnChatPriority;
    virtual int OnChat(void* /*msg*/) { return 0; }

    // Inventory Full Check
    Priority OnCheckInventoryFullPriority;
    virtual int OnCheckInventoryFull(void* /*player*/, void* /*item*/) { return 0; }

    // P2P Request
    Priority OnP2PRequestPriority;
    virtual int OnP2PRequest(uint64_t /*steamID*/) { return 0; }

    // Game Tick - Se llama cada tick del juego
    Priority OnGameTickPriority;
    virtual void OnGameTick(void* /*game*/) {}

    // Zone Generated - Se llama cuando se genera una zona
    Priority OnZoneGeneratedPriority;
    virtual void OnZoneGenerated(void* /*zone*/) {}

    // Zone Destroy - Se llama cuando se destruye una zona
    Priority OnZoneDestroyPriority;
    virtual void OnZoneDestroy(void* /*zone*/) {}

    // Window Proc - Para procesamiento de mensajes de ventana
    Priority OnWindowProcPriority;
    virtual int OnWindowProc(HWND /*hwnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/) { return 0; }

    // Keyboard Input - Para procesar entrada de teclado
    Priority OnGetKeyboardStatePriority;
    virtual void OnGetKeyboardState(BYTE* /*diKeys*/) {}

    // Mouse Input - Para procesar entrada de ratón
    Priority OnGetMouseStatePriority;
    virtual void OnGetMouseState(void* /*diMouse*/) {}

    // Render - Se llama cada frame para renderizar
    Priority OnPresentPriority;
    virtual void OnPresent(void* /*SwapChain*/, UINT /*SyncInterval*/, UINT /*Flags*/) {}

    // Constructor
    GenericMod()
        : OnChatPriority(NormalPriority),
          OnCheckInventoryFullPriority(NormalPriority),
          OnP2PRequestPriority(NormalPriority),
          OnGameTickPriority(NormalPriority),
          OnZoneGeneratedPriority(NormalPriority),
          OnZoneDestroyPriority(NormalPriority),
          OnWindowProcPriority(NormalPriority),
          OnGetKeyboardStatePriority(NormalPriority),
          OnGetMouseStatePriority(NormalPriority),
          OnPresentPriority(NormalPriority)
    {
    }

    // Destructor virtual
    virtual ~GenericMod() {}
};

#endif // GENERICMOD_H
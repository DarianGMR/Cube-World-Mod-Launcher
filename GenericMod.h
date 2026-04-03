#ifndef GENERICMOD_H
#define GENERICMOD_H

#include <windows.h>
#include <cstdint>

// Interface base mejorada para todos los mods de Cube World Alpha 0.1.1
// Versión 2.5 con soporte completo de callbacks
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
    Priority OnChatPriority = NormalPriority;
    virtual int OnChat(void* /*msg*/) { return 0; }

    // Inventory Full Check
    Priority OnCheckInventoryFullPriority = NormalPriority;
    virtual int OnCheckInventoryFull(void* /*player*/, void* /*item*/) { return 0; }

    // P2P Request
    Priority OnP2PRequestPriority = NormalPriority;
    virtual int OnP2PRequest(uint64_t /*steamID*/) { return 0; }

    // Game Tick - Se llama cada tick del juego
    Priority OnGameTickPriority = NormalPriority;
    virtual void OnGameTick(void* /*game*/) {}

    // Zone Generated - Se llama cuando se genera una zona
    Priority OnZoneGeneratedPriority = NormalPriority;
    virtual void OnZoneGenerated(void* /*zone*/) {}

    // Zone Destroy - Se llama cuando se destruye una zona
    Priority OnZoneDestroyPriority = NormalPriority;
    virtual void OnZoneDestroy(void* /*zone*/) {}

    // Window Proc - Para procesamiento de mensajes de ventana
    Priority OnWindowProcPriority = NormalPriority;
    virtual int OnWindowProc(HWND /*hwnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/) { return 0; }

    // Keyboard Input - Para procesar entrada de teclado
    Priority OnGetKeyboardStatePriority = NormalPriority;
    virtual void OnGetKeyboardState(BYTE* /*diKeys*/) {}

    // Mouse Input - Para procesar entrada de ratón
    Priority OnGetMouseStatePriority = NormalPriority;
    virtual void OnGetMouseState(void* /*diMouse*/) {}

    // Render - Se llama cada frame para renderizar
    Priority OnPresentPriority = NormalPriority;
    virtual void OnPresent(void* /*SwapChain*/, UINT /*SyncInterval*/, UINT /*Flags*/) {}

    // Creature Stats Calculated Events
    Priority OnCreatureArmorCalculatedPriority = NormalPriority;
    virtual void OnCreatureArmorCalculated(void* /*creature*/, float* /*armor*/) {}

    Priority OnCreatureCriticalCalculatedPriority = NormalPriority;
    virtual void OnCreatureCriticalCalculated(void* /*creature*/, float* /*critical*/) {}

    Priority OnCreatureAttackPowerCalculatedPriority = NormalPriority;
    virtual void OnCreatureAttackPowerCalculated(void* /*creature*/, float* /*power*/) {}

    Priority OnCreatureSpellPowerCalculatedPriority = NormalPriority;
    virtual void OnCreatureSpellPowerCalculated(void* /*creature*/, float* /*power*/) {}

    Priority OnCreatureHasteCalculatedPriority = NormalPriority;
    virtual void OnCreatureHasteCalculated(void* /*creature*/, float* /*haste*/) {}

    Priority OnCreatureHPCalculatedPriority = NormalPriority;
    virtual void OnCreatureHPCalculated(void* /*creature*/, float* /*hp*/) {}

    Priority OnCreatureResistanceCalculatedPriority = NormalPriority;
    virtual void OnCreatureResistanceCalculated(void* /*creature*/, float* /*resistance*/) {}

    Priority OnCreatureRegenerationCalculatedPriority = NormalPriority;
    virtual void OnCreatureRegenerationCalculated(void* /*creature*/, float* /*regeneration*/) {}

    Priority OnCreatureManaGenerationCalculatedPriority = NormalPriority;
    virtual void OnCreatureManaGenerationCalculated(void* /*creature*/, float* /*manaGeneration*/) {}

    // Chunk Remesh Events
    Priority OnChunkRemeshPriority = NormalPriority;
    virtual void OnChunkRemesh(void* /*zone*/) {}

    Priority OnChunkRemeshedPriority = NormalPriority;
    virtual void OnChunkRemeshed(void* /*zone*/) {}

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
          OnPresentPriority(NormalPriority),
          OnCreatureArmorCalculatedPriority(NormalPriority),
          OnCreatureCriticalCalculatedPriority(NormalPriority),
          OnCreatureAttackPowerCalculatedPriority(NormalPriority),
          OnCreatureSpellPowerCalculatedPriority(NormalPriority),
          OnCreatureHasteCalculatedPriority(NormalPriority),
          OnCreatureHPCalculatedPriority(NormalPriority),
          OnCreatureResistanceCalculatedPriority(NormalPriority),
          OnCreatureRegenerationCalculatedPriority(NormalPriority),
          OnCreatureManaGenerationCalculatedPriority(NormalPriority),
          OnChunkRemeshPriority(NormalPriority),
          OnChunkRemeshedPriority(NormalPriority)
    {
    }

    // Destructor virtual
    virtual ~GenericMod() {}
};

#endif // GENERICMOD_H
# Cube World Mod Launcher - Alpha 0.1.1
## Versión 3.0 Profesional

Launcher profesional y optimizado para inyectar y gestionar mods en **Cube World Alpha 0.1.1**.

### 🎯 Características v3.0

- ✅ Validación automática y rigurosa de versión (Alpha 0.1.1 únicamente)
- ✅ Sistema de callbacks avanzado para máxima compatibilidad con mods
- ✅ Gestión segura y robusta de inyección de DLLs
- ✅ Logging completo y detallado de todas las operaciones
- ✅ Soporte para múltiples mods con manejo de prioridades
- ✅ Interfaz de usuario mejorada con colores y mensajes claros
- ✅ Sincronización de threads con mutex nativo
- ✅ Manejo avanzado de errores y recuperación
- ✅ **Compilación sin warnings - Código 100% limpio**

### 📋 Requisitos

- **Sistema Operativo:** Windows 7 o superior (32-bit o 64-bit)
- **Cube World Alpha 0.1.1** (versión exacta - 3,885,568 bytes)
- Visual Studio Build Tools 2019 o superior
- CMake 3.10 o superior

### 📦 Instalación Rápida

1. **Descargar Cube World Alpha 0.1.1:**
   ```
   https://github.com/ChrisMiuchiz/Cube-World-Cracker
   ```

2. **Copiar archivos:**
   - `CubeWorldModLauncher.exe` → Carpeta de Cube World
   - `CallbackManager.dll` → Carpeta de Cube World
   - `Cube.exe` → Verificar tamaño: 3,885,568 bytes

3. **Crear carpeta:**
   - `Mods/` → En la misma carpeta que Cube.exe

### 📂 Estructura de Carpetas

```
Cube World Alpha/
├── Cube.exe (3,885,568 bytes)
├── CubeWorldModLauncher.exe (ESTE LAUNCHER)
├── CallbackManager.dll (OBLIGATORIO)
└── Mods/
    ├── mod1.dll
    ├── mod2.dll
    └── ...
```

### 🚀 Cómo Usar

```bash
# Opción 1: Ejecutar directamente
CubeWorldModLauncher.exe

# Opción 2: Con argumento personalizado
CubeWorldModLauncher.exe "C:\Ruta\A\Cube.exe"
```

**El launcher automáticamente:**
1. ✓ Valida que Cube.exe sea Alpha 0.1.1
2. ✓ Crea la carpeta Mods si no existe
3. ✓ Inicia Cube.exe suspendido
4. ✓ Inyecta CallbackManager.dll y todos los mods
5. ✓ Reanuda el proceso
6. ✓ Espera a que el juego se cierre

### 📝 Archivo de Log

Automáticamente genera `CubeWorldLauncher-Alpha.log` con:
- Timestamp de cada operación
- Mods detectados y cargados
- Errores y advertencias
- Estado de inyección

**Ejemplo:**
```
[14:23:15] ===== INICIANDO LAUNCHER ALPHA 0.1.1 =====
[14:23:15] [PASO 1/7] Verificando Cube.exe
[14:23:15] Cube.exe validado correctamente (Alpha 0.1.1 - 3885568 bytes)
[14:23:16] Mod encontrado: MyMod.dll
```

### 🔧 Compilación desde Código Fuente

```bash
# 1. Crear directorio de build
mkdir build
cd build

# 2. Configurar con CMake
cmake -G "Visual Studio 16 2019" -A Win32 ..

# 3. Compilar
cmake --build . --config Release

# 4. Ejecutable en:
# build/bin/Release/CubeWorldModLauncher.exe
```

### ⚙️ Para Desarrolladores de Mods

Los mods deben implementar `GenericMod`:

```cpp
#include "GenericMod.h"

class MiMod : public GenericMod {
public:
    void Initialize() override {
        // Inicialización del mod
    }
    
    void OnGameTick(void* game) override {
        // Lógica por tick
    }
    
    int OnChat(void* msg) override {
        // Procesar chat
        return 0;  // 0 = permitir, 1 = cancelar
    }
};

extern "C" {
    int ModMajorVersion() { return 1; }
    int ModMinorVersion() { return 0; }
    void ModPreInitialize() {}
    GenericMod* MakeMod() { return new MiMod(); }
}
```

### 🐛 Solución de Problemas

| Problema | Solución |
|----------|----------|
| "Cube.exe no encontrado" | Coloca el launcher en la carpeta de Cube World |
| "Versión incorrecta" | Usa exactamente Cube World Alpha 0.1.1 (3,885,568 bytes) |
| "CallbackManager.dll no encontrado" | Descárgalo y colócalo en la raíz con Cube.exe |
| Los mods no cargan | Coloca los .dll en la carpeta `Mods` |
| Error de permisos | Ejecuta como administrador |
| Revisar logs | Abre `CubeWorldLauncher-Alpha.log` |

### 📚 Links Útiles

- [Cube World Cracker](https://github.com/ChrisMiuchiz/Cube-World-Cracker)
- [Documentación Original](https://github.com/ChrisMiuchiz/Cube-World-Mod-Launcher)
- [CWSDK](https://github.com/ChrisMiuchiz/CWSDK)

### 📄 Notas Técnicas

- **Arquitectura:** x86 (32-bit)
- **Runtime:** MultiThreaded
- **Lenguaje:** C++17
- **Warnings:** 0 (Compilación limpia)
- **Validación:** Tamaño exacto de archivo
- **Logging:** Timestamp completo

### 📄 Licencia

Launcher mejorado para Cube World Alpha 0.1.1. Código original basado en ChrisMiuchiz.

### 👨‍💻 Autor DarianGMR

Versión 3.0 - Actualización profesional y completa

---

**Versión:** 3.0.0  
**Target:** Cube World Alpha 0.1.1  
**Última actualizaci��n:** 2026-04-03  
**Estado:** ✅ Producción

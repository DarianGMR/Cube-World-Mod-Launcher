# Cube World Mod Launcher - Alpha 0.1.1
## Versión Mejorada v2.0

Launcher profesional para inyectar y gestionar mods en **Cube World Alpha 0.1.1**.

### 🎯 Características Principales

- ✅ Validación automática de versión (Cube World Alpha 0.1.1)
- ✅ Sistema de callbacks mejorado para mods
- ✅ Gestión segura de inyección de DLLs
- ✅ Logging completo de operaciones
- ✅ Soporte para múltiples mods simultáneamente
- ✅ Interfaz de usuario clara y en español
- ✅ Sincronización de threads con mutex

### 📋 Requisitos

- **Sistema Operativo:** Windows 7 o superior (32-bit o 64-bit)
- **Cube World Alpha 0.1.1** (versión exacta)
- Visual Studio Build Tools 2019 o superior
- CMake 3.10 o superior

### 📦 Instalación

1. **Descarga Cube World Alpha 0.1.1:**
   - Usa https://github.com/ChrisMiuchiz/Cube-World-Cracker para descargar

2. **Coloca el launcher:**
   - Descarga `CubeWorldModLauncher.exe`
   - Colócalo en la misma carpeta que `Cube.exe`

3. **Requiere estos archivos:**
   - `Cube.exe` (Cube World Alpha 0.1.1)
   - `CallbackManager.dll` (mod loader)
   - `CubeWorldModLauncher.exe` (este launcher)

### 📂 Estructura de Carpetas

```
Cube World Alpha/
├── Cube.exe
├── CubeWorldModLauncher.exe
├── CallbackManager.dll
└── Mods/
    ├── mod1.dll
    ├── mod2.dll
    └── ...
```

### 🚀 Cómo Usar

1. **Ejecuta el launcher:**
   ```
   CubeWorldModLauncher.exe
   ```

2. El launcher automáticamente:
   - Valida que Cube.exe sea la versión correcta
   - Crea la carpeta `Mods` si no existe
   - Inicia Cube.exe en modo suspendido
   - Inyecta `CallbackManager.dll` y todos los mods
   - Reanuda el proceso

3. **Instala mods:**
   - Coloca archivos `.dll` en la carpeta `Mods`
   - Reinicia el launcher

### 📝 Archivo de Log

El launcher crea automáticamente `CubeWorldLauncher.log` con:
- Hora y fecha de cada operación
- Mods cargados
- Errores o advertencias

### 🛠️ Compilación desde Código Fuente

```bash
# Crear directorio de build
mkdir build
cd build

# Configurar con CMake
cmake -G "Visual Studio 16 2019" -A Win32 ..

# Compilar
cmake --build . --config Release

# Ejecutable en: build/bin/CubeWorldModLauncher.exe
```

### ⚙️ Para Desarrolladores de Mods

Los mods deben implementar la interface `GenericMod` y exportar:

```cpp
extern "C" {
    int ModMajorVersion() { return 1; }
    int ModMinorVersion() { return 0; }
    void ModPreInitialize() {}
    GenericMod* MakeMod() { return new TuModClass(); }
}
```

### 🐛 Solución de Problemas

| Problema | Solución |
|----------|----------|
| "Cube.exe no encontrado" | Coloca el launcher en la misma carpeta que Cube.exe |
| "Versión incorrecta" | Usa Cube World Alpha 0.1.1 exacta |
| "CallbackManager.dll no encontrado" | Asegúrate de tener el archivo en la raíz |
| Los mods no cargan | Coloca los DLLs en la carpeta `Mods` |
| El juego se cierra sin razón | Revisa el archivo `.log` |

### 📚 Links Útiles

- [Cube World Cracker](https://github.com/ChrisMiuchiz/Cube-World-Cracker)
- [Documentación del Mod Loader Original](https://github.com/ChrisMiuchiz/Cube-World-Mod-Launcher)

### 📄 Licencia

Este proyecto es un launcher mejorado para Cube World Alpha 0.1.1.

### 👨‍💻 Autor Mejorado

Actualización profesional y mejoras del sistema de callbacks.

---

**Versión:** 2.0  
**Última actualización:** 2026-04-03

# SteamlessControllerDSU 🎮

[![GitHub release](https://img.shields.io/github/v/release/YOUR_USERNAME/SteamlessControllerDSU?style=flat-square&color=blue)](https://github.com/YOUR_USERNAME/SteamlessControllerDSU/releases/latest)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%7C%2011%20(x64)-informational?style=flat-square)](https://github.com/YOUR_USERNAME/SteamlessControllerDSU)
[![License](https://img.shields.io/badge/License-GPLv3-green?style=flat-square)](LICENSE)

A standalone driver and input wrapper for the **Valve Steam Controller** on Windows that operates **without the Steam client running**.

**SteamlessControllerDSU** emulates standard gamepads (**Xbox 360**, **PlayStation 4**, and **Nintendo Switch Pro**) and features an **integrated Cemuhook DSU motion server** (`127.0.0.1:26760`) for instant motion aiming in modern emulators.

---

## 🌟 Key Features

* 🌐 **Built-in Cemuhook DSU Motion Server (`127.0.0.1:26760`)**
  * Direct UDP motion streaming for gyro aiming in emulators (**Ryujinx, Yuzu, Cemu, Dolphin, PCSX2, RPCS3, Citra**).
  * No external wrapper apps (like BetterJoy or JoyShockMapper) required.

* 🕹️ **Three Emulated Controller Modes with Instant Hot-Swapping**
  * **Xbox 360** (via ViGEmBus): Maximum compatibility with PC games.
  * **DualShock 4** (via ViGEmBus): PS4 gamepad emulation with touchpad click and lightbar support.
  * **Nintendo Switch Pro** (via Viiper): Native Switch Pro emulation with real-time battery status and system buttons (`Capture` & `Home`).
  * **Zero-delay hot-swapping**: Switch controller types on the fly from the system tray menu without reconnecting or interrupting games.

* 📳 **Full Haptic Rumble Across All Modes**
  * Advanced translation of dual-motor rumble and **Nintendo HD Rumble** frequency envelopes into realistic tactile waveforms on the Steam Controller's dual linear resonant actuators (LRA).

* 🧠 **Smart Steam Integration**
  * **Off only while in Steam game**: Use your Steam Controller on the desktop while Steam runs in the background. It automatically pauses only when a Steam game launches (`gameoverlayui.exe`) to prevent double-input conflicts.
  * Also supports *Off when Steam is running* and *Always on*.

* 🎯 **Desktop Controls & Auto-Profiles**
  * Smooth mouse trackball emulation on the right trackpad with haptic feedback.
  * Per-game profile switching based on the active foreground application.

---

## 📥 Installation & Setup

### 1. Requirements
* Windows 10 or 11 (64-bit)
* Steam Controller (connected via original USB Cable or Wireless Receiver Dongle)
* **[ViGEmBus](https://github.com/nefarius/ViGEmBus)** (for Xbox 360 & DualShock 4 emulation)
* **[Viiper](https://github.com/Viiper/Viiper)** (for Nintendo Switch Pro emulation)
* Visual C++ 2015–2022 Redistributable (x64)

> 💡 **Quick Install**: An automated installer script (`Instalar requisitos.cmd`) is included in the release zip to download and install all missing drivers with one click.

---

### 2. Getting Started

1. Download **`SteamlessControllerDSU.zip`** from the [Latest Release](https://github.com/YOUR_USERNAME/SteamlessControllerDSU/releases/latest).
2. Extract the folder to your preferred location (e.g. `C:\Games\SteamlessControllerDSU`).
3. If running for the first time, right-click **`Instalar requisitos.cmd`** and select **"Run as administrator"**.
4. Connect your Steam Controller and launch **`SteamlessController.exe`**.
5. The application will minimize to your system tray. Right-click the tray icon to switch profiles or toggle emulation.

---

## ⚙️ Emulator Motion Setup (DSU)

To use gyro aiming in your emulators:

```text
Protocol  : Cemuhook / DSU Client
Server IP : 127.0.0.1  (or localhost)
Port      : 26760
Slot      : 0          (or Auto-detect)
```

| Emulator | Where to configure |
| :--- | :--- |
| **Ryujinx** | `Options` ➔ `Settings` ➔ `Input` ➔ Enable Motion ➔ `DSU Client` (`127.0.0.1:26760`) |
| **Cemu** | `Options` ➔ `GamePad motion source` ➔ `DSU1` ➔ By slot |
| **Dolphin** | `Controllers` ➔ `Motion Sources` ➔ Add `127.0.0.1:26760` |
| **PCSX2** | `Settings` ➔ `Controllers` ➔ `Motion` ➔ `Enable DSU Motion Server` |

---

<details>
<summary><b>▶ 🇪🇸 Versión en Español (Haz clic aquí para expandir)</b></summary>

<br>

## SteamlessControllerDSU 🎮

Controlador independiente para el **Valve Steam Controller** en Windows que funciona **sin necesidad de abrir Steam**.

**SteamlessControllerDSU** emula mandos estándar (**Xbox 360**, **PlayStation 4** y **Nintendo Switch Pro**) e integra un **servidor de movimiento Cemuhook DSU** (`127.0.0.1:26760`) para apuntar con el giroscopio en emuladores modernos de forma nativa.

---

### 🌟 Características Principales

* 🌐 **Servidor de Movimiento Cemuhook DSU Integrado (`127.0.0.1:26760`)**
  * Transmisión directa por UDP de acelerómetro y giroscopio para emuladores (**Ryujinx, Yuzu, Cemu, Dolphin, PCSX2, RPCS3, Citra**).
  * Sin necesidad de software puente adicional como BetterJoy o JoyShockMapper.

* 🕹️ **Tres Modos de Emulación con Cambio en Caliente (0 ms)**
  * **Xbox 360** (vía ViGEmBus): Máxima compatibilidad con juegos de PC.
  * **DualShock 4** (vía ViGEmBus): Soporte de barra de luz y panel táctil.
  * **Nintendo Switch Pro** (vía Viiper): Emulación nativa de mando Switch Pro con reporte de batería y botones `Captura` y `Home`.
  * **Cambio instantáneo**: Alterna entre modos desde el icono de la bandeja del sistema sin desconectar el mando ni reiniciar tus juegos.

* 📳 **Vibración Háptica Completa en los 3 Modos**
  * Conversión avanzada de vibración de dos motores y del **HD Rumble** de Nintendo a ondas táctiles mediante los actuadores LRA del Steam Controller.

* 🧠 **Detección Inteligente de Steam**
  * **Off only while in Steam game**: Usa el mando en el escritorio mientras Steam está abierto en segundo plano. Solo se pausa automáticamente cuando se ejecuta un juego de Steam (`gameoverlayui.exe`) para evitar dobles entradas.
  * También disponible: *Off when Steam is running* y *Always on*.

* 🎯 **Modo Escritorio y Perfiles Automáticos**
  * Emulación de trackball en el trackpad derecho para mover el ratón con retroalimentación háptica.
  * Cambio automático de perfiles según la aplicación o juego en primer plano.

---

### 📥 Instalación y Uso Rápido

1. Descarga **`SteamlessControllerDSU.zip`** desde la sección de [Releases](https://github.com/YOUR_USERNAME/SteamlessControllerDSU/releases/latest).
2. Descomprime la carpeta donde prefieras.
3. Si es tu primera vez, haz clic derecho sobre **`Instalar requisitos.cmd`** y elige **"Ejecutar como administrador"** (descargará e instalará automáticamente ViGEmBus, Viiper y VCRedist).
4. Conecta tu Steam Controller (cable USB

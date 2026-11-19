# SteamlessController DSU Server 🎮

[![GitHub release](https://img.shields.io/github/v/release/ZaydelJmz/SteamlessController-DSU-Server?style=flat-square&color=blue)](https://github.com/ZaydelJmz/SteamlessController-DSU-Server/releases/latest)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%7C%2011%20(x64)-informational?style=flat-square)](https://github.com/ZaydelJmz/SteamlessController-DSU-Server)
[![Hardware](https://img.shields.io/badge/Hardware-Steam%20Controller%202-purple?style=flat-square)](https://github.com/ZaydelJmz/SteamlessController-DSU-Server)
[![License](https://img.shields.io/badge/License-GPLv3-green?style=flat-square)](LICENSE)

> **Notice:** This project is an enhanced standalone fork based on the original [SteamlessController](https://github.com/ddeverill/SteamlessController) by **ddeverill**, extending it for the **Steam Controller 2** with an integrated Cemuhook DSU motion server, Nintendo Switch (Pro Controller) & PlayStation 5 (DualSense) emulation via USBIP, streamlined platform switcher, and advanced haptic controls.

A standalone driver and input wrapper specifically tailored for the **Steam Controller 2** on Windows that operates completely **without the Steam client running**.

**SteamlessController DSU Server** unlocks full independence for your Steam Controller 2, emulating standard and modern gamepads (**Xbox 360 (XInput Controller)**, **PlayStation 4 (DualShock 4)**, **PlayStation 5 (DualSense)**, and **Nintendo Switch (Pro Controller)**) alongside an **integrated Cemuhook DSU motion server** (`127.0.0.1:26760`) for next-gen gyro aiming in emulators and PC games.

---

## 🌟 Key Features

* 🎮 **Full Steam Controller 2 Independence**
  * Complete driver and input wrapper running outside of Steam.
  * Direct support for dual trackpads, analog thumbstick, 4 back grip buttons / paddles, and wireless connectivity via "The Puck" (USB dongle), Bluetooth, or USB cable.

* 🎛️ **Quick Target Platform Selector**
  * Located directly on the system tray panel.
  * Features a **clean, full-width 4-button segmented bar** (`[ Xbox ] [ PS4 ] [ PS5 ] [ Switch ]`) with dedicated color accents:
    * **Xbox**: Emerald green (`#22c55e`)
    * **PS4**: Classic PlayStation blue (`#38bdf8`)
    * **PS5**: DualSense electric blue (`#60a5fa`)
    * **Switch**: Neon red (`#f43f5e`)
  * **Seamless 1000 ms Cooldown Switch**: Clean teardown and rebuild pumping Win32 messages with visual amber feedback (`Switching platform...`), ensuring Windows PnP assigns the new virtual pad strictly to **Player 1 / Gamepad 1 (Slot 0)**.

* 🕹️ **Four Emulated Controller Modes**
  * **Xbox 360 (XInput Controller)** (via ViGEmBus): Maximum out-of-the-box compatibility with Windows PC titles.
  * **PlayStation 4 (DualShock 4)** (via ViGEmBus): PS4 gamepad emulation with lightbar and native touchpad.
  * **PlayStation 5 (DualSense)** (via VIIPER + USBIP): 33-byte native report with dual-point touchpad, gyro/accel motion, and adaptive trigger feedback.
  * **Nintendo Switch (Pro Controller)** (via VIIPER + USBIP): 24-byte report with 0..4095 analog stick resolution, real-time battery reporting, system `Capture` & `Home` buttons, and native 6-axis gyro.

* 🌐 **Built-in Cemuhook DSU Motion Server (`127.0.0.1:26760`)**
  * Direct 6-axis IMU UDP motion streaming for gyro aiming in emulators (**Ryujinx, Yuzu, Cemu, Dolphin, PCSX2, RPCS3, Citra**).
  * No external bridge apps (such as BetterJoy or JoyShockMapper) required.

* 📳 **Advanced Haptics & Trackpad Movement Controls**
  * **Movement Haptics** toggle in Customize Controls to silence continuous trackpad gliding buzz while preserving crisp tactile click pulses.
  * Click pulses only trigger when a real action is bound; inert when set to 'None'.
  * Full translation of dual-motor rumble and **Nintendo HD Rumble** frequency envelopes into realistic tactile waveforms on the Steam Controller 2 LRA actuators.

* 🔋 **Intelligent Auto-Disable Sleep Watchdog (12s)**
  * When the controller powers off (timeout, battery empty, or button) while the wireless dongle stays plugged in, the app automatically transitions to **Off** after 12 seconds of silence.
  * Avoids frozen battery percentages and frees virtual controller slots. Reconnects automatically as soon as the controller powers back on.

* 🗺️ **Map Back button to Touchpad Click (PlayStation 4 & 5)**
  * Dedicated toggle in Customize Controls (`General` tab) mapping physical Back to Touchpad Click and vice versa.
  * Essential for cross-platform titles (such as *Fortnite*) where in-game maps or scoreboards are bound to the touchpad click on PlayStation platforms.

* 🧠 **Smart Steam Integration**
  * Keeps your desktop bindings active while Steam is running in the background.
  * Automatically and transparently pauses only when Steam launches a game (`gameoverlayui.exe`), avoiding double-input conflicts.

---

## 📥 Installation & Setup

### 1. Requirements
* Windows 10 or 11 (64-bit)
* Steam Controller 2 (connected via "The Puck" wireless dongle, Bluetooth, or USB cable)
* **[ViGEmBus](https://github.com/nefarius/ViGEmBus)** (for Xbox 360 (XInput Controller) & PlayStation 4 (DualShock 4) emulation)
* **[usbip-win2](https://github.com/cezanne/usbip-win2)** (for Nintendo Switch (Pro Controller) & PlayStation 5 (DualSense) emulation)
* Visual C++ 2015–2022 Redistributable (x64)

> 💡 **Quick Install**: An automated installer script (`Instalar requisitos.cmd`) is included in the release package to download and install all missing drivers with one click.

---

### 2. Getting Started

1. Download **`SteamlessDSUServer-0.1.6.zip`** from the [Latest Release](https://github.com/ZaydelJmz/SteamlessController-DSU-Server/releases/latest).
2. Extract the folder to your preferred location (e.g. `C:\Programas\SteamlessDSUServer-0.1.6`).
3. If running for the first time, right-click **`Instalar requisitos.cmd`** and select **"Run as administrator"**.
4. Restart Windows if installing drivers for the first time.
5. Connect your Steam Controller 2 and launch **`SteamlessController.exe`**.
6. The application minimizes to the notification area (system tray). Click the tray icon to switch platforms instantly via **"Choose Platform"** or customize controls.

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
| **RPCS3** | `Pads` ➔ `Motion Sensor` ➔ `DSU Client` (`127.0.0.1:26760`) |

---

## 🙏 Credits & Acknowledgments

* **[ddeverill/SteamlessController](https://github.com/ddeverill/SteamlessController)**: The original standalone utility that this project builds upon.
* **[david419kr/steamless-controller-XB-PS-NS](https://github.com/david419kr/steamless-controller-XB-PS-NS)**: Base implementation for Switch Pro support and VIIPER sidecar integration.
* **[Viiper](https://github.com/Viiper/Viiper)** & **usbip-win2**: Framework powering native Switch Pro and PS5 DualSense virtual emulation.
* **[ViGEmBus](https://github.com/nefarius/ViGEmBus)**: Virtual Gamepad Emulation Framework by Nefarius for Xbox 360 and DualShock 4 emulation.
* **[Cemuhook](https://cemuhook.sshnuke.net/)**: The open DSU UDP motion protocol specification.

---

## 📄 License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)** — see the [LICENSE](LICENSE) file for details.

---

<details>
<summary><b>▶ 🇪🇸 Versión en Español (Haz clic aquí para expandir)</b></summary>

<br>

## SteamlessController DSU Server 🎮

> **Aviso:** Este proyecto es un fork independiente basado en el [SteamlessController](https://github.com/ddeverill/SteamlessController) original de **ddeverill**, adaptándolo para el **Steam Controller 2** con un servidor de movimiento Cemuhook DSU integrado, emulación de Nintendo Switch (Pro Controller), PlayStation 5 (DualSense), PlayStation 4 (DualShock 4) y Xbox 360 (XInput Controller) vía ViGEmBus/USBIP, selector de plataformas rediseñado y controles hápticos avanzados.

Controlador independiente para el **Steam Controller 2** en Windows que funciona **sin necesidad de abrir Steam**.

**SteamlessController DSU Server** desbloquea la independencia total del Steam Controller 2, emulando mandos estándar y modernos (**Xbox 360 (XInput Controller)**, **PlayStation 4 (DualShock 4)**, **PlayStation 5 (DualSense)** y **Nintendo Switch (Pro Controller)**) e integrando un **servidor de movimiento Cemuhook DSU** (`127.0.0.1:26760`) para apuntar con el giroscopio en emuladores y juegos de PC.

---

### 🌟 Características Principales

* 🎮 **Soporte Completo e Independiente para Steam Controller 2**
  * Wrapper y controlador completo que funciona fuera de Steam.
  * Soporte para ambos trackpads hápticos, stick analógico, los botones traseros (L4/R4, L5/R5) y conectividad mediante "The Puck" (dongle USB), Bluetooth o cable USB.

* 🎛️ **Selector Rápido de Plataforma**
  * Ubicado directamente en el panel de la bandeja del sistema debajo de *Customize Controls*.
  * Presenta una **barra segmentada de 4 botones de ancho completo** (`[ Xbox ] [ PS4 ] [ PS5 ] [ Switch ]`) con acentos de color dedicados:
    * **Xbox**: Verde esmeralda (`#22c55e`)
    * **PS4**: Azul clásico PlayStation (`#38bdf8`)
    * **PS5**: Azul eléctrico DualSense (`#60a5fa`)
    * **Switch**: Rojo neón (`#f43f5e`)
  * **Transición fluida con Cooldown de 1000 ms**: Desconexión y reconexión limpia procesando mensajes Win32 con indicador visual ámbar (`Switching platform...`), garantizando que Windows asigne siempre el nuevo mando virtual estrictamente al **Jugador 1 / Mando 1 (Slot 0)**.

* 🕹️ **Cuatro Modos de Emulación Virtual**
  * **Xbox 360 (XInput Controller)** (vía ViGEmBus): Máxima compatibilidad con títulos de PC para Windows.
  * **PlayStation 4 (DualShock 4)** (vía ViGEmBus): Emulación de mando de PS4 con touchpad táctil y barra de luz nativa.
  * **PlayStation 5 (DualSense)** (vía VIIPER + USBIP): Reporte nativo de 33 bytes con touchpad de doble punto, giroscopio/acelerómetro y respuesta háptica en gatillos.
  * **Nintendo Switch (Pro Controller)** (vía VIIPER + USBIP): Reporte nativo de 24 bytes con resolución de stick 0..4095, reporte de batería en tiempo real, botones de sistema `Captura` y `Home`, y giroscopio nativo de 6 ejes.

* 🌐 **Servidor de Movimiento Cemuhook DSU Integrado (`127.0.0.1:26760`)**
  * Transmisión directa por UDP de acelerómetro y giroscopio de 6 ejes para emuladores (**Ryujinx, Yuzu, Cemu, Dolphin, PCSX2, RPCS3, Citra**).
  * Sin necesidad de software puente adicional como BetterJoy o JoyShockMapper.

* 📳 **Control de Hápticos y Desplazamiento**
  * Selector "Movement Haptics" para silenciar el zumbido continuo al deslizar el dedo por el trackpad sin perder la respuesta táctil al presionar clics reales.
  * Conversión de vibración de dos motores y del **HD Rumble** de Nintendo a impulsos táctiles LRA del Steam Controller 2.

* 🔋 **Watchdog de Inactividad y Apagado (12s)**
  * Si el control se apaga o se duerme con el dongle conectado, la app pasa a estado **Off** tras 12 segundos, liberando el slot virtual y evitando porcentajes de batería congelados. Al encenderse de nuevo, se reconecta automáticamente.

* 🗺️ **Mapear botón Back a Touchpad Click (PlayStation 4 y 5)**
  * Opción conmutable en Customize Controls (pestaña `General`) para vincular el botón Back físico del Steam Controller 2 con el clic del touchpad de PlayStation y viceversa.
  * Ideal para títulos multiplataforma (como *Fortnite*) donde abrir el mapa o ver la puntuación está ligado al clic del touchpad en consolas de PlayStation.

* 🧠 **Detección Inteligente de Steam**
  * El mando sigue funcionando en el escritorio aunque Steam esté abierto.
  * Se pausa únicamente cuando Steam inicia un juego (`gameoverlayui.exe`) para evitar dobles entradas.

---

### 📥 Instalación y Uso Rápido

1. Descarga **`SteamlessDSUServer-0.1.6.zip`** desde la sección de [Releases](https://github.com/ZaydelJmz/SteamlessController-DSU-Server/releases/latest).
2. Descomprime la carpeta donde prefieras.
3. Si es tu primera vez, haz clic derecho sobre **`Instalar requisitos.cmd`** y elige **"Ejecutar como administrador"**.
4. Reinicia el equipo si instalaste drivers nuevos.
5. Conecta tu Steam Controller 2 y abre **`SteamlessController.exe`**.
6. El programa se iniciará en la bandeja del sistema (junto al reloj de Windows). Haz clic en su icono para alternar de plataforma o configurar controles.

---

### ⚙️ Configurar Giroscopio en Emuladores

```text
Protocolo : Cemuhook / DSU Client
IP        : 127.0.0.1  (o localhost)
Puerto    : 26760
Slot      : 0          (o Automático)
```

---

### 🙏 Créditos y Reconocimientos

* **[ddeverill/SteamlessController](https://github.com/ddeverill/SteamlessController)**: El proyecto original sobre el cual se basa esta versión.
* **[david419kr/steamless-controller-XB-PS-NS](https://github.com/david419kr/steamless-controller-XB-PS-NS)**: Soporte base de Switch Pro y VIIPER.
* **[Viiper](https://github.com/Viiper/Viiper)** & **usbip-win2**: Framework de emulación USBIP para Switch Pro y DualSense.
* **[ViGEmBus](https://github.com/nefarius/ViGEmBus)**: Driver de Nefarius para mandos Xbox 360 y DualShock 4.
* **[Cemuhook](https://cemuhook.sshnuke.net/)**: Especificación del protocolo DSU sobre UDP.

</details>

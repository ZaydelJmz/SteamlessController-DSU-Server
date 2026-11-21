# SteamlessController DSU Server 🎮

[![GitHub release](https://img.shields.io/github/v/release/ZaydelJmz/SteamlessController-DSU-Server?style=flat-square&color=blue)](https://github.com/ZaydelJmz/SteamlessController-DSU-Server/releases/latest)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%7C%2011%20(x64)-informational?style=flat-square)](https://github.com/ZaydelJmz/SteamlessController-DSU-Server)
[![Hardware](https://img.shields.io/badge/Hardware-Steam%20Controller%202-purple?style=flat-square)](https://github.com/ZaydelJmz/SteamlessController-DSU-Server)
[![License](https://img.shields.io/badge/License-GPLv3-green?style=flat-square)](LICENSE)

> **Notice:** This project is an enhanced standalone fork based on the original [SteamlessController](https://github.com/ddeverill/SteamlessController) by **ddeverill**, extending it for the **Steam Controller 2** on Windows with an integrated DSU motion server (UDP server to stream the controller's gyroscope to emulators), Nintendo Switch (Pro Controller) & PlayStation 5 (DualSense) emulation via USBIP, streamlined platform switcher, and advanced haptic controls.

A standalone driver and input wrapper specifically tailored for the **Steam Controller 2** on Windows (10 and 11, 64-bit) that operates completely **without the Steam client running**.

**SteamlessController DSU Server** unlocks full independence for your Steam Controller 2 on Windows, emulating standard and modern gamepads (**Xbox 360 (XInput Controller)**, **PlayStation 4 (DualShock 4)**, **PlayStation 5 (DualSense)**, and **Nintendo Switch (Pro Controller)**) alongside an **integrated DSU motion server** (`127.0.0.1:26760`, UDP motion server that enables gyroscope aiming in emulators) for next-gen gyro aiming in emulators and PC games.

---

## 🌟 Key Features

* 🎮 **Full Steam Controller 2 Independence**
  * Complete driver and input wrapper for Windows running outside of Steam.
  * Direct support for dual trackpads, analog thumbstick, 4 back grip buttons / paddles, and wireless connectivity via "The Puck" (USB dongle), Bluetooth, or USB cable.

* 🎛️ **Quick Target Platform Selector**
  * Located directly on the system tray panel.
  * Features a **clean, full-width 4-button segmented bar** (`[ Xbox ] [ PS4 ] [ PS5 ] [ Switch ]`) with dedicated color accents.
  * **Seamless 1000 ms Cooldown Switch**: Clean teardown and rebuild pumping Win32 messages with visual amber feedback (`Switching platform...`), ensuring Windows PnP assigns the new virtual pad strictly to **Player 1 / Gamepad 1 (Slot 0)**.

* 🕹️ **Four Emulated Controller Modes**
  * **Xbox 360 (XInput Controller)** (via ViGEmBus): Maximum out-of-the-box compatibility with Windows PC titles.
  * **PlayStation 4 (DualShock 4)** (via ViGEmBus): PS4 gamepad emulation with lightbar and native touchpad.
  * **PlayStation 5 (DualSense)** (via VIIPER + USBIP): 33-byte native report with dual-point touchpad, gyro/accel motion, and adaptive trigger feedback.
  * **Nintendo Switch (Pro Controller)** (via VIIPER + USBIP): 24-byte report with 0..4095 analog stick resolution, real-time battery reporting, system `Capture` & `Home` buttons, and native 6-axis gyro.

* 🌐 **Built-in DSU Motion Server (`127.0.0.1:26760`)** *(Motion server to enable gyroscope aiming in emulators for controllers that lack native gyro)*
  * Direct 6-axis IMU UDP motion streaming for gyro aiming in emulators (**Ryujinx, Yuzu, Cemu, Dolphin, PCSX2, RPCS3, Citra**).
  * Allows motion aiming even when emulating controllers that natively lack motion support in Windows (such as Xbox 360 / XInput).
  * No external bridge apps (such as BetterJoy or JoyShockMapper) required.

* 🌐 **Built-in Language Selector (English / Español)**
  * Switch seamlessly between English (default) and Spanish directly from the tray panel or the Customize Controls window.

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

> 💡 **Quick Install**: An automated installer script (`Install Prerequisites.cmd`) is included in the release package to download and install all missing drivers with one click.

---

### 2. Getting Started

1. Download **`SteamlessDSUServer-0.1.8.zip`** from the [Latest Release](https://github.com/ZaydelJmz/SteamlessController-DSU-Server/releases/latest).
2. Extract the folder to your preferred location (e.g. `C:\Programas\SteamlessDSUServer-Latest` or `SteamlessDSUServer-0.1.8`).
3. If running for the first time, right-click **`Install Prerequisites.cmd`** and select **"Run as administrator"**.
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

> **Aviso:** Este proyecto es un fork independiente mejorado basado en el [SteamlessController](https://github.com/ddeverill/SteamlessController) original de **ddeverill**, ampliándolo para el **Steam Controller 2** en Windows con un servidor de movimiento DSU integrado (servidor UDP para transmitir el giroscopio del mando a emuladores), emulación de Nintendo Switch (Pro Controller) y PlayStation 5 (DualSense) vía USBIP, selector de plataformas optimizado y controles hápticos avanzados.

Controlador y wrapper de entrada independiente diseñado específicamente para el **Steam Controller 2** en Windows (10 y 11, 64 bits) que funciona completamente **sin necesidad del cliente de Steam en segundo plano**.

**SteamlessController DSU Server** desbloquea la independencia total de tu Steam Controller 2 en Windows, emulando mandos estándar y modernos (**Xbox 360 (XInput Controller)**, **PlayStation 4 (DualShock 4)**, **PlayStation 5 (DualSense)** y **Nintendo Switch (Pro Controller)**) junto a un **servidor de movimiento DSU integrado** (`127.0.0.1:26760`) para apuntado por giroscopio de última generación en emuladores y juegos de PC.

---

### 🌟 Características Principales

* 🎮 **Independencia Total del Steam Controller 2**
  * Controlador y wrapper de entrada completo para Windows que funciona fuera de Steam.
  * Soporte directo para ambos trackpads, stick analógico, los 4 botones traseros (palancas / grips) y conectividad inalámbrica mediante "The Puck" (dongle USB), Bluetooth o cable USB.

* 🎛️ **Selector Rápido de Plataforma**
  * Ubicado directamente en el panel de la bandeja del sistema.
  * Presenta una **barra segmentada de 4 botones de ancho completo** (`[ Xbox ] [ PS4 ] [ PS5 ] [ Switch ]`) con acentos de color dedicados.
  * **Transición fluida con Cooldown de 1000 ms**: Desconexión y recreación limpia procesando mensajes Win32 con indicador visual ámbar (`Switching platform...`), garantizando que Windows PnP asigne siempre el nuevo mando virtual estrictamente al **Jugador 1 / Mando 1 (Slot 0)**.

* 🕹️ **Cuatro Modos de Mando Emulados**
  * **Xbox 360 (XInput Controller)** (vía ViGEmBus): Máxima compatibilidad directa con títulos de PC para Windows.
  * **PlayStation 4 (DualShock 4)** (vía ViGEmBus): Emulación de mando de PS4 con barra de luz y panel táctil nativo.
  * **PlayStation 5 (DualSense)** (vía VIIPER + USBIP): Reporte nativo de 33 bytes con panel táctil de doble punto, movimiento giroscopio/acelerómetro y respuesta háptica en gatillos adaptativos.
  * **Nintendo Switch (Pro Controller)** (vía VIIPER + USBIP): Reporte nativo de 24 bytes con resolución analógica de 0..4095, reporte de batería en tiempo real, botones de sistema `Captura` y `Home`, y giroscopio nativo de 6 ejes.

* 🌐 **Servidor de Movimiento DSU Integrado (`127.0.0.1:26760`)** *(Servidor para habilitar giroscopio en emuladores incluso en modos de mando que no lo soportan de forma nativa)*
  * Transmisión directa por UDP de movimiento IMU de 6 ejes para apuntar con giroscopio en emuladores (**Ryujinx, Yuzu, Cemu, Dolphin, PCSX2, RPCS3, Citra**).
  * Permite apuntar con giroscopio incluso al emular mandos que en Windows no soportan movimiento de forma nativa (como Xbox 360 / XInput).
  * Sin necesidad de aplicaciones puente externas (como BetterJoy o JoyShockMapper).

* 🌐 **Selector de Idioma Integrado (English / Español)**
  * Alterna de forma fluida e instantánea entre inglés (por defecto) y español directamente desde el panel de la bandeja o la ventana de Personalizar Controles.

* 📳 **Controles Hápticos Avanzados y Movimiento de Trackpad**
  * Interruptor **Movement Haptics** en Customize Controls para silenciar el zumbido continuo al deslizar el dedo por el trackpad, preservando pulsos táctiles nítidos al hacer clic.
  * Los pulsos de clic solo se activan cuando hay una acción real asignada; permanecen inertes al configurarse en 'None'.
  * Traducción completa de vibración de dos motores y envolventes de frecuencia de **Nintendo HD Rumble** en formas de onda táctiles realistas sobre los actuadores LRA del Steam Controller 2.

* 🔋 **Watchdog Inteligente de Inactividad y Suspensión (12s)**
  * Cuando el mando se apaga (por tiempo de espera, batería agotada o botón) mientras el dongle inalámbrico sigue conectado, la app pasa automáticamente a **Off** tras 12 segundos de silencio.
  * Evita porcentajes de batería congelados y libera ranuras virtuales de mando. Se reconecta automáticamente tan pronto como el mando se vuelve a encender.

* 🗺️ **Mapear botón Back a Touchpad Click (PlayStation 4 y 5)**
  * Opción conmutable en Customize Controls (pestaña `General`) que vincula el botón Back físico del Steam Controller 2 con el clic del panel táctil de PlayStation y viceversa.
  * Esencial para títulos multiplataforma (como *Fortnite*) donde los mapas o marcadores dentro del juego están asignados al clic del panel táctil en consolas PlayStation.

* 🧠 **Integración Inteligente con Steam**
  * Mantiene activos tus mapeos de escritorio mientras Steam se ejecuta en segundo plano.
  * Se pausa automática y transparentemente solo cuando Steam inicia un juego (`gameoverlayui.exe`), evitando conflictos de doble entrada.

---

### 📥 Instalación y Configuración

#### 1. Requisitos
* Windows 10 u 11 (64 bits)
* Steam Controller 2 (conectado mediante el dongle inalámbrico "The Puck", Bluetooth o cable USB)
* **[ViGEmBus](https://github.com/nefarius/ViGEmBus)** (para emulación de Xbox 360 (XInput Controller) y PlayStation 4 (DualShock 4))
* **[usbip-win2](https://github.com/cezanne/usbip-win2)** (para emulación de Nintendo Switch (Pro Controller) y PlayStation 5 (DualSense))
* Visual C++ 2015–2022 Redistributable (x64)

> 💡 **Instalación Rápida**: Se incluye un script instalador automatizado (`Install Prerequisites.cmd`) en el paquete de lanzamiento para descargar e instalar todos los controladores faltantes con un solo clic.

---

#### 2. Primeros Pasos

1. Descarga **`SteamlessDSUServer-0.1.8.zip`** desde la [Última Release](https://github.com/ZaydelJmz/SteamlessController-DSU-Server/releases/latest).
2. Descomprime la carpeta en tu ubicación preferida (ej. `C:\Programas\SteamlessDSUServer-Latest` o `SteamlessDSUServer-0.1.8`).
3. Si es tu primera vez, haz clic derecho sobre **`Install Prerequisites.cmd`** y selecciona **"Ejecutar como administrador"**.
4. Reinicia Windows si es la primera vez que instalas los controladores.
5. Conecta tu Steam Controller 2 y abre **`SteamlessController.exe`**.
6. La aplicación se minimizará en el área de notificación (bandeja del sistema). Haz clic en el icono de la bandeja para cambiar de plataforma al instante mediante **"Choose Platform"** o personalizar controles.

---

### ⚙️ Configuración del Giroscopio en Emuladores (DSU)

Para utilizar el apuntado por giroscopio en tus emuladores:

```text
Protocolo : Cemuhook / DSU Client
IP Servidor: 127.0.0.1  (o localhost)
Puerto    : 26760
Ranura    : 0          (o Detección automática)
```

| Emulador | Dónde configurar |
| :--- | :--- |
| **Ryujinx** | `Options` ➔ `Settings` ➔ `Input` ➔ Enable Motion ➔ `DSU Client` (`127.0.0.1:26760`) |
| **Cemu** | `Options` ➔ `GamePad motion source` ➔ `DSU1` ➔ By slot |
| **Dolphin** | `Controllers` ➔ `Motion Sources` ➔ Añadir `127.0.0.1:26760` |
| **PCSX2** | `Settings` ➔ `Controllers` ➔ `Motion` ➔ `Enable DSU Motion Server` |
| **RPCS3** | `Pads` ➔ `Motion Sensor` ➔ `DSU Client` (`127.0.0.1:26760`) |

---

### 🙏 Créditos y Reconocimientos

* **[ddeverill/SteamlessController](https://github.com/ddeverill/SteamlessController)**: La utilidad autónoma original sobre la cual se construye este proyecto.
* **[david419kr/steamless-controller-XB-PS-NS](https://github.com/david419kr/steamless-controller-XB-PS-NS)**: Implementación base para el soporte de Switch Pro e integración del sidecar VIIPER.
* **[Viiper](https://github.com/Viiper/Viiper)** y **usbip-win2**: Framework que impulsa la emulación virtual nativa de Switch Pro y PS5 DualSense.
* **[ViGEmBus](https://github.com/nefarius/ViGEmBus)**: Framework de emulación virtual de mandos de Nefarius para emulación de Xbox 360 y DualShock 4.
* **[Cemuhook](https://cemuhook.sshnuke.net/)**: Especificación abierta del protocolo de movimiento UDP DSU.

---

### 📄 Licencia

Este proyecto está licenciado bajo la **Licencia Pública General de GNU v3.0 (GPLv3)** — consulta el archivo [LICENSE](LICENSE) para más detalles.

</details>

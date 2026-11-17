# 🎮 SteamlessController + Switch Pro & Gyro DSU

[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%2F%2011%20x64-blue.svg)](https://github.com/)
[![License: MIT / GPLv3](https://img.shields.io/badge/License-MIT%20%2F%20GPLv3-green.svg)](LICENSE)
[![Release](https://img.shields.io/badge/Release-v1.0.0--NS-orange.svg)](https://github.com/)

A modern, enhanced fork of **[SteamlessController](https://github.com/ddeverill/SteamlessController)** adding native **Nintendo Switch Pro Controller emulation** (with motion and HD Rumble), an **embedded Cemuhook DSU motion server**, and seamless hot-swapping between controller modes.

Use your physical Steam Controller as a native gamepad on PC emulators and non-Steam games without running Steam in the background and without third-party scripts.

---

## ✨ Features

- 🎯 **3 Virtual Controller Modes**:
  - **Nintendo Switch Pro Controller**: Native emulation via VIIPER USB driver. Full native 6-axis IMU gyro and HD Rumble conversion straight into emulators like Ryujinx, Cemu, Liichi, and Yuzu.
  - **Sony DualShock 4**: Native gyro, touchpad navigation, and dual-motor rumble via ViGEmBus.
  - **Xbox 360**: Universal XInput compatibility with custom haptic rumble curves adapted to Steam Controller LRA actuators.
- 📡 **Built-in Cemuhook DSU Server (`127.0.0.1:26760`)**:
  - Motion streaming is integrated directly inside the executable. **No need to run `sc2gyrodsu.exe` or background PowerShell scripts!**
  - **Smart auto-switching**:
    - **Xbox 360 Mode**: DSU streams active motion data to `127.0.0.1:26760` simultaneously while delivering XInput buttons.
    - **Desktop / Lizard Mode (Steamless OFF)**: Streams gyro motion in the background while leaving desktop mouse/keyboard hardware untouched.
    - **Switch Pro / PS4 Modes**: DSU motion automatically pauses to prioritize native driver gyro and prevent input overlaps.
- ⚡ **Seamless Hot-Swapping**:
  - Change controller platforms on the fly without disconnecting the physical controller or causing driver stalls.
- 🖱️ **Full Desktop & Trackpad Control**:
  - Customizable trackpads (mouse pointer, directional pad, scroll wheel, joystick).
  - Back-grip paddles binding and per-game auto profiles.
- 🛠️ **1-Click Safe Prerequisite Installer**:
  - Automated PowerShell script that verifies and installs ViGEmBus, compatible `usbip-win2`, and VC++ runtimes with official SHA-256 checks.

---

## 🚀 Quick Start

### 1. Requirements
- Windows 10 or 11 (64-bit)
- Official Steam Controller (Wireless Dongle or USB Cable)

### 2. Installation
1. Go to **[Releases](../../releases)** and download `SteamlessController-NS.zip`.
2. Extract the folder anywhere on your PC (e.g. `C:\Programs\SteamlessController-NS`).
3. Right-click **`Instalar requisitos.cmd`** and select **Run as Administrator**.
   - It will automatically download and install *ViGEmBus*, *usbip-win2 (v0.9.7.7)*, and *Visual C++ Redistributable* only if missing.
4. **Reboot your PC** (required for virtual USB drivers to load).
5. Run **`SteamlessController.exe`**. The app will live in your Windows system tray.

---

## 🎯 Gyro & Motion Guide for Emulators

| Target Mode in Steamless | Gyro in Emulators (Ryujinx, Cemu, Dolphin, etc.) | Setup Needed |
| :--- | :--- | :--- |
| **Nintendo Switch Pro** | **Native Hardware Gyro** | None. In the emulator, select "Nintendo Switch Pro Controller" as an SDL/DirectInput device. Motion works out of the box. |
| **PlayStation 4 (DS4)** | **Native DS4 Gyro** | None. Select "DualShock 4" input in emulator settings. |
| **Xbox 360** | **Cemuhook DSU Server** | In emulator Motion settings, add DSU server: `127.0.0.1:26760`. |
| **Steamless Mode OFF** | **Cemuhook DSU Server** | Same as above. Steam Controller functions as desktop mouse while streaming gyro to `127.0.0.1:26760`. |

---

## 📂 Project Structure

```text
SteamlessController-NS/
├── SteamlessController.exe      # Main GUI & embedded DSU Cemuhook server
├── viiper.exe                   # Virtual Switch Pro sidecar server (VIIPER v0.6.1-steamless8)
├── SteamlessDeviceCycle.exe     # PnP device reclamation helper
├── ViGEmBusProbe.exe            # Driver integrity diagnostic probe
├── Instalar requisitos.cmd      # 1-click prerequisite installer
├── install-prereqs.ps1          # Cryptographically verified driver installer script
└── LEEME.txt                    # Documentation & user manual (Spanish)
```

---

## 🤝 Credits & Acknowledgments

- **[ddeverill/SteamlessController](https://github.com/ddeverill/SteamlessController)**: The original SteamlessController project.
- **[Alia5/VIIPER](https://github.com/Alia5/VIIPER)**: Virtual USB device sidecar providing the Switch Pro emulation layer.
- **[Nefarius](https://github.com/nefarius/ViGEmBus)**: ViGEmBus virtual gamepad framework.
- **[Cemuhook](https://cemuhook.sshnuke.net/)**: Cemuhook UDP DSU motion protocol.
- **[SteamController2GyroDSU-Win](https://github.com/Visualtaggy/SteamController2GyroDSU-Win)**: Gyro mapping and calibration reference.

---

## 📜 License

- SteamlessController code is licensed under the [MIT License](LICENSE).
- `viiper.exe` and its patches are licensed under the [GNU General Public License v3.0](VIIPER-LICENSE.txt).

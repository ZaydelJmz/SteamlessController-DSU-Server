#include "I18n.h"

namespace {

Language g_setting = Language::English;
bool g_initialized = false;

constexpr wchar_t kRegKey[] = L"Software\\SteamlessController";
constexpr wchar_t kRegVal[] = L"Language";

struct Strings {
    const wchar_t* w;
    const char* u8;
};

#define ENTRY(wstr, ustr) { L##wstr, ustr }

const Strings kEn[] = {
    // SteamlessMode
    ENTRY("Steamless Mode", "Steamless Mode"),
    // StatusActive
    ENTRY("Active", "Active"),
    // StatusOff
    ENTRY("Off", "Off"),
    // StatusSwitching
    ENTRY("Switching platform...", "Switching platform..."),
    // StatusNoController
    ENTRY("No controller detected", "No controller detected"),
    // StatusUnavailable
    ENTRY("Controller unavailable", "Controller unavailable"),
    // TargetPlatform
    ENTRY("TARGET PLATFORM", "TARGET PLATFORM"),
    // DsuServer
    ENTRY("DSU SERVER (CEMUHOOK)", "DSU SERVER (CEMUHOOK)"),
    // Copied
    ENTRY("✓ Copied", "✓ Copied"),
    // CustomizeControls
    ENTRY("Customize Controls", "Customize Controls"),
    // CustomizeSub
    ENTRY("Bindings, trackpads, triggers, sticks", "Bindings, trackpads, triggers, sticks"),
    // StartWithWindows
    ENTRY("Start with Windows", "Start with Windows"),
    // ShowNotifications
    ENTRY("Show Notifications", "Show Notifications"),
    // LanguageRow
    ENTRY("Language", "Language"),
    // EventLog
    ENTRY("Event log", "Event log"),
    // Exit
    ENTRY("Exit", "Exit"),
    // DevSwitchedOff
    ENTRY("Controller device is switched off in Windows.", "Controller device is switched off in Windows."),
    // ReEnable
    ENTRY("Re-enable", "Re-enable"),
    // BatteryPercent
    ENTRY("battery", "battery"),

    // BalloonPlatformChanged
    ENTRY("Platform Changed", "Platform Changed"),
    // BalloonEmulating
    ENTRY("Emulating %ls controller (Player 1)", "Emulating %ls controller (Player 1)"),
    // BalloonSteamPaused
    ENTRY("Steam is running a game. Steamless is paused.", "Steam is running a game. Steamless is paused."),
    // BalloonNoProfilesTitle
    ENTRY("No game profiles yet", "No game profiles yet"),
    // BalloonNoProfilesBody
    ENTRY("The controller stays in its own keyboard/mouse mode until you add a profile for a game in Customize Controls.",
          "The controller stays in its own keyboard/mouse mode until you add a profile for a game in Customize Controls."),

    // LanguageLabel
    ENTRY("Language", "Language"),
    // LangEnglish
    ENTRY("English", "English"),
    // LangSpanish
    ENTRY("Español", "Español"),
};

const Strings kEs[] = {
    // SteamlessMode
    ENTRY("Modo Steamless", "Modo Steamless"),
    // StatusActive
    ENTRY("Activo", "Activo"),
    // StatusOff
    ENTRY("Desactivado", "Desactivado"),
    // StatusSwitching
    ENTRY("Cambiando de plataforma...", "Cambiando de plataforma..."),
    // StatusNoController
    ENTRY("Mando no detectado", "Mando no detectado"),
    // StatusUnavailable
    ENTRY("Mando no disponible", "Mando no disponible"),
    // TargetPlatform
    ENTRY("PLATAFORMA DESTINO", "PLATAFORMA DESTINO"),
    // DsuServer
    ENTRY("SERVIDOR DSU (CEMUHOOK)", "SERVIDOR DSU (CEMUHOOK)"),
    // Copied
    ENTRY("✓ Copiado", "✓ Copiado"),
    // CustomizeControls
    ENTRY("Personalizar Controles", "Personalizar Controles"),
    // CustomizeSub
    ENTRY("Mapeos, trackpads, gatillos, sticks", "Mapeos, trackpads, gatillos, sticks"),
    // StartWithWindows
    ENTRY("Iniciar con Windows", "Iniciar con Windows"),
    // ShowNotifications
    ENTRY("Mostrar notificaciones", "Mostrar notificaciones"),
    // LanguageRow
    ENTRY("Idioma", "Idioma"),
    // EventLog
    ENTRY("Registro de eventos", "Registro de eventos"),
    // Exit
    ENTRY("Salir", "Salir"),
    // DevSwitchedOff
    ENTRY("El mando está desactivado en Windows.", "El mando está desactivado en Windows."),
    // ReEnable
    ENTRY("Reactivar", "Reactivar"),
    // BatteryPercent
    ENTRY("batería", "batería"),

    // BalloonPlatformChanged
    ENTRY("Plataforma cambiada", "Plataforma cambiada"),
    // BalloonEmulating
    ENTRY("Emulando mando de %ls (Jugador 1)", "Emulando mando de %ls (Jugador 1)"),
    // BalloonSteamPaused
    ENTRY("Steam está ejecutando un juego. Steamless está pausado.", "Steam está ejecutando un juego. Steamless está pausado."),
    // BalloonNoProfilesTitle
    ENTRY("Aún no hay perfiles de juegos", "Aún no hay perfiles de juegos"),
    // BalloonNoProfilesBody
    ENTRY("El mando permanece en su modo teclado/ratón hasta que agregues un perfil de juego en Personalizar Controles.",
          "El mando permanece en su modo teclado/ratón hasta que agregues un perfil de juego en Personalizar Controles."),

    // LanguageLabel
    ENTRY("Idioma", "Idioma"),
    // LangEnglish
    ENTRY("English", "English"),
    // LangSpanish
    ENTRY("Español", "Español"),
};

#undef ENTRY

} // namespace

namespace I18n {

void Init() {
    if (g_initialized) return;
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRegKey, 0, KEY_READ, &key) == ERROR_SUCCESS) {
        DWORD val = 0;
        DWORD size = sizeof(val);
        if (RegQueryValueExW(key, kRegVal, nullptr, nullptr, reinterpret_cast<LPBYTE>(&val), &size) == ERROR_SUCCESS) {
            if (val == 1) {
                g_setting = Language::Spanish;
            } else {
                g_setting = Language::English;
            }
        }
        RegCloseKey(key);
    }
    g_initialized = true;
}

Language GetSetting() {
    Init();
    return g_setting;
}

Language GetEffective() {
    Init();
    return g_setting;
}

void SetSetting(Language lang) {
    g_setting = lang;
    g_initialized = true;
    HKEY key = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kRegKey, 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr) == ERROR_SUCCESS) {
        DWORD val = (lang == Language::Spanish) ? 1 : 0;
        RegSetValueExW(key, kRegVal, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&val), sizeof(val));
        RegCloseKey(key);
    }
}

void ToggleSetting() {
    Init();
    SetSetting(g_setting == Language::Spanish ? Language::English : Language::Spanish);
}

const wchar_t* Text(TextId id) {
    size_t idx = static_cast<size_t>(id);
    if (GetEffective() == Language::Spanish) {
        if (idx < sizeof(kEs) / sizeof(kEs[0])) return kEs[idx].w;
    }
    if (idx < sizeof(kEn) / sizeof(kEn[0])) return kEn[idx].w;
    return L"";
}

const char* TextUtf8(TextId id) {
    size_t idx = static_cast<size_t>(id);
    if (GetEffective() == Language::Spanish) {
        if (idx < sizeof(kEs) / sizeof(kEs[0])) return kEs[idx].u8;
    }
    if (idx < sizeof(kEn) / sizeof(kEn[0])) return kEn[idx].u8;
    return "";
}

const char* GetLangCode() {
    return (GetEffective() == Language::Spanish) ? "es" : "en";
}

} // namespace I18n

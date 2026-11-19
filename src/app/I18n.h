#pragma once
#include <windows.h>
#include <string>

enum class Language {
    Auto = 0,
    English = 1,
    Spanish = 2
};

enum class TextId {
    // Tray Panel
    SteamlessMode,
    StatusActive,
    StatusOff,
    StatusSwitching,
    StatusNoController,
    StatusUnavailable,
    TargetPlatform,
    CustomizeControls,
    CustomizeSub,
    StartWithWindows,
    ShowNotifications,
    LanguageRow,
    EventLog,
    Exit,
    DevSwitchedOff,
    ReEnable,
    BatteryPercent,

    // Balloon notifications
    BalloonPlatformChanged,
    BalloonEmulating,
    BalloonSteamPaused,
    BalloonNoProfilesTitle,
    BalloonNoProfilesBody,

    // Language names
    LanguageLabel,
    LangAuto,
    LangEnglish,
    LangSpanish,
};

namespace I18n {

void Init();
Language GetSetting();
Language GetEffective();
void SetSetting(Language lang);

const wchar_t* Text(TextId id);
const char* TextUtf8(TextId id);

const char* GetLangCode(); // "en" or "es"

} // namespace I18n

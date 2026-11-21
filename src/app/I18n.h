#pragma once
#include <windows.h>
#include <string>

enum class Language {
    English = 0,
    Spanish = 1
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
    DsuServer,
    Copied,
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
    LangEnglish,
    LangSpanish,
};

namespace I18n {

void Init();
Language GetSetting();
Language GetEffective();
void SetSetting(Language lang);
void ToggleSetting();

const wchar_t* Text(TextId id);
const char* TextUtf8(TextId id);
const char* GetLangCode();

} // namespace I18n

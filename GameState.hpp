#pragma once

enum class GameState {
    MainMenu,
    SaveSlotSelect,
    Playing,
    Settings,
    Paused
};

enum class MenuAction {
    NewGame,
    Continue,
    Settings,
    Quit,
    None
};
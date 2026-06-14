#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Settings {
public:
    Settings(sf::RenderWindow& window, sf::Font& font);
    bool handleEvent(const sf::Event& event); // 返回 true 表示退出设置
    void draw();

    int getMusicVolume() const { return musicVolume; }
    int getSfxVolume() const { return sfxVolume; }
    bool getFullscreen() const { return fullscreen; }

private:
    sf::RenderWindow& window;
    sf::Font& font;
    int selectedIndex = 0;
    int musicVolume = 80;
    int sfxVolume = 100;
    bool fullscreen = false;

    struct SettingItem {
        std::string name;
        std::string getValue(const Settings& s) const;
        int index;
    };
};
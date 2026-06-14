#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "SaveSystem.hpp"

class SlotSelect {
public:
    SlotSelect(sf::RenderWindow& window, sf::Font& font, bool isSaving);
    int handleEvent(const sf::Event& event); // 返回槽位 0-7，-1取消，-2无操作
    void update(float dt);
    void draw();

private:
    sf::RenderWindow& window;
    sf::Font& font;
    bool isSaving;
    int selectedSlot = 0;
    float animTimer = 0.f;
    const int SLOT_COUNT = 8;
};
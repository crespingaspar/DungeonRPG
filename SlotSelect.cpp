#include "SlotSelect.hpp"
#include <sstream>

SlotSelect::SlotSelect(sf::RenderWindow& window, sf::Font& font, bool isSaving)
    : window(window), font(font), isSaving(isSaving) {}

int SlotSelect::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::W ||
            event.key.code == sf::Keyboard::Up)
            selectedSlot = (selectedSlot - 1 + SLOT_COUNT) % SLOT_COUNT;
        if (event.key.code == sf::Keyboard::S ||
            event.key.code == sf::Keyboard::Down)
            selectedSlot = (selectedSlot + 1) % SLOT_COUNT;
        if (event.key.code == sf::Keyboard::Return)
            return selectedSlot;
        if (event.key.code == sf::Keyboard::Escape)
            return -1;
    }
    return -2;
}

void SlotSelect::update(float dt) {
    animTimer += dt;
}

void SlotSelect::draw() {
    // 背景
    sf::RectangleShape bg(sf::Vector2f(800, 600));
    bg.setFillColor(sf::Color(10, 10, 25));
    window.draw(bg);

    // 标题
    sf::Text title;
    title.setFont(font);
    title.setString(isSaving ? "SAVE GAME" : "LOAD GAME");
    title.setCharacterSize(40);
    title.setFillColor(sf::Color(180, 100, 255));
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.width / 2.f, tb.height / 2.f);
    title.setPosition(400, 50);
    window.draw(title);

    // 8个存档槽
    for (int i = 0; i < SLOT_COUNT; i++) {
        bool selected = (i == selectedSlot);
        float x = (i % 2 == 0) ? 150.f : 470.f;
        float y = 120.f + (i / 2) * 100.f;

        // 槽位背景
        sf::RectangleShape slot(sf::Vector2f(280, 80));
        slot.setPosition(x, y);
        slot.setFillColor(selected ? sf::Color(80, 40, 120, 180) : sf::Color(30, 30, 50, 180));
        slot.setOutlineColor(selected ? sf::Color(180, 100, 255) : sf::Color(60, 60, 90));
        slot.setOutlineThickness(selected ? 2.f : 1.f);
        window.draw(slot);

        // 存档文件名
        std::string filename = "save_" + std::to_string(i) + ".dat";
        bool exists = SaveSystem::hasSave(filename);

        // 槽位编号
        sf::Text numText;
        numText.setFont(font);
        numText.setString("Slot " + std::to_string(i + 1));
        numText.setCharacterSize(18);
        numText.setFillColor(selected ? sf::Color(220, 180, 255) : sf::Color(140, 120, 180));
        numText.setPosition(x + 10, y + 8);
        window.draw(numText);

        // 存档信息
        sf::Text infoText;
        infoText.setFont(font);
        infoText.setCharacterSize(14);

        if (exists) {
            GameSaveData data;
            SaveSystem::load(data, filename);
            std::ostringstream ss;
            ss << "Floor " << data.floor
               << "  Lv." << data.playerLevel
               << "  HP:" << data.playerHP << "/" << data.playerMaxHP;
            infoText.setString(ss.str());
            infoText.setFillColor(sf::Color(150, 200, 150));
        } else {
            infoText.setString("-- Empty --");
            infoText.setFillColor(sf::Color(80, 80, 100));
        }
        infoText.setPosition(x + 10, y + 48);
        window.draw(infoText);
    }

    // 操作提示
    sf::Text hint;
    hint.setFont(font);
    hint.setString("[W/S] Select    [Enter] Confirm    [ESC] Back");
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(100, 100, 140));
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin(hb.width / 2.f, hb.height / 2.f);
    hint.setPosition(400, 565);
    window.draw(hint);
}
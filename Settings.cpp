#include "Settings.hpp"

Settings::Settings(sf::RenderWindow &window, sf::Font &font)
    : window(window), font(font) {}

bool Settings::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Escape)
            return true;
        if (event.key.code == sf::Keyboard::W ||
            event.key.code == sf::Keyboard::Up)
            selectedIndex = (selectedIndex - 1 + 3) % 3;
        if (event.key.code == sf::Keyboard::S ||
            event.key.code == sf::Keyboard::Down)
            selectedIndex = (selectedIndex + 1) % 3;

        if (event.key.code == sf::Keyboard::Left)
        {
            if (selectedIndex == 0)
                musicVolume = std::max(0, musicVolume - 10);
            if (selectedIndex == 1)
                sfxVolume = std::max(0, sfxVolume - 10);
        }
        if (event.key.code == sf::Keyboard::Right)
        {
            if (selectedIndex == 0)
                musicVolume = std::min(100, musicVolume + 10);
            if (selectedIndex == 1)
                sfxVolume = std::min(100, sfxVolume + 10);
        }
        if (event.key.code == sf::Keyboard::Return)
        {
            if (selectedIndex == 2)
                fullscreen = !fullscreen;
        }
    }
    return false;
}

void Settings::draw()
{
    sf::RectangleShape bg(sf::Vector2f(800, 600));
    bg.setFillColor(sf::Color(10, 10, 25));
    window.draw(bg);

    sf::Text title;
    title.setFont(font);
    title.setString("SETTINGS");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color(180, 100, 255));
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.width / 2.f, tb.height / 2.f);
    title.setPosition(400, 80);
    window.draw(title);

    struct Item
    {
        std::string name;
        std::string value;
    };
    std::vector<Item> items = {
        {"音乐音量", std::to_string(musicVolume) + "%"},
        {"音效音量", std::to_string(sfxVolume) + "%"},
        {"全屏模式", fullscreen ? "开启" : "关闭"}};
    for (int i = 0; i < (int)items.size(); i++)
    {
        bool selected = (i == selectedIndex);
        float y = 220.f + i * 90.f;

        sf::RectangleShape row(sf::Vector2f(500, 60));
        row.setOrigin(250.f, 30.f);
        row.setPosition(400, y);
        row.setFillColor(selected ? sf::Color(80, 40, 120, 160) : sf::Color(30, 30, 50));
        row.setOutlineColor(selected ? sf::Color(180, 100, 255) : sf::Color(60, 60, 90));
        row.setOutlineThickness(selected ? 2.f : 1.f);
        window.draw(row);

        sf::Text nameText;
        nameText.setFont(font);
        nameText.setString(items[i].name);
        nameText.setCharacterSize(22);
        nameText.setFillColor(selected ? sf::Color(220, 180, 255) : sf::Color(160, 140, 200));
        nameText.setPosition(170, y - 14);
        window.draw(nameText);

        // 左右箭头 + 值
        if (i < 2)
        {
            sf::Text arrow;
            arrow.setFont(font);
            arrow.setCharacterSize(22);
            arrow.setFillColor(sf::Color(180, 100, 255));
            arrow.setString("<");
            arrow.setPosition(490, y - 14);
            window.draw(arrow);
            arrow.setString(">");
            arrow.setPosition(600, y - 14);
            window.draw(arrow);
        }

        sf::Text valText;
        valText.setFont(font);
        valText.setString(items[i].value);
        valText.setCharacterSize(22);
        valText.setFillColor(sf::Color(100, 220, 180));
        sf::FloatRect vb = valText.getLocalBounds();
        valText.setOrigin(vb.width / 2.f, vb.height / 2.f);
        valText.setPosition(548, y);
        window.draw(valText);
    }

    // 进度条（音量）
    for (int i = 0; i < 2; i++)
    {
        float val = (i == 0 ? musicVolume : sfxVolume) / 100.f;
        float y = 220.f + i * 90.f + 18.f;

        sf::RectangleShape barBg(sf::Vector2f(200, 6));
        barBg.setPosition(170, y + 24);
        barBg.setFillColor(sf::Color(60, 40, 80));
        window.draw(barBg);

        sf::RectangleShape bar(sf::Vector2f(200 * val, 6));
        bar.setPosition(170, y + 24);
        bar.setFillColor(sf::Color(180, 100, 255));
        window.draw(bar);
    }

    sf::Text hint;
    hint.setFont(font);
    hint.setString("[W/S] 选择    [左/右] 调整    [ESC] 返回");
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(100, 100, 140));
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin(hb.width / 2.f, hb.height / 2.f);
    hint.setPosition(400, 565);
    window.draw(hint);
}
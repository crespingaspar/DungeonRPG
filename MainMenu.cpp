#include "MainMenu.hpp"
#include <cmath>
#include <random>

static std::mt19937 menuRng(std::random_device{}());

MainMenu::MainMenu(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font) {
    initParticles();
}

void MainMenu::initParticles() {
    std::uniform_real_distribution<float> px(0, 800);
    std::uniform_real_distribution<float> py(0, 600);
    std::uniform_real_distribution<float> vx(-20.f, 20.f);
    std::uniform_real_distribution<float> vy(-30.f, -10.f);
    std::uniform_real_distribution<float> sz(1.f, 3.f);

    particles.resize(60);
    for (auto& p : particles) {
        p.pos = sf::Vector2f(px(menuRng), py(menuRng));
        p.vel = sf::Vector2f(vx(menuRng), vy(menuRng));
        p.size = sz(menuRng);
        p.alpha = std::uniform_real_distribution<float>(50.f, 200.f)(menuRng);
    }
}

void MainMenu::updateParticles(float dt) {
    std::uniform_real_distribution<float> px(0, 800);
    std::uniform_real_distribution<float> vx(-20.f, 20.f);
    std::uniform_real_distribution<float> vy(-30.f, -10.f);

    for (auto& p : particles) {
        p.pos += p.vel * dt;
        p.alpha -= 30.f * dt;
        if (p.alpha <= 0.f || p.pos.y < 0.f) {
            p.pos = sf::Vector2f(px(menuRng), 620.f);
            p.vel = sf::Vector2f(vx(menuRng), vy(menuRng));
            p.alpha = std::uniform_real_distribution<float>(100.f, 200.f)(menuRng);
        }
    }
}

void MainMenu::drawParticles() {
    for (auto& p : particles) {
        sf::CircleShape c(p.size);
        c.setOrigin(p.size, p.size);
        c.setPosition(p.pos);
        c.setFillColor(sf::Color(100, 150, 255, (sf::Uint8)p.alpha));
        window.draw(c);
    }
}

void MainMenu::drawBackground() {
    // 渐变背景
    sf::RectangleShape bg(sf::Vector2f(800, 600));
    bg.setFillColor(sf::Color(10, 10, 25));
    window.draw(bg);

    // 网格线
    for (int i = 0; i < 20; i++) {
        sf::RectangleShape line(sf::Vector2f(800, 1));
        line.setPosition(0, i * 32);
        line.setFillColor(sf::Color(30, 30, 60, 80));
        window.draw(line);
    }
    for (int i = 0; i < 25; i++) {
        sf::RectangleShape line(sf::Vector2f(1, 600));
        line.setPosition(i * 32, 0);
        line.setFillColor(sf::Color(30, 30, 60, 80));
        window.draw(line);
    }

    drawParticles();
}

void MainMenu::drawTitle() {
    // 标题阴影
    sf::Text shadow;
    shadow.setFont(font);
    shadow.setString("DUNGEON RPG");
    shadow.setCharacterSize(64);
    shadow.setFillColor(sf::Color(80, 0, 120, 180));
    sf::FloatRect sb = shadow.getLocalBounds();
    shadow.setOrigin(sb.width / 2.f, sb.height / 2.f);
    shadow.setPosition(404, 124);
    window.draw(shadow);

    // 主标题
    sf::Text title;
    title.setFont(font);
    title.setString("DUNGEON RPG");
    title.setCharacterSize(64);

    // 颜色随时间变化
    float r = 150 + 100 * std::sin(animTimer * 1.5f);
    float g = 50 + 30 * std::sin(animTimer * 0.8f);
    float b = 220 + 35 * std::sin(animTimer * 2.0f);
    title.setFillColor(sf::Color((sf::Uint8)r, (sf::Uint8)g, (sf::Uint8)b));

    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.width / 2.f, tb.height / 2.f);
    title.setPosition(400, 120);
    window.draw(title);

    // 副标题
    sf::Text sub;
    sub.setFont(font);
    sub.setString("- Explore the Depths -");
    sub.setCharacterSize(18);
    sub.setFillColor(sf::Color(160, 130, 200));
    sf::FloatRect subb = sub.getLocalBounds();
    sub.setOrigin(subb.width / 2.f, subb.height / 2.f);
    sub.setPosition(400, 175);
    window.draw(sub);
}

void MainMenu::drawMenuItems() {
    bool hasSave = SaveSystem::hasSave();

    for (int i = 0; i < (int)items.size(); i++) {
        bool selected = (i == selectedIndex);
        bool disabled = (i == 1 && !hasSave); // Continue 没存档时禁用

        float y = 260.f + i * 72.f;

        // 选中背景
        if (selected) {
            sf::RectangleShape bg(sf::Vector2f(300, 54));
            bg.setOrigin(150.f, 27.f);
            bg.setPosition(400, y);
            bg.setFillColor(sf::Color(80, 40, 120, 160));
            bg.setOutlineColor(sf::Color(180, 100, 255, 200));
            bg.setOutlineThickness(2.f);
            window.draw(bg);

            // 左右箭头
            sf::Text arrow;
            arrow.setFont(font);
            arrow.setCharacterSize(24);
            arrow.setFillColor(sf::Color(180, 100, 255));
            float pulse = 1.f + 0.1f * std::sin(animTimer * 4.f);
            arrow.setString(">");
            arrow.setPosition(230 - 20 * pulse, y - 14);
            window.draw(arrow);
            arrow.setString("<");
            arrow.setPosition(550 + 20 * pulse, y - 14);
            window.draw(arrow);
        }

        // 菜单文字
        sf::Text text;
        text.setFont(font);
        text.setString(items[i]);
        text.setCharacterSize(32);

        if (disabled)
            text.setFillColor(sf::Color(80, 80, 80));
        else if (selected)
            text.setFillColor(sf::Color(220, 180, 255));
        else
            text.setFillColor(sf::Color(160, 140, 200));

        sf::FloatRect tb = text.getLocalBounds();
        text.setOrigin(tb.width / 2.f, tb.height / 2.f);
        text.setPosition(400, y);
        window.draw(text);
    }

    // 操作提示
    sf::Text hint;
    hint.setFont(font);
    hint.setString("[W/S] or [Arrow] Navigate    [Enter] Select");
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(100, 100, 140));
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin(hb.width / 2.f, hb.height / 2.f);
    hint.setPosition(400, 570);
    window.draw(hint);
}

MenuAction MainMenu::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::W ||
            event.key.code == sf::Keyboard::Up) {
            selectedIndex = (selectedIndex - 1 + items.size()) % items.size();
        }
        if (event.key.code == sf::Keyboard::S ||
            event.key.code == sf::Keyboard::Down) {
            selectedIndex = (selectedIndex + 1) % items.size();
        }
        if (event.key.code == sf::Keyboard::Return) {
            bool hasSave = SaveSystem::hasSave();
            switch (selectedIndex) {
                case 0: return MenuAction::NewGame;
                case 1: return hasSave ? MenuAction::Continue : MenuAction::None;
                case 2: return MenuAction::Settings;
                case 3: return MenuAction::Quit;
            }
        }
    }
    return MenuAction::None;
}

void MainMenu::update(float dt) {
    animTimer += dt;
    updateParticles(dt);
}

void MainMenu::draw() {
    drawBackground();
    drawTitle();
    drawMenuItems();
}
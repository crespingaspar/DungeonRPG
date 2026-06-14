#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "GameState.hpp"
#include "SaveSystem.hpp"

class MainMenu
{
public:
    MainMenu(sf::RenderWindow &window, sf::Font &font);
    MenuAction handleEvent(const sf::Event &event);
    void update(float dt);
    void draw();

private:
    void drawTitle();
    void drawMenuItems();
    void drawBackground();

    sf::RenderWindow &window;
    sf::Font &font;

    std::vector<std::string> items = {
        "开始游戏", "继续游戏", "游戏设置", "退出游戏"};
    int selectedIndex = 0;
    float animTimer = 0.f;

    // 背景粒子
    struct Particle
    {
        sf::Vector2f pos;
        sf::Vector2f vel;
        float alpha = 255.f;
        float size = 2.f;
    };
    std::vector<Particle> particles;
    void initParticles();
    void updateParticles(float dt);
    void drawParticles();
};
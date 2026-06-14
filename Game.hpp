#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Map.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"
#include "Item.hpp"
#include "Boss.hpp"
#include "Minimap.hpp"
#include "SaveSystem.hpp"
#include "EnemyTypes.hpp"

class Game {
public:
    Game(sf::RenderWindow& window, sf::Font& font);
    bool handleEvent(const sf::Event& event); // 返回 true 表示退出到主菜单
    void update(float dt);
    void render();

    void newGame();
    void saveToSlot(const std::string& filename);
    void loadFromSlot(const std::string& filename);

private:
    void drawHUD();
    void spawnEnemies();
    void spawnItems();
    void handleCombat(float dt);
    void handleShooting(float dt);
    void handleItems();
    void handleBoss(float dt);
    void handleSkills();
    void checkFloorClear(float dt);
    void nextFloor();
    sf::Vector2f getCamera();

    sf::RenderWindow& window;
    sf::Font& font;
    bool fontLoaded = true;

    Map map;
    Player player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<Bullet> bullets;
    std::vector<Bullet> bossBullets;
    std::vector<Item> items;
    std::unique_ptr<Boss> boss;
    Minimap minimap;

    struct DamageText {
        sf::Text text;
        sf::Vector2f pos;
        float lifetime = 1.0f;
        float vy = -60.f;
    };
    std::vector<DamageText> damageTexts;

    float playerAttackCooldown = 0.f;
    const float PLAYER_ATTACK_RATE = 0.3f;
    int playerAttackPower = 20;

    int currentFloor = 1;
    bool floorCleared = false;
    float floorClearTimer = 0.f;
    bool bossSpawned = false;

    bool wantsToQuit = false;
};
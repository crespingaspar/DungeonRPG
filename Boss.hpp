#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "Bullet.hpp"
#include <vector>

enum class BossState {
    Idle,
    Phase1,  // 普通追击 + 单发子弹
    Phase2,  // 快速 + 扇形子弹
    Dead
};

class Boss {
public:
    Boss(float x, float y);
    void update(float dt, const Map& map, sf::Vector2f playerPos,
                std::vector<Bullet>& bullets);
    void draw(sf::RenderWindow& window, sf::Vector2f cameraOffset, sf::Vector2f playerPos);
    void takeDamage(int dmg);
    bool isDead() const { return hp <= 0; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    int getExpReward() const { return 300; }
    bool canAttack() const { return meleeCooldown <= 0.f; }
    int getMeleeDamage() const { return 25; }
    void resetMeleeCooldown() { meleeCooldown = 1.5f; }

private:
    void shootBullet(sf::Vector2f dir, std::vector<Bullet>& bullets);
    void shootSpread(sf::Vector2f playerPos, std::vector<Bullet>& bullets);

    sf::RectangleShape shape;
    sf::RectangleShape hpBarBg;
    sf::RectangleShape hpBar;
    sf::Text nameText;

    BossState state = BossState::Phase1;
    int hp, maxHp;
    float speed = 60.f;

    float shootCooldown = 0.f;
    float meleeCooldown = 0.f;
    float phaseTimer = 0.f;

    bool phase2Triggered = false;
    float flashTimer = 0.f;
};
#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "Skill.hpp"
#include <vector>
#include "SaveSystem.hpp"
class Player
{
public:
    Player();
    void setPosition(float x, float y);
    void handleInput(float dt, const Map &map);
    void draw(sf::RenderWindow &window, sf::Vector2f cameraOffset);
    void takeDamage(int dmg);
    void heal(int amount);
    void addSpeed(float s);
    void reset();
    bool isDead() const { return hp <= 0; }
    GameSaveData getSaveData() const;
    void loadSaveData(const GameSaveData &data);
    int getExp() const { return exp; }
    int getExpToNext() const { return expToNext; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    int getHP() const { return hp; }
    int getMaxHP() const { return maxHp; }
    int getLevel() const { return level; }
    float getSpeed() const { return speed; }
    void addExp(int exp);

    // 技能
    bool tryDash(sf::Vector2f dir);
    bool tryMultiShot(sf::Vector2f targetPos, std::vector<Bullet> &bullets, int dmg);
    bool tryShieldBurst();
    bool hasShield() const { return shieldActive; }
    void updateSkills(float dt);
    const std::vector<Skill> &getSkills() const { return skills; }

    bool isDashing() const { return dashTimer > 0.f; }

private:
    sf::RectangleShape shape;
    sf::RectangleShape shieldShape;

    float speed = 180.f;
    int hp = 100;
    int maxHp = 100;
    int level = 1;
    int exp = 0;
    int expToNext = 50;
    int attackPower = 15;

    // 冲刺
    float dashTimer = 0.f;
    float dashSpeed = 600.f;
    sf::Vector2f dashDir;

    // 护盾
    bool shieldActive = false;
    float shieldTimer = 0.f;

    std::vector<Skill> skills;
};
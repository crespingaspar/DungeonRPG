#pragma once
#include <SFML/Graphics.hpp>
#include "Bullet.hpp"
#include <vector>
#include <functional>

enum class SkillType {
    Dash,        // Q：瞬间冲刺
    MultiShot,   // E：三连发
    ShieldBurst  // F：护盾爆炸，推开周围敌人
};

struct Skill {
    SkillType type;
    std::string name;
    sf::Keyboard::Key key;
    float cooldown;
    float currentCooldown = 0.f;
    bool isReady() const { return currentCooldown <= 0.f; }
    void use() { currentCooldown = cooldown; }
    void update(float dt) { if (currentCooldown > 0.f) currentCooldown -= dt; }
};
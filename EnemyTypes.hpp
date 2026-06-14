#pragma once
#include "Enemy.hpp"
#include "Bullet.hpp"
#include <vector>

// 快速型敌人：速度快血少
class FastEnemy : public Enemy {
public:
    FastEnemy(float x, float y, int level = 1) : Enemy(x, y, level) {
        shape.setFillColor(sf::Color(50, 220, 150));
        shape.setSize(sf::Vector2f(TILE_SIZE - 10, TILE_SIZE - 10));
        shape.setOrigin(shape.getSize() / 2.f);
        speed = 160.f + level * 10.f;
        chaseSpeed = 220.f + level * 12.f;
        hp = maxHp = 15 + level * 5;
        attackPower = 5 + level * 2;
        expReward = 15 + level * 8;
    }
};

// 坦克型敌人：血多速度慢
class TankEnemy : public Enemy {
public:
    TankEnemy(float x, float y, int level = 1) : Enemy(x, y, level) {
        shape.setFillColor(sf::Color(150, 80, 200));
        shape.setSize(sf::Vector2f(TILE_SIZE + 4, TILE_SIZE + 4));
        shape.setOrigin(shape.getSize() / 2.f);
        speed = 40.f + level * 3.f;
        chaseSpeed = 70.f + level * 5.f;
        hp = maxHp = 100 + level * 30;
        attackPower = 20 + level * 5;
        expReward = 40 + level * 15;
    }
};
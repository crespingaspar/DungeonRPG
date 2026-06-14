#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"

enum class EnemyState {
    Patrol,   // 巡逻
    Chase,    // 追击
    Attack    // 攻击
};

class Enemy {
public:
    Enemy(float x, float y, int level = 1);
    void update(float dt, const Map& map, sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window, sf::Vector2f cameraOffset);
    void takeDamage(int dmg);
    bool isDead() const { return hp <= 0; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    int getExpReward() const { return expReward; }
    bool canAttack() const;
    int getAttackPower() const { return attackPower; }
    void resetAttackCooldown();

protected:
    void updatePatrol(float dt, const Map& map);
    void updateChase(float dt, const Map& map, sf::Vector2f playerPos);
    float distanceTo(sf::Vector2f target) const;

    sf::RectangleShape shape;
    sf::RectangleShape hpBar;

    EnemyState state = EnemyState::Patrol;
    sf::Vector2f patrolTarget;
    float speed = 80.f;
    float chaseSpeed = 130.f;

    int hp;
    int maxHp;
    int attackPower;
    int expReward;

    float attackCooldown = 0.f;
    const float ATTACK_RATE = 1.0f;
    const float DETECT_RANGE = 180.f;
    const float ATTACK_RANGE = 36.f;
    const float LOSE_RANGE  = 300.f;

    float patrolTimer = 0.f;
    const float PATROL_INTERVAL = 2.5f;
};
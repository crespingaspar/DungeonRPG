#include "Enemy.hpp"
#include <cmath>
#include <random>

static std::mt19937 rng(std::random_device{}());

Enemy::Enemy(float x, float y, int level) {
    hp = maxHp = 30 + level * 10;
    attackPower = 8 + level * 3;
    expReward = 20 + level * 10;
    speed = 80.f + level * 5.f;
    chaseSpeed = 130.f + level * 8.f;

    shape.setSize(sf::Vector2f(TILE_SIZE - 6, TILE_SIZE - 6));
    shape.setOrigin(shape.getSize() / 2.f);
    shape.setFillColor(sf::Color(220, 60, 60));
    shape.setPosition(x, y);

    hpBar.setSize(sf::Vector2f(TILE_SIZE, 4));
    hpBar.setFillColor(sf::Color(200, 50, 50));

    // 初始巡逻目标
    patrolTarget = sf::Vector2f(x, y);
}

float Enemy::distanceTo(sf::Vector2f target) const {
    sf::Vector2f d = target - shape.getPosition();
    return std::sqrt(d.x * d.x + d.y * d.y);
}

bool Enemy::canAttack() const {
    return attackCooldown <= 0.f;
}

void Enemy::resetAttackCooldown() {
    attackCooldown = ATTACK_RATE;
}

void Enemy::takeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
}

void Enemy::updatePatrol(float dt, const Map& map) {
    patrolTimer -= dt;
    if (patrolTimer <= 0.f) {
        patrolTimer = PATROL_INTERVAL;
        // 随机选一个新巡逻目标
        std::uniform_int_distribution<int> dist(-3, 3);
        float tx = shape.getPosition().x + dist(rng) * TILE_SIZE;
        float ty = shape.getPosition().y + dist(rng) * TILE_SIZE;
        int tileX = (int)(tx / TILE_SIZE);
        int tileY = (int)(ty / TILE_SIZE);
        if (map.isWalkable(tileX, tileY))
            patrolTarget = sf::Vector2f(tx, ty);
    }

    // 移向巡逻目标
    sf::Vector2f dir = patrolTarget - shape.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 4.f) {
        dir /= len;
        sf::Vector2f pos = shape.getPosition();
        float newX = pos.x + dir.x * speed * dt;
        float newY = pos.y + dir.y * speed * dt;
        if (map.isWalkable((int)(newX / TILE_SIZE), (int)(pos.y / TILE_SIZE)))
            pos.x = newX;
        if (map.isWalkable((int)(pos.x / TILE_SIZE), (int)(newY / TILE_SIZE)))
            pos.y = newY;
        shape.setPosition(pos);
    }
}

void Enemy::updateChase(float dt, const Map& map, sf::Vector2f playerPos) {
    sf::Vector2f dir = playerPos - shape.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 1.f) {
        dir /= len;
        sf::Vector2f pos = shape.getPosition();
        float newX = pos.x + dir.x * chaseSpeed * dt;
        float newY = pos.y + dir.y * chaseSpeed * dt;
        if (map.isWalkable((int)(newX / TILE_SIZE), (int)(pos.y / TILE_SIZE)))
            pos.x = newX;
        if (map.isWalkable((int)(pos.x / TILE_SIZE), (int)(newY / TILE_SIZE)))
            pos.y = newY;
        shape.setPosition(pos);
    }
}

void Enemy::update(float dt, const Map& map, sf::Vector2f playerPos) {
    if (attackCooldown > 0.f) attackCooldown -= dt;

    float dist = distanceTo(playerPos);

    // 状态切换
    if (dist <= ATTACK_RANGE) {
        state = EnemyState::Attack;
    } else if (dist <= DETECT_RANGE) {
        state = EnemyState::Chase;
    } else if (dist > LOSE_RANGE) {
        state = EnemyState::Patrol;
    }

    switch (state) {
        case EnemyState::Patrol:
            shape.setFillColor(sf::Color(220, 60, 60));
            updatePatrol(dt, map);
            break;
        case EnemyState::Chase:
            shape.setFillColor(sf::Color(255, 100, 0));  // 橙色：发现玩家
            updateChase(dt, map, playerPos);
            break;
        case EnemyState::Attack:
            shape.setFillColor(sf::Color(255, 30, 30));  // 亮红：攻击范围
            updateChase(dt, map, playerPos);
            break;
    }

    // 更新血条
    float ratio = (float)hp / maxHp;
    hpBar.setSize(sf::Vector2f(TILE_SIZE * ratio, 4));
}

void Enemy::draw(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    sf::RectangleShape s = shape;
    s.setPosition(shape.getPosition() - cameraOffset);
    window.draw(s);

    // 血条背景
    sf::RectangleShape bg(sf::Vector2f(TILE_SIZE, 4));
    bg.setPosition(shape.getPosition().x - TILE_SIZE / 2.f - cameraOffset.x,
                   shape.getPosition().y - TILE_SIZE / 2.f - 6 - cameraOffset.y);
    bg.setFillColor(sf::Color(80, 0, 0));
    window.draw(bg);

    // 血条
    sf::RectangleShape bar = hpBar;
    bar.setPosition(shape.getPosition().x - TILE_SIZE / 2.f - cameraOffset.x,
                    shape.getPosition().y - TILE_SIZE / 2.f - 6 - cameraOffset.y);
    window.draw(bar);
}
#include "Boss.hpp"
#include <cmath>

Boss::Boss(float x, float y) {
    hp = maxHp = 500;

    shape.setSize(sf::Vector2f(52, 52));
    shape.setOrigin(26.f, 26.f);
    shape.setFillColor(sf::Color(180, 0, 220));
    shape.setOutlineColor(sf::Color(255, 100, 255));
    shape.setOutlineThickness(2.f);
    shape.setPosition(x, y);
}

void Boss::takeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
    flashTimer = 0.1f;

    // 进入第二阶段
    if (!phase2Triggered && hp < maxHp / 2) {
        phase2Triggered = true;
        state = BossState::Phase2;
        speed = 110.f;
        shape.setFillColor(sf::Color(220, 0, 80));
        shape.setOutlineColor(sf::Color(255, 50, 50));
    }
}

void Boss::shootBullet(sf::Vector2f dir, std::vector<Bullet>& bullets) {
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0) dir /= len;
    Bullet b(shape.getPosition(), dir, 12);
    bullets.push_back(b);
}

void Boss::shootSpread(sf::Vector2f playerPos, std::vector<Bullet>& bullets) {
    sf::Vector2f dir = playerPos - shape.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0) dir /= len;

    float baseAngle = std::atan2(dir.y, dir.x);
    int count = (state == BossState::Phase2) ? 7 : 3;
    float spread = 3.14159f / 6.f;

    for (int i = 0; i < count; i++) {
        float angle = baseAngle - spread + (i * spread * 2.f / (count - 1));
        sf::Vector2f d(std::cos(angle), std::sin(angle));
        Bullet b(shape.getPosition(), d, 10);
        bullets.push_back(b);
    }
}

void Boss::update(float dt, const Map& map, sf::Vector2f playerPos,
                  std::vector<Bullet>& bullets) {
    if (isDead()) return;

    if (shootCooldown > 0.f) shootCooldown -= dt;
    if (meleeCooldown > 0.f) meleeCooldown -= dt;
    if (flashTimer > 0.f) {
        flashTimer -= dt;
        shape.setOutlineThickness(flashTimer > 0 ? 4.f : 2.f);
    }

    phaseTimer += dt;

    // 追击玩家
    sf::Vector2f dir = playerPos - shape.getPosition();
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    if (dist > 40.f) {
        dir /= dist;
        sf::Vector2f pos = shape.getPosition();
        float newX = pos.x + dir.x * speed * dt;
        float newY = pos.y + dir.y * speed * dt;
        if (map.isWalkable((int)(newX / TILE_SIZE), (int)(pos.y / TILE_SIZE)))
            pos.x = newX;
        if (map.isWalkable((int)(pos.x / TILE_SIZE), (int)(newY / TILE_SIZE)))
            pos.y = newY;
        shape.setPosition(pos);
    }

    // 射击
    if (shootCooldown <= 0.f) {
        if (state == BossState::Phase1) {
            if ((int)(phaseTimer) % 3 == 0)
                shootSpread(playerPos, bullets);
            else
                shootBullet(playerPos - shape.getPosition(), bullets);
            shootCooldown = 1.2f;
        } else {
            shootSpread(playerPos, bullets);
            shootCooldown = 0.7f;
        }
    }
}

void Boss::draw(sf::RenderWindow& window, sf::Vector2f cameraOffset, sf::Vector2f playerPos) {
    if (isDead()) return;

    sf::RectangleShape s = shape;
    s.setPosition(shape.getPosition() - cameraOffset);
    window.draw(s);

    // 只有玩家靠近 300 像素才显示血条
    sf::Vector2f diff = shape.getPosition() - playerPos;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (dist > 300.f) return;

    float ratio = (float)hp / maxHp;

    sf::RectangleShape bg(sf::Vector2f(400, 20));
    bg.setPosition(200, 565);
    bg.setFillColor(sf::Color(60, 0, 60));
    window.draw(bg);

    sf::RectangleShape bar(sf::Vector2f(400 * ratio, 20));
    bar.setPosition(200, 565);
    bar.setFillColor(state == BossState::Phase2 ?
        sf::Color(220, 30, 30) : sf::Color(180, 0, 220));
    window.draw(bar);

    // Boss 名字
    // （字体在 Game 里，这里只画血条）
}
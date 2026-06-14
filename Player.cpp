#include "Player.hpp"
#include <cmath>

Player::Player() {
    shape.setSize(sf::Vector2f(TILE_SIZE - 4, TILE_SIZE - 4));
    shape.setFillColor(sf::Color(100, 180, 255));
    shape.setOrigin(shape.getSize() / 2.f);

    shieldShape.setSize(sf::Vector2f(TILE_SIZE + 8, TILE_SIZE + 8));
    shieldShape.setFillColor(sf::Color(100, 200, 255, 60));
    shieldShape.setOutlineColor(sf::Color(100, 200, 255, 180));
    shieldShape.setOutlineThickness(2.f);
    shieldShape.setOrigin(shieldShape.getSize() / 2.f);

    // 初始化技能
    skills.push_back({SkillType::Dash,        "Dash",         sf::Keyboard::Q, 3.f});
    skills.push_back({SkillType::MultiShot,   "MultiShot",    sf::Keyboard::E, 4.f});
    skills.push_back({SkillType::ShieldBurst, "ShieldBurst",  sf::Keyboard::F, 8.f});
}

void Player::setPosition(float x, float y) {
    shape.setPosition(x, y);
}

void Player::updateSkills(float dt) {
    for (auto& s : skills) s.update(dt);
    if (shieldTimer > 0.f) {
        shieldTimer -= dt;
        if (shieldTimer <= 0.f) shieldActive = false;
    }
}

bool Player::tryDash(sf::Vector2f dir) {
    if (!skills[0].isReady()) return false;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 0.1f) dir = sf::Vector2f(1.f, 0.f);
    else dir /= len;
    dashDir = dir;
    dashTimer = 0.15f;
    skills[0].use();
    return true;
}

bool Player::tryMultiShot(sf::Vector2f targetPos, std::vector<Bullet>& bullets, int dmg) {
    if (!skills[1].isReady()) return false;
    sf::Vector2f pos = shape.getPosition();
    sf::Vector2f dir = targetPos - pos;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0) dir /= len;

    float baseAngle = std::atan2(dir.y, dir.x);
    float spread = 0.25f;
    for (int i = -1; i <= 1; i++) {
        float angle = baseAngle + i * spread;
        sf::Vector2f d(std::cos(angle), std::sin(angle));
        bullets.emplace_back(pos, d, dmg);
    }
    skills[1].use();
    return true;
}

bool Player::tryShieldBurst() {
    if (!skills[2].isReady()) return false;
    shieldActive = true;
    shieldTimer = 5.f;
    skills[2].use();
    return true;
}

void Player::handleInput(float dt, const Map& map) {
    // 冲刺中
    if (dashTimer > 0.f) {
        dashTimer -= dt;
        sf::Vector2f pos = shape.getPosition();
        float newX = pos.x + dashDir.x * dashSpeed * dt;
        float newY = pos.y + dashDir.y * dashSpeed * dt;
        if (map.isWalkable((int)(newX / TILE_SIZE), (int)(pos.y / TILE_SIZE))) pos.x = newX;
        if (map.isWalkable((int)(pos.x / TILE_SIZE), (int)(newY / TILE_SIZE))) pos.y = newY;
        shape.setPosition(pos);
        return;
    }

    sf::Vector2f move(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    move.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  move.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  move.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) move.x += 1.f;

    if (move.x != 0 && move.y != 0) { move.x *= 0.707f; move.y *= 0.707f; }

    sf::Vector2f pos = shape.getPosition();
    float newX = pos.x + move.x * speed * dt;
    int tileX = (int)(newX / TILE_SIZE);
    int tileY = (int)(pos.y / TILE_SIZE);
    if (map.isWalkable(tileX, tileY)) pos.x = newX;

    float newY = pos.y + move.y * speed * dt;
    tileX = (int)(pos.x / TILE_SIZE);
    tileY = (int)(newY / TILE_SIZE);
    if (map.isWalkable(tileX, tileY)) pos.y = newY;
    shape.setPosition(pos);
}

void Player::draw(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    if (shieldActive) {
        shieldShape.setPosition(shape.getPosition() - cameraOffset);
        window.draw(shieldShape);
    }
    sf::RectangleShape s = shape;
    s.setPosition(shape.getPosition() - cameraOffset);
    // 冲刺时变色
    if (dashTimer > 0.f) s.setFillColor(sf::Color(200, 240, 255));
    window.draw(s);
}

void Player::takeDamage(int dmg) {
    if (shieldActive) { dmg = dmg / 3; } // 护盾减伤
    hp -= dmg;
    if (hp < 0) hp = 0;
}

void Player::heal(int amount) {
    hp += amount;
    if (hp > maxHp) hp = maxHp;
}

void Player::addSpeed(float s) { speed += s; }

void Player::addExp(int e) {
    exp += e;
    if (exp >= expToNext) {
        exp -= expToNext;
        level++;
        expToNext = (int)(expToNext * 1.5f);
        maxHp += 20;
        hp = maxHp;
        attackPower += 5;
    }
}

void Player::reset() {
    hp = maxHp = 100;
    exp = 0;
    level = 1;
    expToNext = 50;
    attackPower = 15;
    speed = 180.f;
    dashTimer = 0.f;
    shieldActive = false;
    shieldTimer = 0.f;
    for (auto& s : skills) s.currentCooldown = 0.f;
}
GameSaveData Player::getSaveData() const {
    GameSaveData data;
    data.playerHP = hp;
    data.playerMaxHP = maxHp;
    data.playerLevel = level;
    data.playerExp = exp;
    data.playerExpToNext = expToNext;
    data.playerSpeed = speed;
    data.playerX = shape.getPosition().x;
    data.playerY = shape.getPosition().y;
    return data;
}

void Player::loadSaveData(const GameSaveData& data) {
    hp = data.playerHP;
    maxHp = data.playerMaxHP;
    level = data.playerLevel;
    exp = data.playerExp;
    expToNext = data.playerExpToNext;
    speed = data.playerSpeed;
    shape.setPosition(data.playerX, data.playerY);
}
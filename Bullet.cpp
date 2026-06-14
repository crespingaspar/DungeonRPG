#include "Bullet.hpp"

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f dir, int damage)
    : damage(damage) {
    shape.setRadius(5.f);
    shape.setOrigin(5.f, 5.f);
    shape.setFillColor(sf::Color(255, 220, 50));
    shape.setPosition(pos);
    velocity = dir * speed;
}

void Bullet::update(float dt, const Map& map) {
    if (!alive) return;

    lifetime -= dt;
    if (lifetime <= 0.f) { alive = false; return; }

    sf::Vector2f pos = shape.getPosition();
    pos += velocity * dt;

    // 碰墙消失
    int tileX = (int)(pos.x / TILE_SIZE);
    int tileY = (int)(pos.y / TILE_SIZE);
    if (!map.isWalkable(tileX, tileY)) {
        alive = false;
        return;
    }

    shape.setPosition(pos);
}

void Bullet::draw(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    if (!alive) return;
    sf::CircleShape s = shape;
    s.setPosition(shape.getPosition() - cameraOffset);
    window.draw(s);
}
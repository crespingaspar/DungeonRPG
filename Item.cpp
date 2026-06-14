#include "Item.hpp"

Item::Item(float x, float y, ItemType type) : type(type), baseY(y) {
    shape.setRadius(10.f);
    shape.setOrigin(10.f, 10.f);
    shape.setPosition(x, y);

    switch (type) {
        case ItemType::HealthPotion:
            shape.setFillColor(sf::Color(220, 50, 80));
            break;
        case ItemType::PowerUp:
            shape.setFillColor(sf::Color(255, 180, 0));
            break;
        case ItemType::SpeedUp:
            shape.setFillColor(sf::Color(50, 220, 180));
            break;
    }
}

void Item::draw(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    if (collected) return;

    // 上下浮动动画
    bobTimer += 0.05f;
    sf::Vector2f pos = shape.getPosition();
    pos.y = baseY + std::sin(bobTimer) * 4.f;
    shape.setPosition(pos);

    sf::CircleShape s = shape;
    s.setPosition(shape.getPosition() - cameraOffset);

    // 外圈发光效果
    sf::CircleShape glow(14.f);
    glow.setOrigin(14.f, 14.f);
    glow.setPosition(s.getPosition());
    glow.setFillColor(sf::Color(255, 255, 255, 40));
    window.draw(glow);
    window.draw(s);
}
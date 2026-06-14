#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"

enum class ItemType {
    HealthPotion,   // 血瓶 +30HP
    PowerUp,        // 攻击力 +10
    SpeedUp         // 移速 +20%
};

class Item {
public:
    Item(float x, float y, ItemType type);
    void draw(sf::RenderWindow& window, sf::Vector2f cameraOffset);
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    ItemType getType() const { return type; }
    bool isCollected() const { return collected; }
    void collect() { collected = true; }

private:
    sf::CircleShape shape;
    sf::Text label;
    ItemType type;
    bool collected = false;
    float bobTimer = 0.f;
    float baseY;
};
#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"

class Bullet {
public:
    Bullet(sf::Vector2f pos, sf::Vector2f dir, int damage);
    void update(float dt, const Map& map);
    void draw(sf::RenderWindow& window, sf::Vector2f cameraOffset);
    bool isAlive() const { return alive; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    int getDamage() const { return damage; }
    void kill() { alive = false; }

private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    int damage;
    bool alive = true;
    float lifetime = 2.0f;
    float speed = 400.f;
};
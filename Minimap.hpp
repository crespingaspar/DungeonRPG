#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "Player.hpp"

class Minimap {
public:
    Minimap();
    void update(const Map& map, const Player& player);
    void draw(sf::RenderWindow& window);

private:
    static const int MINI_TILE = 4;
    static const int MINI_W = MAP_WIDTH  * MINI_TILE;
    static const int MINI_H = MAP_HEIGHT * MINI_TILE;
    static const int MARGIN = 10;

    sf::RenderTexture renderTex;
    sf::Sprite sprite;
    sf::RectangleShape background;
    sf::RectangleShape playerDot;
};
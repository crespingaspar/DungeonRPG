#include "Minimap.hpp"

Minimap::Minimap() {
    renderTex.create(MINI_W, MINI_H);

    background.setSize(sf::Vector2f(MINI_W + 4, MINI_H + 4));
    background.setFillColor(sf::Color(0, 0, 0, 180));
    background.setOutlineColor(sf::Color(100, 100, 100, 200));
    background.setOutlineThickness(1);

    playerDot.setSize(sf::Vector2f(MINI_TILE, MINI_TILE));
    playerDot.setFillColor(sf::Color(100, 180, 255));
}

void Minimap::update(const Map& map, const Player& player) {
    renderTex.clear(sf::Color::Transparent);

    sf::RectangleShape tile(sf::Vector2f(MINI_TILE, MINI_TILE));

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            TileType t = map.getTile(x, y);
            if (t == TileType::Wall) continue;
            tile.setPosition(x * MINI_TILE, y * MINI_TILE);
            tile.setFillColor(sf::Color(120, 90, 60, 200));
            renderTex.draw(tile);
        }
    }

    // 玩家位置
    sf::Vector2f pPos = player.getPosition();
    playerDot.setPosition(
        (pPos.x / TILE_SIZE) * MINI_TILE,
        (pPos.y / TILE_SIZE) * MINI_TILE
    );
    renderTex.draw(playerDot);
    renderTex.display();

    sprite.setTexture(renderTex.getTexture());
}

void Minimap::draw(sf::RenderWindow& window) {
    float wx = window.getSize().x - MINI_W - MARGIN;
    float wy = MARGIN;

    background.setPosition(wx - 2, wy - 2);
    window.draw(background);

    sprite.setPosition(wx, wy);
    window.draw(sprite);
}
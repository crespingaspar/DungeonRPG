#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

const int TILE_SIZE = 32;
const int MAP_WIDTH = 50;
const int MAP_HEIGHT = 50;

enum class TileType {
    Wall,
    Floor,
    Door
};

struct Room {
    int x, y, w, h;
    int centerX() const { return x + w / 2; }
    int centerY() const { return y + h / 2; }
};

class Map {
public:
    Map();
    void generate();
    void draw(sf::RenderWindow& window, sf::Vector2f cameraOffset);
    bool isWalkable(int x, int y) const;
    TileType getTile(int x, int y) const;
    const std::vector<Room>& getRooms() const { return rooms; }

private:
    void generateRooms();
    void connectRooms(const Room& a, const Room& b);
    void carveHCorridor(int x1, int x2, int y);
    void carveVCorridor(int y1, int y2, int x);
    void buildVertexArray();

    std::vector<std::vector<TileType>> tiles;
    std::vector<Room> rooms;
    sf::VertexArray vertexArray;
    sf::Color floorColor  = sf::Color(80, 60, 40);
    sf::Color wallColor   = sf::Color(40, 40, 60);
    sf::Color doorColor   = sf::Color(139, 90, 43);
};
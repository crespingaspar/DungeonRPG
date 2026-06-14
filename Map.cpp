#include "Map.hpp"
#include <algorithm>

Map::Map() {
    tiles.assign(MAP_HEIGHT, std::vector<TileType>(MAP_WIDTH, TileType::Wall));
}

void Map::generate() {
    tiles.assign(MAP_HEIGHT, std::vector<TileType>(MAP_WIDTH, TileType::Wall));
    rooms.clear();
    generateRooms();
    buildVertexArray();
}

void Map::generateRooms() {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> roomW(5, 12);
    std::uniform_int_distribution<int> roomH(4, 10);
    std::uniform_int_distribution<int> posX(1, MAP_WIDTH - 14);
    std::uniform_int_distribution<int> posY(1, MAP_HEIGHT - 12);

    int attempts = 200;
    while ((int)rooms.size() < 15 && attempts-- > 0) {
        Room r;
        r.w = roomW(rng);
        r.h = roomH(rng);
        r.x = posX(rng);
        r.y = posY(rng);

        // 检查是否和已有房间重叠
        bool overlap = false;
        for (auto& other : rooms) {
            if (r.x < other.x + other.w + 1 && r.x + r.w + 1 > other.x &&
                r.y < other.y + other.h + 1 && r.y + r.h + 1 > other.y) {
                overlap = true;
                break;
            }
        }
        if (overlap) continue;

        // 雕刻房间
        for (int y = r.y; y < r.y + r.h; y++)
            for (int x = r.x; x < r.x + r.w; x++)
                tiles[y][x] = TileType::Floor;

        // 连接到上一个房间
        if (!rooms.empty()) {
            auto& prev = rooms.back();
            if (rng() % 2 == 0) {
                carveHCorridor(prev.centerX(), r.centerX(), prev.centerY());
                carveVCorridor(prev.centerY(), r.centerY(), r.centerX());
            } else {
                carveVCorridor(prev.centerY(), r.centerY(), prev.centerX());
                carveHCorridor(prev.centerX(), r.centerX(), r.centerY());
            }
        }
        rooms.push_back(r);
    }
}

void Map::carveHCorridor(int x1, int x2, int y) {
    for (int x = std::min(x1, x2); x <= std::max(x1, x2); x++)
        if (y >= 0 && y < MAP_HEIGHT && x >= 0 && x < MAP_WIDTH)
            tiles[y][x] = TileType::Floor;
}

void Map::carveVCorridor(int y1, int y2, int x) {
    for (int y = std::min(y1, y2); y <= std::max(y1, y2); y++)
        if (y >= 0 && y < MAP_HEIGHT && x >= 0 && x < MAP_WIDTH)
            tiles[y][x] = TileType::Floor;
}

bool Map::isWalkable(int x, int y) const {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return false;
    return tiles[y][x] != TileType::Wall;
}

TileType Map::getTile(int x, int y) const {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return TileType::Wall;
    return tiles[y][x];
}

void Map::buildVertexArray() {
    vertexArray.setPrimitiveType(sf::Quads);
    vertexArray.resize(MAP_WIDTH * MAP_HEIGHT * 4);

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            sf::Vertex* quad = &vertexArray[(y * MAP_WIDTH + x) * 4];
            float px = x * TILE_SIZE;
            float py = y * TILE_SIZE;

            quad[0].position = sf::Vector2f(px, py);
            quad[1].position = sf::Vector2f(px + TILE_SIZE, py);
            quad[2].position = sf::Vector2f(px + TILE_SIZE, py + TILE_SIZE);
            quad[3].position = sf::Vector2f(px, py + TILE_SIZE);

            sf::Color c = (tiles[y][x] == TileType::Wall) ? wallColor : floorColor;
            quad[0].color = quad[1].color = quad[2].color = quad[3].color = c;
        }
    }
}

void Map::draw(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    sf::RenderStates states;
    states.transform.translate(-cameraOffset);
    window.draw(vertexArray, states);
}
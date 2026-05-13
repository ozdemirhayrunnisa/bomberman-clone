#include "Map.hpp"
#include <cstdlib>
#include <ctime>

Map::Map() {
    tileShape.setSize({static_cast<float>(TILE_SIZE) - 1.f,
                       static_cast<float>(TILE_SIZE) - 1.f});
    generate();
}

void Map::generate() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            bool border = (r == 0 || r == MAP_ROWS - 1 || c == 0 || c == MAP_COLS - 1);
            bool pillar = (r % 2 == 0 && c % 2 == 0);
            grid[r][c]  = (border || pillar) ? TileType::Wall : TileType::Empty;
        }
    }

    for (int r = 1; r < MAP_ROWS - 1; ++r) {
        for (int c = 1; c < MAP_COLS - 1; ++c) {
            if (grid[r][c] != TileType::Empty) continue;

            bool p1zone = (r <= 2 && c <= 2);
            bool p2zone = (r <= 2 && c >= MAP_COLS - 3);
            if (p1zone || p2zone) continue;

            if (std::rand() % 100 < 65)
                grid[r][c] = TileType::Block;
        }
    }
}

void Map::render(sf::RenderWindow& window) {
    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            tileShape.setPosition({
                static_cast<float>(c * TILE_SIZE) + 0.5f,
                static_cast<float>(r * TILE_SIZE) + 0.5f
            });
            switch (grid[r][c]) {
                case TileType::Wall:  tileShape.setFillColor({70,  70,  85 }); break;
                case TileType::Block: tileShape.setFillColor({160, 100, 40 }); break;
                case TileType::Empty: tileShape.setFillColor({200, 185, 140}); break;
            }
            window.draw(tileShape);
        }
    }
}

TileType Map::getTile(int col, int row) const {
    if (col < 0 || col >= MAP_COLS || row < 0 || row >= MAP_ROWS)
        return TileType::Wall;
    return grid[row][col];
}

void Map::setTile(int col, int row, TileType type) {
    if (col >= 0 && col < MAP_COLS && row >= 0 && row < MAP_ROWS)
        grid[row][col] = type;
}

bool Map::isWalkable(int col, int row) const {
    return getTile(col, row) == TileType::Empty;
}

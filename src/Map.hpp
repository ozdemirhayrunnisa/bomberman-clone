#pragma once

#include <array>
#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include "Tile.hpp"

class Map {
public:
    Map();

    void     render(sf::RenderWindow& window);
    TileType getTile(int col, int row) const;
    void     setTile(int col, int row, TileType type);
    bool     isWalkable(int col, int row) const;

private:
    std::array<std::array<TileType, MAP_COLS>, MAP_ROWS> grid{};
    sf::RectangleShape tileShape;

    void generate();
};

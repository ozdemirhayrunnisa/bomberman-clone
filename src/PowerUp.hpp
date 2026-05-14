#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"

enum class PowerUpType { Range, Bomb };

class PowerUp {
public:
    PowerUp(int col, int row, PowerUpType type);

    void render(sf::RenderWindow& window);

    int         getCol()  const { return col; }
    int         getRow()  const { return row; }
    PowerUpType getType() const { return type; }

private:
    int         col, row;
    PowerUpType type;

    sf::CircleShape    circle;
    sf::RectangleShape icon;
};

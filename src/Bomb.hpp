#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"

class Bomb {
public:
    Bomb(int col, int row, int range, float fuse = BOMB_FUSE);

    void update(float dt);
    void render(sf::RenderWindow& window);

    bool shouldExplode() const { return timer <= 0.f && !expired; }
    bool isExpired()     const { return expired; }
    void setExpired()          { expired = true; }

    int getCol()   const { return col; }
    int getRow()   const { return row; }
    int getRange() const { return range; }

private:
    int   col, row, range;
    float timer;
    bool  expired = false;

    sf::CircleShape    body;
    sf::CircleShape    fuse_dot;
};

#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"

class Explosion {
public:
    Explosion(int col, int row, float duration = 0.6f);

    void update(float dt);
    void render(sf::RenderWindow& window);

    bool isFinished() const { return timer <= 0.f; }
    int  getCol()     const { return col; }
    int  getRow()     const { return row; }

private:
    int   col, row;
    float timer;
    float duration;

    sf::RectangleShape shape;
};

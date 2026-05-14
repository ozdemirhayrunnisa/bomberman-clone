#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"

class Map;

constexpr float ENEMY_SPEED = 1.5f * TILE_SIZE;

class Enemy {
public:
    Enemy(int col, int row, float vx, float vy);

    void update(float dt, const Map& map);
    void render(sf::RenderWindow& window);

    int  getCol() const { return static_cast<int>(px) / TILE_SIZE; }
    int  getRow() const { return static_cast<int>(py) / TILE_SIZE; }
    bool isAlive() const { return alive; }
    void kill()          { alive = false; }

private:
    float px, py;
    float vx, vy;
    bool  alive = true;

    sf::RectangleShape shape;

    bool canMoveTo(float x, float y, const Map& map) const;
};

#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"

class Map;

class Player {
public:
    Player(int startCol, int startRow, sf::Color color);

    void handleInput(sf::Keyboard::Key up, sf::Keyboard::Key down,
                     sf::Keyboard::Key left, sf::Keyboard::Key right,
                     sf::Keyboard::Key bombKey);
    void update(float dt, const Map& map);
    void render(sf::RenderWindow& window);

    int  getCol()       const { return static_cast<int>(px) / TILE_SIZE; }
    int  getRow()       const { return static_cast<int>(py) / TILE_SIZE; }
    bool isAlive()      const { return alive; }
    int  getLives()     const { return lives; }
    int  getBombCount() const { return bombCount; }
    int  getExpRange()  const { return expRange; }

    bool wantsBomb()      const { return bombReq; }
    void consumeBombReq()       { bombReq = false; }

    void loseLife() { if (--lives <= 0) alive = false; }
    void addBomb()  { ++bombCount; }
    void addRange() { ++expRange; }

private:
    float px, py;
    sf::RectangleShape shape;

    bool alive     = true;
    int  lives     = 3;
    int  bombCount = 1;
    int  expRange  = 2;

    bool moveUp    = false;
    bool moveDown  = false;
    bool moveLeft  = false;
    bool moveRight = false;
    bool bombReq   = false;
    bool bombHeld  = false;

    bool canMoveTo(float x, float y, const Map& map) const;
};

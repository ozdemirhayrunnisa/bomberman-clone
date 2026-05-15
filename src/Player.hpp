#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include "Constants.hpp"

class Map;

class Player {
public:
    Player(int startCol, int startRow, sf::Color fallbackColor);

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

    bool isInvincible()         const { return invTimer > 0.f; }
    void updateInvincible(float dt)   { if (invTimer > 0.f) invTimer -= dt; }

    void hit();
    void addBomb()  { ++bombCount; }
    void addRange() { ++expRange; }

private:
    float     px, py;
    int       startCol, startRow;
    sf::Color fallbackColor;

    // Animasyon
    static constexpr int   FRAME_COUNT = 3;
    static constexpr float FRAME_TIME  = 0.15f;
    std::array<sf::Texture, FRAME_COUNT> textures;
    bool  texturesLoaded = false;
    int   currentFrame   = 0;
    float frameTimer     = 0.f;
    bool  facingLeft     = false;

    // Yedek kare (sprite yuklenemezse)
    sf::RectangleShape shape;

    bool  alive     = true;
    int   lives     = 3;
    int   bombCount = 1;
    int   expRange  = 2;
    float invTimer  = 0.f;

    bool moveUp    = false;
    bool moveDown  = false;
    bool moveLeft  = false;
    bool moveRight = false;
    bool bombReq   = false;
    bool bombHeld  = false;

    bool canMoveTo(float x, float y, const Map& map) const;
    void respawn();
};

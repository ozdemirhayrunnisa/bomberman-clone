#include "Player.hpp"
#include "Map.hpp"
#include <algorithm>

Player::Player(int startCol, int startRow, sf::Color fallbackColor)
    : px(static_cast<float>(startCol * TILE_SIZE)),
      py(static_cast<float>(startRow * TILE_SIZE)),
      startCol(startCol), startRow(startRow),
      fallbackColor(fallbackColor) {

    // Sprite frameleri yukle
    texturesLoaded = true;
    for (int i = 0; i < FRAME_COUNT; ++i) {
        if (!textures[i].loadFromFile("assets/player" + std::to_string(i) + ".png"))
            texturesLoaded = false;
    }

    // Yedek kare
    const float size = TILE_SIZE - 10.f;
    shape.setSize({size, size});
    shape.setFillColor(fallbackColor);
    shape.setOutlineThickness(2.f);
    shape.setOutlineColor(sf::Color::White);
}

void Player::handleInput(sf::Keyboard::Key up, sf::Keyboard::Key down,
                         sf::Keyboard::Key left, sf::Keyboard::Key right,
                         sf::Keyboard::Key bombKey) {
    moveUp    = sf::Keyboard::isKeyPressed(up);
    moveDown  = sf::Keyboard::isKeyPressed(down);
    moveLeft  = sf::Keyboard::isKeyPressed(left);
    moveRight = sf::Keyboard::isKeyPressed(right);

    bool pressed = sf::Keyboard::isKeyPressed(bombKey);
    if (pressed && !bombHeld) {
        bombReq  = true;
        bombHeld = true;
    }
    if (!pressed) bombHeld = false;
}

void Player::update(float dt, const Map& map) {
    float vx = 0.f, vy = 0.f;
    if (moveUp)    vy -= PLAYER_SPEED;
    if (moveDown)  vy += PLAYER_SPEED;
    if (moveLeft)  vx -= PLAYER_SPEED;
    if (moveRight) vx += PLAYER_SPEED;

    if (vx != 0.f && vy != 0.f) {
        vx *= 0.7071f;
        vy *= 0.7071f;
    }

    if (vx < 0.f) facingLeft = true;
    if (vx > 0.f) facingLeft = false;

    float newX = px + vx * dt;
    if (canMoveTo(newX, py, map)) px = newX;

    float newY = py + vy * dt;
    if (canMoveTo(px, newY, map)) py = newY;

    // Kare animasyonu
    bool isMoving = (moveUp || moveDown || moveLeft || moveRight);
    if (isMoving) {
        frameTimer += dt;
        if (frameTimer >= FRAME_TIME) {
            frameTimer = 0.f;
            currentFrame = (currentFrame + 1) % FRAME_COUNT;
        }
    } else {
        currentFrame = 0;
        frameTimer   = 0.f;
    }
}

bool Player::canMoveTo(float x, float y, const Map& map) const {
    const float m = 5.f;
    int c0 = static_cast<int>(x + m)             / TILE_SIZE;
    int c1 = static_cast<int>(x + TILE_SIZE - m) / TILE_SIZE;
    int r0 = static_cast<int>(y + m)             / TILE_SIZE;
    int r1 = static_cast<int>(y + TILE_SIZE - m) / TILE_SIZE;

    return map.isWalkable(c0, r0) &&
           map.isWalkable(c1, r0) &&
           map.isWalkable(c0, r1) &&
           map.isWalkable(c1, r1);
}

void Player::hit() {
    if (--lives <= 0) alive = false;
    respawn();
    invTimer = 1.5f;
}

void Player::respawn() {
    px = static_cast<float>(startCol * TILE_SIZE);
    py = static_cast<float>(startRow * TILE_SIZE);
}

void Player::render(sf::RenderWindow& window) {
    if (invTimer > 0.f && static_cast<int>(invTimer * 10) % 2 == 0)
        return;

    if (texturesLoaded) {
        sf::Sprite sp(textures[currentFrame]);

        auto sz = textures[currentFrame].getSize();
        float scale = static_cast<float>(TILE_SIZE - 4) /
                      static_cast<float>(std::max(sz.x, sz.y));

        if (facingLeft) {
            sp.setScale({-scale, scale});
            sp.setPosition({px + static_cast<float>(TILE_SIZE) - 2.f, py + 2.f});
        } else {
            sp.setScale({scale, scale});
            sp.setPosition({px + 2.f, py + 2.f});
        }

        window.draw(sp);
    } else {
        shape.setPosition({px + 5.f, py + 5.f});
        window.draw(shape);
    }
}

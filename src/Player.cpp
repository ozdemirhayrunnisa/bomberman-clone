#include "Player.hpp"
#include "Map.hpp"

Player::Player(int startCol, int startRow, sf::Color color)
    : px(static_cast<float>(startCol * TILE_SIZE)),
      py(static_cast<float>(startRow * TILE_SIZE)) {

    const float size = TILE_SIZE - 10.f;
    shape.setSize({size, size});
    shape.setFillColor(color);
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

    float newX = px + vx * dt;
    if (canMoveTo(newX, py, map)) px = newX;

    float newY = py + vy * dt;
    if (canMoveTo(px, newY, map)) py = newY;
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

void Player::render(sf::RenderWindow& window) {
    shape.setPosition({px + 5.f, py + 5.f});
    window.draw(shape);
}

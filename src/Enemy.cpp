#include "Enemy.hpp"
#include "Map.hpp"

Enemy::Enemy(int col, int row, float vx, float vy)
    : px(static_cast<float>(col * TILE_SIZE)),
      py(static_cast<float>(row * TILE_SIZE)),
      vx(vx), vy(vy) {

    const float size = TILE_SIZE - 10.f;
    shape.setSize({size, size});
    shape.setFillColor(sf::Color(200, 50, 50));
    shape.setOutlineThickness(2.f);
    shape.setOutlineColor(sf::Color(255, 120, 120));
}

void Enemy::update(float dt, const Map& map) {
    float newX = px + vx * dt;
    if (canMoveTo(newX, py, map)) {
        px = newX;
    } else {
        vx = -vx;
    }

    float newY = py + vy * dt;
    if (canMoveTo(px, newY, map)) {
        py = newY;
    } else {
        vy = -vy;
    }
}

bool Enemy::canMoveTo(float x, float y, const Map& map) const {
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

void Enemy::render(sf::RenderWindow& window) {
    shape.setPosition({px + 5.f, py + 5.f});
    window.draw(shape);
}

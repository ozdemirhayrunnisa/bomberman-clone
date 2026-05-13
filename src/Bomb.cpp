#include "Bomb.hpp"
#include <cmath>

Bomb::Bomb(int col, int row, int range, float fuse)
    : col(col), row(row), range(range), timer(fuse) {

    float r = TILE_SIZE / 2.f - 7.f;
    body.setRadius(r);
    body.setOrigin({r, r});
    body.setFillColor({30, 30, 30});
    body.setOutlineThickness(2.f);
    body.setOutlineColor({180, 180, 180});

    float dr = 4.f;
    fuse_dot.setRadius(dr);
    fuse_dot.setOrigin({dr, dr});
    fuse_dot.setFillColor({255, 200, 0});
}

void Bomb::update(float dt) {
    if (timer > 0.f) timer -= dt;
}

void Bomb::render(sf::RenderWindow& window) {
    float cx = col * TILE_SIZE + TILE_SIZE / 2.f;
    float cy = row * TILE_SIZE + TILE_SIZE / 2.f;

    float speed = (timer < 1.f) ? 20.f : 8.f;
    float pulse = std::sin(timer * speed);
    float scale = 1.f + 0.15f * pulse;

    body.setPosition({cx, cy});
    body.setScale({scale, scale});

    if (timer < 1.f)
        body.setFillColor({200, 50, 50});
    else
        body.setFillColor({30, 30, 30});

    window.draw(body);

    float angle = timer * 5.f;
    float fr = TILE_SIZE / 2.f - 10.f;
    fuse_dot.setPosition({
        cx + std::cos(angle) * fr * 0.5f,
        cy - fr * 0.7f + std::sin(angle) * 3.f
    });
    window.draw(fuse_dot);
}

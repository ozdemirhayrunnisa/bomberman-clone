#include "PowerUp.hpp"

PowerUp::PowerUp(int col, int row, PowerUpType type)
    : col(col), row(row), type(type) {

    float cx = col * TILE_SIZE + TILE_SIZE / 2.f;
    float cy = row * TILE_SIZE + TILE_SIZE / 2.f;

    float r = TILE_SIZE / 2.f - 8.f;
    circle.setRadius(r);
    circle.setOrigin({r, r});
    circle.setPosition({cx, cy});

    if (type == PowerUpType::Range)
        circle.setFillColor(sf::Color(230, 120, 20));
    else
        circle.setFillColor(sf::Color(60, 140, 220));

    // Kucuk beyaz kare ikon
    float is = 10.f;
    icon.setSize({is, is});
    icon.setOrigin({is / 2.f, is / 2.f});
    icon.setPosition({cx, cy});
    icon.setFillColor(sf::Color::White);
}

void PowerUp::render(sf::RenderWindow& window) {
    window.draw(circle);
    window.draw(icon);
}

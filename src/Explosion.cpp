#include "Explosion.hpp"

Explosion::Explosion(int col, int row, float duration)
    : col(col), row(row), timer(duration), duration(duration) {

    shape.setSize({static_cast<float>(TILE_SIZE) - 1.f,
                   static_cast<float>(TILE_SIZE) - 1.f});
    shape.setPosition({
        static_cast<float>(col * TILE_SIZE) + 0.5f,
        static_cast<float>(row * TILE_SIZE) + 0.5f
    });
}

void Explosion::update(float dt) {
    if (timer > 0.f) timer -= dt;
}

void Explosion::render(sf::RenderWindow& window) {
    float ratio = timer / duration;
    auto  alpha = static_cast<std::uint8_t>(220 * ratio);
    auto  green = static_cast<std::uint8_t>(160 * ratio);

    shape.setFillColor(sf::Color(255, green, 0, alpha));
    window.draw(shape);
}

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <string>
#include "Map.hpp"
#include "Player.hpp"
#include "Bomb.hpp"

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::Clock        clock;

    sf::Font              font;
    bool                  fontLoaded = false;
    std::optional<sf::Text> livesText;
    std::optional<sf::Text> scoreText;
    std::optional<sf::Text> infoText;

    Map    map;
    Player player1;
    int    score       = 0;
    int    activeBombs = 0;

    std::vector<Bomb> bombs;
    sf::RectangleShape hudBar;

    void handleEvents();
    void update(float dt);
    void render();
    void renderHUD();

    void tryPlaceBomb(int col, int row);
    void processExplosions();
    void refreshHUD();
};

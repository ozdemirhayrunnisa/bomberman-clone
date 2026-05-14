#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <string>
#include "Map.hpp"
#include "Player.hpp"
#include "Bomb.hpp"
#include "Explosion.hpp"
#include "PowerUp.hpp"
#include "Enemy.hpp"

enum class GameState { Playing, GameOver };

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::Clock        clock;

    sf::Font               font;
    bool                   fontLoaded = false;
    std::optional<sf::Text> livesText;
    std::optional<sf::Text> scoreText;
    std::optional<sf::Text> infoText;
    std::optional<sf::Text> gameOverText;
    std::optional<sf::Text> restartText;

    Map       map;
    Player    player1;
    int       score       = 0;
    int       activeBombs = 0;
    GameState state       = GameState::Playing;

    std::vector<Bomb>      bombs;
    std::vector<Explosion> explosions;
    std::vector<PowerUp>   powerUps;
    std::vector<Enemy>     enemies;

    sf::RectangleShape hudBar;
    sf::RectangleShape overlay;

    void handleEvents();
    void update(float dt);
    void render();
    void renderHUD();
    void renderGameOver();

    void tryPlaceBomb(int col, int row);
    void processExplosions();
    void checkPlayerDeath();
    void checkEnemyCollisions();
    void spawnEnemies();
    void refreshHUD();
    void reset();
};

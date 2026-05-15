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

enum class GameState { Playing, GameOver, Won };

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::Clock        clock;

    sf::Font                font;
    bool                    fontLoaded = false;
    std::optional<sf::Text> p1InfoText;
    std::optional<sf::Text> p2InfoText;
    std::optional<sf::Text> scoreText;
    std::optional<sf::Text> enemyText;
    std::optional<sf::Text> centerText;
    std::optional<sf::Text> restartText;

    Map       map;
    Player    player1;
    Player    player2;
    int       score       = 0;
    int       activeBombs1 = 0;
    int       activeBombs2 = 0;
    GameState state        = GameState::Playing;

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
    void renderWin();

    void tryPlaceBomb(Player& player, int& activeBombs);
    void processExplosions();
    void checkPlayerDeath(Player& player);
    void checkEnemyCollisions(Player& player);
    void spawnEnemies();
    void refreshHUD();
    void reset();
};

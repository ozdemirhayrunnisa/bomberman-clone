#include "Game.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <cstdlib>

Game::Game()
    : window(sf::VideoMode({static_cast<unsigned>(WINDOW_WIDTH),
                            static_cast<unsigned>(WINDOW_HEIGHT)}),
             "Bomberman Clone",
             sf::Style::Titlebar | sf::Style::Close),
      player1(1, 1, sf::Color(70, 130, 220)) {

    window.setFramerateLimit(60);

    fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    if (fontLoaded) {
        auto makeText = [&](std::optional<sf::Text>& t, float x, float y, unsigned sz) {
            t.emplace(font, "", sz);
            t->setFillColor(sf::Color::White);
            t->setPosition({x, y});
        };
        makeText(livesText,  10.f,  static_cast<float>(HUD_TOP) + 12.f, 20);
        makeText(scoreText,  240.f, static_cast<float>(HUD_TOP) + 12.f, 20);
        makeText(infoText,   450.f, static_cast<float>(HUD_TOP) + 14.f, 18);
        makeText(gameOverText, 0.f, 0.f, 48);
        makeText(restartText,  0.f, 0.f, 22);
    }

    hudBar.setSize({static_cast<float>(WINDOW_WIDTH), static_cast<float>(HUD_HEIGHT)});
    hudBar.setPosition({0.f, static_cast<float>(HUD_TOP)});
    hudBar.setFillColor(sf::Color(35, 35, 55));

    overlay.setSize({static_cast<float>(WINDOW_WIDTH), static_cast<float>(HUD_TOP)});
    overlay.setPosition({0.f, 0.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 160));

    refreshHUD();
}

void Game::run() {
    while (window.isOpen()) {
        float dt = std::min(clock.restart().asSeconds(), 0.05f);
        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            window.close();
        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Escape)
                window.close();
            if (key->code == sf::Keyboard::Key::R && state == GameState::GameOver)
                reset();
        }
    }
}

void Game::update(float dt) {
    if (state == GameState::GameOver) return;

    player1.handleInput(
        sf::Keyboard::Key::W, sf::Keyboard::Key::S,
        sf::Keyboard::Key::A, sf::Keyboard::Key::D,
        sf::Keyboard::Key::Space
    );
    player1.update(dt, map);
    player1.updateInvincible(dt);

    if (player1.wantsBomb()) {
        tryPlaceBomb(player1.getCol(), player1.getRow());
        player1.consumeBombReq();
    }

    for (auto& bomb : bombs) bomb.update(dt);
    processExplosions();

    for (auto& exp : explosions) exp.update(dt);
    explosions.erase(
        std::remove_if(explosions.begin(), explosions.end(),
                       [](const Explosion& e) { return e.isFinished(); }),
        explosions.end()
    );

    auto before = static_cast<int>(bombs.size());
    bombs.erase(
        std::remove_if(bombs.begin(), bombs.end(),
                       [](const Bomb& b) { return b.isExpired(); }),
        bombs.end()
    );
    activeBombs -= before - static_cast<int>(bombs.size());

    // Power-up toplama
    powerUps.erase(
        std::remove_if(powerUps.begin(), powerUps.end(),
            [&](const PowerUp& p) {
                if (p.getCol() == player1.getCol() && p.getRow() == player1.getRow()) {
                    if (p.getType() == PowerUpType::Range) player1.addRange();
                    else                                   player1.addBomb();
                    return true;
                }
                return false;
            }),
        powerUps.end()
    );

    checkPlayerDeath();
    refreshHUD();
}

void Game::render() {
    window.clear(sf::Color(20, 20, 30));
    map.render(window);
    for (auto& pu  : powerUps)   pu.render(window);
    for (auto& exp : explosions) exp.render(window);
    for (auto& bomb : bombs)     bomb.render(window);
    player1.render(window);
    renderHUD();
    if (state == GameState::GameOver) renderGameOver();
    window.display();
}

void Game::renderHUD() {
    window.draw(hudBar);
    if (livesText) window.draw(*livesText);
    if (scoreText) window.draw(*scoreText);
    if (infoText)  window.draw(*infoText);
}

void Game::renderGameOver() {
    window.draw(overlay);

    if (!gameOverText) return;

    gameOverText->setString("GAME OVER");
    auto gb = gameOverText->getLocalBounds();
    gameOverText->setOrigin({gb.size.x / 2.f, gb.size.y / 2.f});
    gameOverText->setPosition({WINDOW_WIDTH / 2.f, HUD_TOP / 2.f - 30.f});
    window.draw(*gameOverText);

    restartText->setString("R - Yeniden Baslat");
    auto rb = restartText->getLocalBounds();
    restartText->setOrigin({rb.size.x / 2.f, rb.size.y / 2.f});
    restartText->setPosition({WINDOW_WIDTH / 2.f, HUD_TOP / 2.f + 30.f});
    window.draw(*restartText);
}

void Game::tryPlaceBomb(int col, int row) {
    if (activeBombs >= player1.getBombCount()) return;
    for (const auto& b : bombs)
        if (b.getCol() == col && b.getRow() == row) return;
    bombs.emplace_back(col, row, player1.getExpRange());
    ++activeBombs;
}

void Game::processExplosions() {
    static const int dx[] = { 0,  0, 1, -1 };
    static const int dy[] = {-1,  1, 0,  0 };

    for (auto& bomb : bombs) {
        if (!bomb.shouldExplode()) continue;

        explosions.emplace_back(bomb.getCol(), bomb.getRow());

        for (int d = 0; d < 4; ++d) {
            for (int i = 1; i <= bomb.getRange(); ++i) {
                int nc = bomb.getCol() + dx[d] * i;
                int nr = bomb.getRow() + dy[d] * i;
                TileType t = map.getTile(nc, nr);
                if (t == TileType::Wall) break;
                explosions.emplace_back(nc, nr);
                if (t == TileType::Block) {
                    map.setTile(nc, nr, TileType::Empty);
                    score += 10;
                    if (std::rand() % 100 < 30) {
                        PowerUpType pu = (std::rand() % 2 == 0) ? PowerUpType::Range : PowerUpType::Bomb;
                        powerUps.emplace_back(nc, nr, pu);
                    }
                    break;
                }
            }
        }
        bomb.setExpired();
    }
}

void Game::checkPlayerDeath() {
    if (player1.isInvincible()) return;

    for (const auto& exp : explosions) {
        if (exp.getCol() == player1.getCol() && exp.getRow() == player1.getRow()) {
            player1.hit();
            if (!player1.isAlive())
                state = GameState::GameOver;
            break;
        }
    }
}

void Game::refreshHUD() {
    if (!livesText) return;
    livesText->setString("Can: " + std::to_string(player1.getLives()));
    scoreText->setString("Skor: " + std::to_string(score));
    infoText->setString(
        "Bomba: " + std::to_string(player1.getBombCount()) +
        "  Menzil: " + std::to_string(player1.getExpRange())
    );
}

void Game::reset() {
    map         = Map();
    player1     = Player(1, 1, sf::Color(70, 130, 220));
    bombs.clear();
    explosions.clear();
    powerUps.clear();
    activeBombs = 0;
    score       = 0;
    state       = GameState::Playing;
    refreshHUD();
}

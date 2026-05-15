#include "Game.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <cstdlib>

Game::Game()
    : window(sf::VideoMode({static_cast<unsigned>(WINDOW_WIDTH),
                            static_cast<unsigned>(WINDOW_HEIGHT)}),
             "Bomberman Clone",
             sf::Style::Titlebar | sf::Style::Close),
      player1(1,            1, sf::Color(70,  130, 220)),
      player2(MAP_COLS - 2, 1, sf::Color(60,  180, 80)) {

    window.setFramerateLimit(60);

    fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    if (fontLoaded) {
        auto mk = [&](std::optional<sf::Text>& t, float x, float y, unsigned sz) {
            t.emplace(font, "", sz);
            t->setFillColor(sf::Color::White);
            t->setPosition({x, y});
        };
        float hy = static_cast<float>(HUD_TOP) + 8.f;
        mk(p1InfoText,  5.f,                         hy, 17);
        mk(p2InfoText,  WINDOW_WIDTH / 2.f + 10.f,   hy, 17);
        mk(scoreText,   WINDOW_WIDTH / 2.f - 80.f,   hy, 17);
        mk(enemyText,   WINDOW_WIDTH / 2.f - 80.f,   hy + 18.f, 15);
        mk(centerText,  0.f, 0.f, 48);
        mk(restartText, 0.f, 0.f, 22);
    }

    hudBar.setSize({static_cast<float>(WINDOW_WIDTH), static_cast<float>(HUD_HEIGHT)});
    hudBar.setPosition({0.f, static_cast<float>(HUD_TOP)});
    hudBar.setFillColor(sf::Color(35, 35, 55));

    overlay.setSize({static_cast<float>(WINDOW_WIDTH), static_cast<float>(HUD_TOP)});
    overlay.setPosition({0.f, 0.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 160));

    spawnEnemies();
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
            if (key->code == sf::Keyboard::Key::R &&
                (state == GameState::GameOver || state == GameState::Won))
                reset();
        }
    }
}

void Game::update(float dt) {
    if (state != GameState::Playing) return;

    // Oyuncu 1 — WASD + Space
    if (player1.isAlive()) {
        player1.handleInput(
            sf::Keyboard::Key::W, sf::Keyboard::Key::S,
            sf::Keyboard::Key::A, sf::Keyboard::Key::D,
            sf::Keyboard::Key::Space
        );
        player1.update(dt, map);
        player1.updateInvincible(dt);
        if (player1.wantsBomb()) {
            tryPlaceBomb(player1, activeBombs1);
            player1.consumeBombReq();
        }
    }

    // Oyuncu 2 — Ok tuslari + Enter
    if (player2.isAlive()) {
        player2.handleInput(
            sf::Keyboard::Key::Up,    sf::Keyboard::Key::Down,
            sf::Keyboard::Key::Left,  sf::Keyboard::Key::Right,
            sf::Keyboard::Key::Enter
        );
        player2.update(dt, map);
        player2.updateInvincible(dt);
        if (player2.wantsBomb()) {
            tryPlaceBomb(player2, activeBombs2);
            player2.consumeBombReq();
        }
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
    int removed = before - static_cast<int>(bombs.size());
    activeBombs1 = std::max(0, activeBombs1 - removed);
    activeBombs2 = std::max(0, activeBombs2 - removed);

    // Power-up toplama
    powerUps.erase(
        std::remove_if(powerUps.begin(), powerUps.end(),
            [&](const PowerUp& p) {
                auto collect = [&](Player& pl) {
                    if (!pl.isAlive()) return false;
                    if (p.getCol() == pl.getCol() && p.getRow() == pl.getRow()) {
                        if (p.getType() == PowerUpType::Range) pl.addRange();
                        else                                   pl.addBomb();
                        return true;
                    }
                    return false;
                };
                return collect(player1) || collect(player2);
            }),
        powerUps.end()
    );

    // Dusman guncelle
    for (auto& e : enemies) e.update(dt, map);
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [&](Enemy& e) {
                for (const auto& exp : explosions)
                    if (exp.getCol() == e.getCol() && exp.getRow() == e.getRow()) {
                        score += 100;
                        return true;
                    }
                return false;
            }),
        enemies.end()
    );

    checkPlayerDeath(player1);
    checkPlayerDeath(player2);
    checkEnemyCollisions(player1);
    checkEnemyCollisions(player2);

    if (!player1.isAlive() && !player2.isAlive())
        state = GameState::GameOver;
    else if (enemies.empty())
        state = GameState::Won;

    refreshHUD();
}

void Game::render() {
    window.clear(sf::Color(20, 20, 30));
    map.render(window);
    for (auto& e  : enemies)    e.render(window);
    for (auto& pu : powerUps)   pu.render(window);
    for (auto& exp : explosions) exp.render(window);
    for (auto& bomb : bombs)    bomb.render(window);
    if (player1.isAlive()) player1.render(window);
    if (player2.isAlive()) player2.render(window);
    renderHUD();
    if (state == GameState::GameOver) renderGameOver();
    if (state == GameState::Won)      renderWin();
    window.display();
}

void Game::renderHUD() {
    window.draw(hudBar);

    // Ayirici cizgi
    sf::RectangleShape div;
    div.setSize({1.f, static_cast<float>(HUD_HEIGHT) - 8.f});
    div.setPosition({WINDOW_WIDTH / 2.f, static_cast<float>(HUD_TOP) + 4.f});
    div.setFillColor(sf::Color(100, 100, 130));
    window.draw(div);

    if (p1InfoText) window.draw(*p1InfoText);
    if (p2InfoText) window.draw(*p2InfoText);
    if (scoreText)  window.draw(*scoreText);
    if (enemyText)  window.draw(*enemyText);
}

void Game::renderGameOver() {
    window.draw(overlay);
    if (!centerText) return;

    centerText->setString("GAME OVER");
    centerText->setFillColor(sf::Color::White);
    auto gb = centerText->getLocalBounds();
    centerText->setOrigin({gb.size.x / 2.f, gb.size.y / 2.f});
    centerText->setPosition({WINDOW_WIDTH / 2.f, HUD_TOP / 2.f - 30.f});
    window.draw(*centerText);

    restartText->setString("R - Yeniden Baslat");
    restartText->setFillColor(sf::Color::White);
    auto rb = restartText->getLocalBounds();
    restartText->setOrigin({rb.size.x / 2.f, rb.size.y / 2.f});
    restartText->setPosition({WINDOW_WIDTH / 2.f, HUD_TOP / 2.f + 30.f});
    window.draw(*restartText);
}

void Game::renderWin() {
    sf::RectangleShape greenOverlay;
    greenOverlay.setSize({static_cast<float>(WINDOW_WIDTH), static_cast<float>(HUD_TOP)});
    greenOverlay.setPosition({0.f, 0.f});
    greenOverlay.setFillColor(sf::Color(0, 80, 0, 160));
    window.draw(greenOverlay);

    if (!centerText) return;

    centerText->setString("TEBRIKLER!");
    centerText->setFillColor(sf::Color(100, 255, 100));
    auto gb = centerText->getLocalBounds();
    centerText->setOrigin({gb.size.x / 2.f, gb.size.y / 2.f});
    centerText->setPosition({WINDOW_WIDTH / 2.f, HUD_TOP / 2.f - 40.f});
    window.draw(*centerText);

    restartText->setString("Skor: " + std::to_string(score) + "   |   R - Yeniden Baslat");
    restartText->setFillColor(sf::Color::White);
    auto rb = restartText->getLocalBounds();
    restartText->setOrigin({rb.size.x / 2.f, rb.size.y / 2.f});
    restartText->setPosition({WINDOW_WIDTH / 2.f, HUD_TOP / 2.f + 20.f});
    window.draw(*restartText);

    centerText->setFillColor(sf::Color::White);
}

void Game::tryPlaceBomb(Player& player, int& activeBombs) {
    if (activeBombs >= player.getBombCount()) return;
    int c = player.getCol(), r = player.getRow();
    for (const auto& b : bombs)
        if (b.getCol() == c && b.getRow() == r) return;
    bombs.emplace_back(c, r, player.getExpRange());
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

void Game::checkPlayerDeath(Player& player) {
    if (!player.isAlive() || player.isInvincible()) return;
    for (const auto& exp : explosions)
        if (exp.getCol() == player.getCol() && exp.getRow() == player.getRow()) {
            player.hit();
            return;
        }
}

void Game::checkEnemyCollisions(Player& player) {
    if (!player.isAlive() || player.isInvincible()) return;
    for (const auto& e : enemies)
        if (e.getCol() == player.getCol() && e.getRow() == player.getRow()) {
            player.hit();
            return;
        }
}

void Game::spawnEnemies() {
    struct Spawn { int c, r; float vx, vy; };
    const Spawn spawns[] = {
        {13, 11,  ENEMY_SPEED, 0.f},
        {11,  9,  0.f, ENEMY_SPEED},
        { 5, 11, -ENEMY_SPEED, 0.f},
    };
    for (auto& s : spawns) {
        map.setTile(s.c, s.r, TileType::Empty);
        enemies.emplace_back(s.c, s.r, s.vx, s.vy);
    }
}

void Game::refreshHUD() {
    if (!p1InfoText) return;

    p1InfoText->setString(
        "P1  Can:" + std::to_string(player1.getLives()) +
        "  B:"     + std::to_string(player1.getBombCount()) +
        "  R:"     + std::to_string(player1.getExpRange())
    );
    p1InfoText->setFillColor(player1.isAlive() ? sf::Color(130, 180, 255) : sf::Color(120, 120, 120));

    p2InfoText->setString(
        "P2  Can:" + std::to_string(player2.getLives()) +
        "  B:"     + std::to_string(player2.getBombCount()) +
        "  R:"     + std::to_string(player2.getExpRange())
    );
    p2InfoText->setFillColor(player2.isAlive() ? sf::Color(130, 220, 130) : sf::Color(120, 120, 120));

    scoreText->setString("Skor: " + std::to_string(score));
    enemyText->setString("Dusman: " + std::to_string(enemies.size()));
}

void Game::reset() {
    map          = Map();
    player1      = Player(1,            1, sf::Color(70,  130, 220));
    player2      = Player(MAP_COLS - 2, 1, sf::Color(60,  180, 80));
    bombs.clear();
    explosions.clear();
    powerUps.clear();
    enemies.clear();
    activeBombs1 = 0;
    activeBombs2 = 0;
    score        = 0;
    state        = GameState::Playing;
    spawnEnemies();
    refreshHUD();
}

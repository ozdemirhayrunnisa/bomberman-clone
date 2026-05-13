#include "Game.hpp"
#include "Constants.hpp"
#include <algorithm>

Game::Game()
    : window(sf::VideoMode({static_cast<unsigned>(WINDOW_WIDTH),
                            static_cast<unsigned>(WINDOW_HEIGHT)}),
             "Bomberman Clone",
             sf::Style::Titlebar | sf::Style::Close),
      player1(1, 1, sf::Color(70, 130, 220)) {

    window.setFramerateLimit(60);

    fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    if (fontLoaded) {
        auto makeText = [&](std::optional<sf::Text>& t, float x, unsigned sz) {
            t.emplace(font, "", sz);
            t->setFillColor(sf::Color::White);
            t->setPosition({x, static_cast<float>(HUD_TOP) + 12.f});
        };
        makeText(livesText, 10.f,  20);
        makeText(scoreText, 240.f, 20);
        makeText(infoText,  450.f, 18);
    }

    hudBar.setSize({static_cast<float>(WINDOW_WIDTH), static_cast<float>(HUD_HEIGHT)});
    hudBar.setPosition({0.f, static_cast<float>(HUD_TOP)});
    hudBar.setFillColor(sf::Color(35, 35, 55));

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
        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            if (key->code == sf::Keyboard::Key::Escape)
                window.close();
    }
}

void Game::update(float dt) {
    player1.handleInput(
        sf::Keyboard::Key::W, sf::Keyboard::Key::S,
        sf::Keyboard::Key::A, sf::Keyboard::Key::D,
        sf::Keyboard::Key::Space
    );
    player1.update(dt, map);

    if (player1.wantsBomb()) {
        tryPlaceBomb(player1.getCol(), player1.getRow());
        player1.consumeBombReq();
    }

    for (auto& bomb : bombs)
        bomb.update(dt);

    processExplosions();

    auto before = static_cast<int>(bombs.size());
    bombs.erase(
        std::remove_if(bombs.begin(), bombs.end(),
                       [](const Bomb& b) { return b.isExpired(); }),
        bombs.end()
    );
    activeBombs -= before - static_cast<int>(bombs.size());

    refreshHUD();
}

void Game::render() {
    window.clear(sf::Color(20, 20, 30));
    map.render(window);
    for (auto& bomb : bombs) bomb.render(window);
    player1.render(window);
    renderHUD();
    window.display();
}

void Game::renderHUD() {
    window.draw(hudBar);
    if (livesText) window.draw(*livesText);
    if (scoreText) window.draw(*scoreText);
    if (infoText)  window.draw(*infoText);
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

        map.setTile(bomb.getCol(), bomb.getRow(), TileType::Empty);

        for (int d = 0; d < 4; ++d) {
            for (int i = 1; i <= bomb.getRange(); ++i) {
                int nc = bomb.getCol() + dx[d] * i;
                int nr = bomb.getRow() + dy[d] * i;
                TileType t = map.getTile(nc, nr);
                if (t == TileType::Wall)  break;
                if (t == TileType::Block) {
                    map.setTile(nc, nr, TileType::Empty);
                    score += 10;
                    break;
                }
            }
        }
        bomb.setExpired();
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

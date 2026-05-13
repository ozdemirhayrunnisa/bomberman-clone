#pragma once

constexpr int   TILE_SIZE     = 48;
constexpr int   MAP_COLS      = 15;
constexpr int   MAP_ROWS      = 13;
constexpr int   HUD_HEIGHT    = 48;
constexpr int   WINDOW_WIDTH  = TILE_SIZE * MAP_COLS;
constexpr int   WINDOW_HEIGHT = TILE_SIZE * MAP_ROWS + HUD_HEIGHT;
constexpr int   HUD_TOP       = TILE_SIZE * MAP_ROWS;

constexpr float PLAYER_SPEED  = 3.5f * TILE_SIZE;
constexpr float BOMB_FUSE     = 3.0f;

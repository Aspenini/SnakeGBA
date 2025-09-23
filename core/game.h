// Portable game logic header for Snake
#pragma once
#include "platform.h"

// Game constants
#define MAX_SNAKE_LEN (30 * 20)  // Max grid size
#define TICK_MS 150              // Movement speed in milliseconds

// Game state
typedef enum {
    GAME_MENU,
    GAME_PLAYING,
    GAME_PAUSED,
    GAME_OVER
} GameState;

// Game data structure
typedef struct {
    // Snake data
    Vec2 snake[MAX_SNAKE_LEN];
    int snake_len;
    int dir_x, dir_y;
    
    // Food
    Vec2 food;
    
    // Game state
    GameState state;
    int score;
    int high_score;
    int level;
    
    // Grid for collision detection
    uint8_t grid[20][30]; // 0=empty, 1=snake, 2=food
    
    // Timing
    int frame_count;
    int move_timer;
    
    // Graphics info
    GfxInfo gfx;
} Game;

// Game functions
void game_init(Game* game);
void game_update(Game* game, uint32_t buttons);
void game_render(Game* game);
void game_reset(Game* game);
void game_spawn_food(Game* game);
void game_render_menu(Game* game);
void game_render_game(Game* game);
void game_render_pause(Game* game);
void game_render_game_over(Game* game);
void game_render_score(Game* game);

// Portable game logic for Snake - works on all platforms
#include "game.h"
#include <string.h>

// Initialize game
void game_init(Game* game) {
    memset(game, 0, sizeof(Game));
    
    game->gfx = plat_gfx_info();
    game->state = GAME_MENU;
    game->snake_len = 3;
    game->dir_x = 1;
    game->dir_y = 0;
    game->move_timer = 0;
    
    // Initialize snake in center
    int center_x = game->gfx.tiles_w / 2;
    int center_y = game->gfx.tiles_h / 2;
    
    game->snake[0].x = center_x;
    game->snake[0].y = center_y;
    game->snake[1].x = center_x - 1;
    game->snake[1].y = center_y;
    game->snake[2].x = center_x - 2;
    game->snake[2].y = center_y;
    
    // Initialize food position (simple placement)
    game->food.x = center_x + 3;
    game->food.y = center_y;
}

// Spawn food at random location
void game_spawn_food(Game* game) {
    // Simple approach: find first empty position
    // This is more reliable than random placement
    for (int fy = 0; fy < game->gfx.tiles_h; fy++) {
        for (int fx = 0; fx < game->gfx.tiles_w; fx++) {
            // Check if this position is empty
            int occupied = 0;
            for (int i = 0; i < game->snake_len; i++) {
                if (game->snake[i].x == fx && game->snake[i].y == fy) {
                    occupied = 1;
                    break;
                }
            }
            
            if (!occupied) {
                game->food.x = fx;
                game->food.y = fy;
                return;
            }
        }
    }
    
    // If no empty position found (shouldn't happen), place at top-left corner
    game->food.x = 0;
    game->food.y = 0;
}

// Update game state
void game_update(Game* game, uint32_t buttons) {
    game->frame_count++;
    
    // Handle input based on game state
    if (buttons & BTN_START) {
        if (game->state == GAME_MENU || game->state == GAME_OVER) {
            game_reset(game);
        } else if (game->state == GAME_PLAYING) {
            game->state = GAME_PAUSED;
        } else if (game->state == GAME_PAUSED) {
            game->state = GAME_PLAYING;
        }
    }
    
    if (game->state != GAME_PLAYING) return;
    
    // Handle movement input
    if (buttons & BTN_UP && game->dir_y != 1) {
        game->dir_x = 0; game->dir_y = -1;
    } else if (buttons & BTN_DOWN && game->dir_y != -1) {
        game->dir_x = 0; game->dir_y = 1;
    } else if (buttons & BTN_LEFT && game->dir_x != 1) {
        game->dir_x = -1; game->dir_y = 0;
    } else if (buttons & BTN_RIGHT && game->dir_x != -1) {
        game->dir_x = 1; game->dir_y = 0;
    }
    
    // Movement timing (fixed timestep)
    game->move_timer++;
    int move_frames = 60 * TICK_MS / 1000; // Convert ms to frames at 60fps
    if (game->move_timer < move_frames) return;
    game->move_timer = 0;
    
    // Calculate new head position
    int nx = game->snake[0].x + game->dir_x;
    int ny = game->snake[0].y + game->dir_y;
    
    // Wall wrapping
    if (nx < 0) nx = game->gfx.tiles_w - 1;
    if (ny < 0) ny = game->gfx.tiles_h - 1;
    if (nx >= game->gfx.tiles_w) nx = 0;
    if (ny >= game->gfx.tiles_h) ny = 0;
    
    // Check collision
    if (game->grid[ny][nx] == 1) {
        // Game over
        game->state = GAME_OVER;
        if (game->score > game->high_score) {
            game->high_score = game->score;
        }
        plat_beep_hit();
        return;
    }
    
    // Check food collision BEFORE moving snake
    int ate_food = 0;
    if (nx == game->food.x && ny == game->food.y) {
        ate_food = 1;
    }
    
    // Move snake
    for (int i = game->snake_len - 1; i > 0; i--) {
        game->snake[i] = game->snake[i - 1];
    }
    game->snake[0].x = nx;
    game->snake[0].y = ny;
    
    // If we ate food, grow snake
    if (ate_food) {
        // Grow snake
        if (game->snake_len < MAX_SNAKE_LEN) {
            game->snake_len++;
            game->snake[game->snake_len - 1] = game->snake[game->snake_len - 2];
        }
        
        // Update score
        game->score += 10;
        if (game->score > game->high_score) {
            game->high_score = game->score;
        }
        
        // Level up every 5 food
        game->level = (game->score / 50) + 1;
        
        plat_beep_ok();
    }
    
    // Update grid with new snake positions
    memset(game->grid, 0, sizeof(game->grid));
    for (int i = 0; i < game->snake_len; i++) {
        game->grid[game->snake[i].y][game->snake[i].x] = 1;
    }
    
    // Spawn new food AFTER grid update (if we ate food)
    if (ate_food) {
        game_spawn_food(game);
    }
}

// Render game
void game_render(Game* game) {
    // Clear screen
    plat_clear_bg();
    
    if (game->state == GAME_MENU) {
        // Draw menu
        game_render_menu(game);
    } else if (game->state == GAME_PLAYING || game->state == GAME_PAUSED) {
        // Draw game
        game_render_game(game);
        
        if (game->state == GAME_PAUSED) {
            game_render_pause(game);
        }
    } else if (game->state == GAME_OVER) {
        // Draw game over
        game_render_game_over(game);
    }
    
    plat_present();
}

// Render menu screen
void game_render_menu(Game* game) {
    // Draw logo (8×4 tiles, centered)
    int logo_x = (game->gfx.tiles_w - 8) / 2;  // Center horizontally
    int logo_y = 6;  // Position from top
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 8; x++) {
            int tile_index = y * 8 + x;
            plat_put_tile(logo_x + x, logo_y + y, tile_index, 0); // Logo tiles start at 0
        }
    }
    
    // Draw "PRESS START" below logo
    int start_y = logo_y + 6;  // Below logo
    int start_x = (game->gfx.tiles_w - 11) / 2;  // Center "PRESS START"
    
    plat_put_tile(start_x + 0, start_y, 32, 1);  // P
    plat_put_tile(start_x + 1, start_y, 33, 1);  // R
    plat_put_tile(start_x + 2, start_y, 34, 1);  // E
    plat_put_tile(start_x + 3, start_y, 35, 1);  // S
    plat_put_tile(start_x + 4, start_y, 35, 1);  // S
    plat_put_tile(start_x + 6, start_y, 36, 1);  // START
    plat_put_tile(start_x + 7, start_y, 37, 1);  // T
    plat_put_tile(start_x + 8, start_y, 38, 1);  // A
    plat_put_tile(start_x + 9, start_y, 39, 1);  // R
    plat_put_tile(start_x + 10, start_y, 40, 1); // T
}

// Render game screen
void game_render_game(Game* game) {
    // Draw snake using sprites
    for (int i = 0; i < game->snake_len; i++) {
        int px = game->snake[i].x * game->gfx.tile_px;
        int py = game->snake[i].y * game->gfx.tile_px;
        
        uint16_t tile = (i == 0) ? 10 : 11; // Head or body tile
        uint8_t pal = (i == 0) ? 2 : 3;     // Head or body palette
        
        plat_sprite_set(i, px, py, tile, pal);
    }
    
    // Hide unused sprites
    for (int i = game->snake_len; i < MAX_SNAKE_LEN; i++) {
        plat_sprite_hide(i);
    }
    
    // Draw food (only if coordinates are valid)
    if (game->food.x >= 0 && game->food.x < game->gfx.tiles_w && 
        game->food.y >= 0 && game->food.y < game->gfx.tiles_h) {
        int food_px = game->food.x * game->gfx.tile_px;
        int food_py = game->food.y * game->gfx.tile_px;
        plat_sprite_set(MAX_SNAKE_LEN, food_px, food_py, 12, 1); // Food tile, red palette
    } else {
        // Hide food sprite if coordinates are invalid
        plat_sprite_hide(MAX_SNAKE_LEN);
    }
    
    // Draw score
    game_render_score(game);
}

// Render score
void game_render_score(Game* game) {
    // Simple score display using tiles
    int score = game->score;
    int pos = 2; // Start position
    
    // Draw score digits
    do {
        int digit = score % 10;
        plat_put_tile(pos, 0, 20 + digit, 0); // Score tiles start at 20
        score /= 10;
        pos--;
    } while (score > 0 && pos >= 0);
}

// Render pause screen
void game_render_pause(Game* game) {
    // Draw "PAUSED" overlay
    plat_put_tile(13, 8, 30, 2); // P
    plat_put_tile(14, 8, 31, 2); // A
    plat_put_tile(15, 8, 32, 2); // U
    plat_put_tile(16, 8, 33, 2); // S
    plat_put_tile(17, 8, 34, 2); // E
    plat_put_tile(18, 8, 35, 2); // D
}

// Render game over screen
void game_render_game_over(Game* game) {
    // Draw "GAME OVER"
    plat_put_tile(12, 8, 40, 2); // G
    plat_put_tile(13, 8, 41, 2); // A
    plat_put_tile(14, 8, 42, 2); // M
    plat_put_tile(15, 8, 43, 2); // E
    plat_put_tile(17, 8, 44, 2); // O
    plat_put_tile(18, 8, 45, 2); // V
    plat_put_tile(19, 8, 46, 2); // E
    plat_put_tile(20, 8, 47, 2); // R
    
    // Draw final score
    game_render_score(game);
}

// Reset game
void game_reset(Game* game) {
    game->state = GAME_PLAYING;
    game->score = 0;
    game->level = 1;
    game->snake_len = 3;
    game->dir_x = 1;
    game->dir_y = 0;
    game->move_timer = 0;
    
    // Reset snake position
    int center_x = game->gfx.tiles_w / 2;
    int center_y = game->gfx.tiles_h / 2;
    
    game->snake[0].x = center_x;
    game->snake[0].y = center_y;
    game->snake[1].x = center_x - 1;
    game->snake[1].y = center_y;
    game->snake[2].x = center_x - 2;
    game->snake[2].y = center_y;
    
    // Initialize food position (simple placement)
    game->food.x = center_x + 3;
    game->food.y = center_y;
}

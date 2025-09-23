// Platform abstraction layer for multi-platform Snake game
// This interface allows the same game logic to run on GBA, NDS, and GameCube

#pragma once
#include <stdint.h>

// Common types
typedef struct { 
    int x, y; 
} Vec2;

// Button enumeration - maps to platform-specific inputs
typedef enum {
    BTN_UP    = 1<<0,
    BTN_DOWN  = 1<<1, 
    BTN_LEFT  = 1<<2,
    BTN_RIGHT = 1<<3,
    BTN_A     = 1<<4,
    BTN_B     = 1<<5,
    BTN_START = 1<<6,
    BTN_SELECT= 1<<7
} Buttons;

// Graphics info - screen dimensions in tiles
typedef struct {
    int tiles_w, tiles_h;  // GBA: 30x20, NDS: 32x24
    int tile_px;           // 8 pixels per tile
} GfxInfo;

// Platform initialization
void plat_init(void);             // Initialize video, IRQ, etc.

// Frame timing
void plat_vblank(void);           // Wait for VBlank

// Input
uint32_t plat_buttons(void);      // Returns bitmask of Buttons

// Background rendering
void plat_clear_bg(void);         // Clear BG tilemap
void plat_put_tile(int tx, int ty, uint16_t tileIndex, uint8_t pal); // Put BG tile

// Sprite rendering (for snake segments and food)
void plat_sprite_set(int id, int px, int py, uint16_t tileIndex, uint8_t pal);     // Set sprite
void plat_sprite_hide(int id);    // Hide sprite
void plat_present(void);          // Commit OAM if needed

// Graphics info
GfxInfo plat_gfx_info(void);      // Get screen dimensions

// Asset loading
void plat_load_assets(const uint16_t* bgPal, const uint8_t* bgTiles, int bgTilesLen,
                      const uint16_t* objPal, const uint8_t* objTiles, int objTilesLen);

// Audio (optional)
void plat_beep_ok(void);          // Play success sound
void plat_beep_hit(void);         // Play collision sound

// Random number generation
void plat_seed_random(uint32_t seed);
uint32_t plat_random(void);       // Returns random number

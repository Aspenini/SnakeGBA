// GBA platform implementation for Snake game - simplified version
#include <gba.h>
#include <stdlib.h>
#include "platform.h"

// GBA-specific constants
#define GBA_TILES_W 30
#define GBA_TILES_H 20
#define GBA_TILE_PX 8

// Screen buffer for Mode 3
static volatile u16* const videoBuffer = (u16*)0x06000000;

// Initialize GBA hardware
void plat_init(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);
    
    // Mode 3: bitmap mode for simplicity
    REG_DISPCNT = MODE_3 | BG2_ON;
}

void plat_vblank(void) {
    VBlankIntrWait();
}

uint32_t plat_buttons(void) {
    scanKeys();
    u16 keys = keysDown();
    
    uint32_t buttons = 0;
    if (keys & KEY_UP)    buttons |= BTN_UP;
    if (keys & KEY_DOWN)  buttons |= BTN_DOWN;
    if (keys & KEY_LEFT)  buttons |= BTN_LEFT;
    if (keys & KEY_RIGHT) buttons |= BTN_RIGHT;
    if (keys & KEY_A)     buttons |= BTN_A;
    if (keys & KEY_B)     buttons |= BTN_B;
    if (keys & KEY_START) buttons |= BTN_START;
    if (keys & KEY_SELECT) buttons |= BTN_SELECT;
    
    return buttons;
}

void plat_clear_bg(void) {
    // Clear screen to black
    DMA3COPY(&(u16){0}, videoBuffer, DMA_SRC_FIXED | DMA16 | (240*160));
}

void plat_put_tile(int tx, int ty, uint16_t tileIndex, uint8_t pal) {
    // Simple tile drawing using Mode 3
    if (tx < 0 || tx >= 30 || ty < 0 || ty >= 20) return;
    
    int px = tx * 8;
    int py = ty * 8;
    u16 color = RGB5(31, 31, 31); // White for now
    
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            videoBuffer[(py + y) * 240 + (px + x)] = color;
        }
    }
}

void plat_sprite_set(int id, int px, int py, uint16_t tileIndex, uint8_t pal) {
    // Simple sprite drawing using Mode 3
    if (px < 0 || px >= 240 || py < 0 || py >= 160) return;
    
    u16 color;
    switch (pal) {
        case 1: color = RGB5(31, 0, 0); break;   // Red
        case 2: color = RGB5(31, 31, 31); break; // White
        case 3: color = RGB5(0, 31, 0); break;   // Green
        default: color = RGB5(31, 31, 0); break; // Yellow
    }
    
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (px + x < 240 && py + y < 160) {
                videoBuffer[(py + y) * 240 + (px + x)] = color;
            }
        }
    }
}

void plat_sprite_hide(int id) {
    // Nothing to do in Mode 3
}

void plat_present(void) {
    // Nothing to do in Mode 3
}

GfxInfo plat_gfx_info(void) {
    GfxInfo info = {
        .tiles_w = GBA_TILES_W,
        .tiles_h = GBA_TILES_H,
        .tile_px = GBA_TILE_PX
    };
    return info;
}

void plat_load_assets(const uint16_t* bgPal, const uint8_t* bgTiles, int bgTilesLen,
                      const uint16_t* objPal, const uint8_t* objTiles, int objTilesLen) {
    // No assets needed for simple Mode 3 version
}

void plat_beep_ok(void) {
    // Simple beep - no sound for now
}

void plat_beep_hit(void) {
    // Simple beep - no sound for now
}

void plat_seed_random(uint32_t seed) {
    srand(seed);
}

uint32_t plat_random(void) {
    return rand();
}
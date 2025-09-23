// NDS platform implementation for Snake game
#include <nds.h>
#include "platform.h"

// NDS-specific constants
#define NDS_TILES_W 32
#define NDS_TILES_H 24
#define NDS_TILE_PX 8

// Initialize NDS hardware
void plat_init(void) {
    // Initialize video
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    
    // Set up VRAM banks
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_SPRITE);
    vramSetBankC(VRAM_C_SUB_BG);
    
    // Initialize main screen BG0
    bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    
    // Initialize sprites
    oamInit(&oamMain, SpriteMapping_1D_32, false);
    
    // Enable interrupts
    irqInit();
    irqEnable(IRQ_VBLANK);
}

void plat_vblank(void) {
    swiWaitForVBlank();
}

uint32_t plat_buttons(void) {
    scanKeys();
    u16 keys = keysHeld();
    
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
    // Clear BG0 tilemap
    u16* bgMap = bgGetMapPtr(0);
    for (int i = 0; i < 32*32; i++) {
        bgMap[i] = 0;
    }
}

void plat_put_tile(int tx, int ty, uint16_t tileIndex, uint8_t pal) {
    if (tx < 0 || tx >= 32 || ty < 0 || ty >= 32) return;
    
    u16* bgMap = bgGetMapPtr(0);
    bgMap[ty * 32 + tx] = tileIndex | (pal << 12);
}

void plat_sprite_set(int id, int px, int py, uint16_t tileIndex, uint8_t pal) {
    if (id < 0 || id >= 128) return;
    
    oamSet(&oamMain, id, px, py, 0, pal, SpriteSize_8x8, SpriteColorFormat_16Color,
           (u8*)SPRITE_GFX + tileIndex * 32, -1, false, false, false, false, false);
}

void plat_sprite_hide(int id) {
    if (id < 0 || id >= 128) return;
    
    oamSet(&oamMain, id, 0, 0, 0, 0, SpriteSize_8x8, SpriteColorFormat_16Color,
           0, -1, false, false, false, false, false);
}

void plat_present(void) {
    // Update OAM
    oamUpdate(&oamMain);
}

GfxInfo plat_gfx_info(void) {
    GfxInfo info = {
        .tiles_w = NDS_TILES_W,
        .tiles_h = NDS_TILES_H,
        .tile_px = NDS_TILE_PX
    };
    return info;
}

void plat_load_assets(const uint16_t* bgPal, const uint8_t* bgTiles, int bgTilesLen,
                      const uint16_t* objPal, const uint8_t* objTiles, int objTilesLen) {
    
    // Load background palette
    if (bgPal) {
        dmaCopy(bgPal, BG_PALETTE, 16 * 2); // 16 colors * 2 bytes
    }
    
    // Load background tiles
    if (bgTiles) {
        dmaCopy(bgTiles, bgGetGfxPtr(0), bgTilesLen);
    }
    
    // Load sprite palette
    if (objPal) {
        dmaCopy(objPal, SPRITE_PALETTE, 16 * 2); // 16 colors * 2 bytes
    }
    
    // Load sprite tiles
    if (objTiles) {
        dmaCopy(objTiles, SPRITE_GFX, objTilesLen);
    }
}

void plat_beep_ok(void) {
    // Simple beep using NDS sound
    soundPlaySample(0, SoundFormat_8Bit, 440, 64, 64, 0, 0);
}

void plat_beep_hit(void) {
    // Lower frequency beep
    soundPlaySample(0, SoundFormat_8Bit, 220, 64, 64, 0, 0);
}

void plat_seed_random(uint32_t seed) {
    srand(seed);
}

uint32_t plat_random(void) {
    return rand();
}

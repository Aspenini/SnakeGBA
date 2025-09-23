// Main entry point for multi-platform Snake game
#include <stddef.h>
#include "core/game.h"

// Game instance
static Game game;

int main(void) {
    // Initialize platform
    plat_init();
    
    // Load assets (platform-specific)
    plat_load_assets(NULL, NULL, 0, NULL, NULL, 0);
    
    // Initialize game
    game_init(&game);
    
    // Seed random number generator
    plat_seed_random(0x12345678);
    
    // Main game loop
    while (1) {
        // Wait for VBlank
        plat_vblank();
        
        // Get input
        uint32_t buttons = plat_buttons();
        
        // Update game logic
        game_update(&game, buttons);
        
        // Render
        game_render(&game);
    }
    
    return 0;
}

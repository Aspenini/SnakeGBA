# Multi-Platform Snake Game 🐍✨

A portable Snake game that runs on Game Boy Advance, Nintendo DS, and (planned) GameCube using a clean architecture with separated platform layers.

## 🏗️ Architecture

The game uses a **clean separation** between portable game logic and platform-specific hardware details:

```
/snake
  /core
    game.h           // Game logic interface
    game.c           // All platform-agnostic logic
  /platform
    platform.h       // Tiny interface used by core
    gba.c            // GBA implementation
    nds.c            // NDS implementation
    ngc.c            // GameCube implementation (planned)
  /assets
    tiles.png        // Shared 4bpp tile art (planned)
  /build
  main.c             // Entry point
  Makefile           // Multi-platform build system
```

## 🎮 Features

- **Portable Game Logic**: All Snake gameplay in pure C
- **Platform Abstraction**: Clean API hides hardware differences
- **Multi-Platform**: GBA, NDS, and GameCube support
- **Efficient Rendering**: Uses appropriate graphics modes per platform
- **Red Food**: Bright red food as requested! 🍎

## 🛠️ Building

### Prerequisites
- devkitPro with devkitARM and devkitPPC
- Make

### Build Commands
```bash
# Build for GBA (default)
make PLATFORM=gba
# or
./build.bat gba

# Build for NDS
make PLATFORM=nds
# or
./build.bat nds

# Build for GameCube (planned)
make PLATFORM=ngc
# or
./build.bat ngc

# Build all platforms
make all-platforms
# or
./build.bat all
```

## 🎯 Platform Details

### GBA (Game Boy Advance)
- **Graphics**: Mode 3 bitmap rendering
- **Resolution**: 240×160 (30×20 grid)
- **Input**: D-pad, A/B, Start/Select
- **File**: `snake.gba`

### NDS (Nintendo DS)
- **Graphics**: Main engine BG0 + OBJ sprites
- **Resolution**: 256×192 (32×24 grid)
- **Input**: D-pad, A/B, Start/Select
- **File**: `snake.nds`

### GameCube (Planned)
- **Graphics**: GX textured quads
- **Resolution**: 640×480 (upscaled tiles)
- **Input**: GameCube controller
- **File**: `snake.dol`

## 🎮 Controls

- **D-Pad**: Move snake (prevents 180° turns)
- **START**: Start game, pause/unpause, restart
- **A/B**: Reserved for future features

## 🧩 Platform Interface

The core game only calls these functions:

```c
// Platform initialization
void plat_init(void);
void plat_vblank(void);

// Input
uint32_t plat_buttons(void);

// Rendering
void plat_clear_bg(void);
void plat_put_tile(int tx, int ty, uint16_t tileIndex, uint8_t pal);
void plat_sprite_set(int id, int px, int py, uint16_t tileIndex, uint8_t pal);
void plat_sprite_hide(int id);
void plat_present(void);

// Graphics info
GfxInfo plat_gfx_info(void);

// Assets & audio
void plat_load_assets(...);
void plat_beep_ok(void);
void plat_beep_hit(void);

// Random
void plat_seed_random(uint32_t seed);
uint32_t plat_random(void);
```

## 🎨 Game Logic

The core game logic is completely platform-agnostic:

- **Grid-based movement**: Snake moves on a tile grid
- **Collision detection**: Simple grid array for fast checks
- **Scoring system**: Points for eating food
- **Game states**: Menu, Playing, Paused, Game Over
- **Fixed timestep**: Consistent movement speed

## 🚀 Benefits of This Architecture

1. **One Codebase**: Same game logic runs everywhere
2. **Easy to Extend**: Add new platforms by implementing the interface
3. **Efficient**: Each platform uses optimal rendering methods
4. **Maintainable**: Clear separation of concerns
5. **Testable**: Core logic can be tested independently

## 📁 File Structure

```
SnakeGBA/
├── core/
│   ├── game.h          # Game logic interface
│   └── game.c          # Portable game implementation
├── platform/
│   ├── platform.h      # Platform abstraction interface
│   ├── gba.c           # GBA hardware implementation
│   └── nds.c           # NDS hardware implementation
├── assets/             # Shared graphics assets (planned)
├── build/              # Build output directory
├── main.c              # Entry point
├── Makefile            # Multi-platform build system
├── build.bat           # Windows build script
└── README.md           # This file
```

## 🎯 Current Status

- ✅ **GBA**: Fully working with Mode 3 bitmap rendering
- ✅ **NDS**: Platform layer implemented (needs testing)
- ⏳ **GameCube**: Planned for future
- ⏳ **Assets**: Shared tile graphics (planned)
- ⏳ **Audio**: Sound effects (planned)

## 🔧 Technical Details

### GBA Implementation
- Uses Mode 3 bitmap mode for simplicity
- DMA-based screen clearing
- Direct pixel manipulation for sprites
- 30×20 grid with 8×8 pixel tiles

### NDS Implementation
- Uses main engine BG0 + OBJ sprites
- 32×24 grid (larger than GBA)
- libnds APIs for graphics and input
- OAM-based sprite management

The architecture makes it trivial to add new platforms - just implement the `platform.h` interface and add a new build target to the Makefile!

## 🎮 Enjoy Your Multi-Platform Snake Game!

The game demonstrates how to create truly portable console games using clean architecture principles. The same Snake logic runs on multiple Nintendo platforms with platform-optimized rendering! 🐍✨

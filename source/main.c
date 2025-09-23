#include <gba.h>
#include <stdio.h>
#include <stdlib.h>

// Screen dimensions
#define SCREEN_W 240
#define SCREEN_H 160

// Video buffer
static volatile u16* const videoBuffer = (u16*)0x06000000;

// Helper functions
static inline void putPixel(int x, int y, u16 color) {
    if ((unsigned)x < SCREEN_W && (unsigned)y < SCREEN_H)
        videoBuffer[y * SCREEN_W + x] = color;
}

static void fillScreen(u16 color) {
    DMA3COPY(&color, videoBuffer, DMA_SRC_FIXED | DMA16 | (SCREEN_W*SCREEN_H));
}

static void fillRect(int x, int y, int w, int h, u16 color) {
    if (w <= 0 || h <= 0) return;
    int x0 = x, x1 = x + w - 1;
    int y0 = y, y1 = y + h - 1;
    if (x0 < 0) x0 = 0; 
    if (y0 < 0) y0 = 0;
    if (x1 >= SCREEN_W)  x1 = SCREEN_W - 1;
    if (y1 >= SCREEN_H)  y1 = SCREEN_H - 1;
    for (int j = y0; j <= y1; ++j) {
        DMA3COPY(&color, &videoBuffer[j*SCREEN_W + x0],
                 DMA_SRC_FIXED | DMA16 | (x1 - x0 + 1));
    }
}

// Game constants
#define GRID_W 30
#define GRID_H 20
#define MAX_LEN (GRID_W * GRID_H)

// Grid positions
typedef struct {
    int x, y;
} Point;

// Game state
static Point snake[MAX_LEN];
static int snake_len = 3;
static int dir_x = 1;
static int dir_y = 0;
static Point food;
static int score = 0;
static int game_started = 0;
static u8 grid[GRID_H][GRID_W]; // 0=empty, 1=snake, 2=food

// Simple font data for numbers (4x8 pixels each)
static const u8 font_numbers[10][8] = {
    {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E, 0x00}, // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E, 0x00}, // 1
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F, 0x00}, // 2
    {0x0E, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0E, 0x00}, // 3
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02, 0x00}, // 4
    {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E, 0x00}, // 5
    {0x0E, 0x10, 0x1E, 0x11, 0x11, 0x11, 0x0E, 0x00}, // 6
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08, 0x00}, // 7
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E, 0x00}, // 8
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x11, 0x0E, 0x00}  // 9
};

// Simple RNG
static int randRange(int lo, int hi) {
    return lo + (rand() % (hi - lo + 1));
}

// Clear grid
static void clearGrid(void) {
    for (int y = 0; y < GRID_H; y++) {
        for (int x = 0; x < GRID_W; x++) {
            grid[y][x] = 0;
        }
    }
}

// Spawn food
static void spawnFood(void) {
    while (1) {
        int fx = randRange(0, GRID_W - 1);
        int fy = randRange(0, GRID_H - 1);
        if (grid[fy][fx] == 0) {
            food.x = fx;
            food.y = fy;
            grid[fy][fx] = 2;
            return;
        }
    }
}

// Reset game
static void resetGame(void) {
    clearGrid();
    
    // Initialize snake
    snake_len = 3;
    snake[0].x = GRID_W/2;
    snake[0].y = GRID_H/2;
    snake[1].x = GRID_W/2 - 1;
    snake[1].y = GRID_H/2;
    snake[2].x = GRID_W/2 - 2;
    snake[2].y = GRID_H/2;
    
    // Set snake in grid
    for (int i = 0; i < snake_len; i++) {
        grid[snake[i].y][snake[i].x] = 1;
    }
    
    dir_x = 1;
    dir_y = 0;
    score = 0;
    game_started = 1;
    spawnFood();
}

// Check collision
static int checkCollision(int x, int y) {
    if (x < 0 || x >= GRID_W || y < 0 || y >= GRID_H) return 1;
    return grid[y][x] == 1;
}

// Draw number on screen
static void drawNumber(int x, int y, int number, u16 color) {
    char str[16];
    sprintf(str, "%d", number);
    
    for (int i = 0; str[i] != '\0'; i++) {
        int digit = str[i] - '0';
        if (digit >= 0 && digit <= 9) {
            for (int py = 0; py < 8; py++) {
                u8 line = font_numbers[digit][py];
                for (int px = 0; px < 8; px++) {
                    if (line & (0x80 >> px)) {
                        putPixel(x + i * 8 + px, y + py, color);
                    }
                }
            }
        }
    }
}

// Draw game screen
static void drawGame(void) {
    fillScreen(RGB5(0, 0, 0)); // Black background
    
    // Draw grid lines
    for (int y = 0; y < SCREEN_H; y += 8) {
        for (int x = 0; x < SCREEN_W; x++) {
            putPixel(x, y, RGB5(8, 8, 8)); // Dark gray
        }
    }
    for (int x = 0; x < SCREEN_W; x += 8) {
        for (int y = 0; y < SCREEN_H; y++) {
            putPixel(x, y, RGB5(8, 8, 8)); // Dark gray
        }
    }
    
    // Draw snake
    for (int i = 0; i < snake_len; i++) {
        int px = snake[i].x * 8;
        int py = snake[i].y * 8;
        u16 color = (i == 0) ? RGB5(31, 31, 31) : RGB5(0, 31, 0); // White head, green body
        fillRect(px, py, 8, 8, color);
    }
    
    // Draw food (RED!)
    fillRect(food.x * 8, food.y * 8, 8, 8, RGB5(31, 0, 0)); // Red food
    
    // Draw score
    drawNumber(8, 8, score, RGB5(31, 31, 31)); // White text
}

// Draw menu
static void drawMenu(void) {
    fillScreen(RGB5(0, 0, 0)); // Black background
    
    // Draw "SNAKE" title
    fillRect(100, 60, 40, 10, RGB5(31, 31, 31)); // White
    fillRect(100, 70, 10, 30, RGB5(31, 31, 31)); // White
    fillRect(130, 70, 10, 30, RGB5(31, 31, 31)); // White
    
    // Draw "PRESS START"
    fillRect(80, 100, 80, 10, RGB5(0, 31, 0)); // Green
    
    // Draw "D-PAD TO MOVE"
    fillRect(70, 120, 100, 10, RGB5(31, 31, 0)); // Yellow
}

int main(void) {
    // Initialize GBA
    irqInit();
    irqEnable(IRQ_VBLANK);
    
    // Mode 3 bitmap mode
    REG_DISPCNT = MODE_3 | BG2_ON;
    
    // Seed RNG
    srand(0x12345678);
    
    int frames = 0;
    const int moveDelay = 8;
    
    while (1) {
        VBlankIntrWait();
        frames++;
        
        // Input
        scanKeys();
        u16 kd = keysDown();
        
        if (kd & KEY_START) {
            if (!game_started) {
                resetGame();
            } else {
                // Restart game
                resetGame();
            }
        }
        
        if (game_started) {
            // Change direction
            if ((kd & KEY_UP)    && dir_y != 1)  { dir_x = 0; dir_y = -1; }
            if ((kd & KEY_DOWN)  && dir_y != -1) { dir_x = 0; dir_y = 1; }
            if ((kd & KEY_LEFT)  && dir_x != 1)  { dir_x = -1; dir_y = 0; }
            if ((kd & KEY_RIGHT) && dir_x != -1) { dir_x = 1; dir_y = 0; }
            
            // Movement
            if (frames % moveDelay == 0) {
                // Calculate new head position
                int nx = snake[0].x + dir_x;
                int ny = snake[0].y + dir_y;
                
                // Wall wrapping
                if (nx < 0) nx = GRID_W - 1;
                if (ny < 0) ny = GRID_H - 1;
                if (nx >= GRID_W) nx = 0;
                if (ny >= GRID_H) ny = 0;
                
                // Check collision
                if (checkCollision(nx, ny)) {
                    // Game over - flash red
                    fillScreen(RGB5(31, 0, 0));
                    for (int i = 0; i < 30; i++) VBlankIntrWait();
                    resetGame();
                    continue;
                }
                
                // Move snake
                for (int i = snake_len - 1; i > 0; i--) {
                    snake[i] = snake[i - 1];
                }
                snake[0].x = nx;
                snake[0].y = ny;
                
                // Update grid
                clearGrid();
                for (int i = 0; i < snake_len; i++) {
                    grid[snake[i].y][snake[i].x] = 1;
                }
                
                // Check food
                if (nx == food.x && ny == food.y) {
                    if (snake_len < MAX_LEN) {
                        snake_len++;
                        snake[snake_len - 1] = snake[snake_len - 2];
                    }
                    score += 10;
                    spawnFood();
                }
            }
            
            // Render
            drawGame();
        } else {
            // Show menu
            drawMenu();
        }
    }
    
    return 0;
}
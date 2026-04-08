/*
 * NEON SURF
 * A Futuristic Endless Runner Game
 *
 * Mini-project for C Programming Course
 * Using Raylib graphics library
 *
 * Controls:
 *   A / Left Arrow  - Move left
 *   D / Right Arrow - Move right
 *   Space / W / Up  - Jump
 *   ESC             - Exit / Menu
 */

#include "game.h"
#include <stdlib.h>
#include <time.h>

int main(void) {
    // Initialize random seed
    srand((unsigned int)time(NULL));

    // Initialize Raylib window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NEON SURF - Endless Runner");
    SetTargetFPS(60);

    // Initialize game
    Game game;
    GameInit(&game);

    // Main game loop
    while (!WindowShouldClose()) {
        // Get delta time
        float deltaTime = GetFrameTime();

        // Update game
        GameUpdate(&game, deltaTime);

        // Render game
        RenderGame(&game);
    }

    // Cleanup
    GameCleanup(&game);
    CloseWindow();

    return 0;
}

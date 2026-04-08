#include "game.h"
#include <stdlib.h>
#include <string.h>

void GameInit(Game* game) {
    // Initialize game state
    game->state = STATE_MENU;
    game->menuSelection = MENU_PLAY;
    game->showHowToPlay = false;
    game->score = 0;
    game->highScore = 0;
    game->distance = 0.0f;
    game->speed = INITIAL_SPEED;
    game->obstacleSpawnTimer = 0.0f;
    game->coinSpawnTimer = 0.0f;
    game->obstacleSpawnInterval = 1.5f;
    game->coinSpawnInterval = 0.8f;

    // Initialize player
    PlayerInit(&game->player);

    // Initialize obstacles array
    game->obstacleCapacity = INITIAL_OBSTACLES;
    game->obstacleCount = 0;
    game->obstacles = (Obstacle*)malloc(sizeof(Obstacle) * game->obstacleCapacity);
    if (game->obstacles) {
        memset(game->obstacles, 0, sizeof(Obstacle) * game->obstacleCapacity);
    }

    // Initialize coins array
    game->coinCapacity = INITIAL_COINS;
    game->coinCount = 0;
    game->coins = (Coin*)malloc(sizeof(Coin) * game->coinCapacity);
    if (game->coins) {
        memset(game->coins, 0, sizeof(Coin) * game->coinCapacity);
    }
}

void GameUpdate(Game* game, float deltaTime) {
    switch (game->state) {
        case STATE_MENU:
            // Handle "How to Play" overlay
            if (game->showHowToPlay) {
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    game->showHowToPlay = false;
                }
                break;
            }

            // Menu navigation
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                if (game->menuSelection > 0) {
                    game->menuSelection--;
                } else {
                    game->menuSelection = MENU_COUNT - 1;
                }
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                if (game->menuSelection < MENU_COUNT - 1) {
                    game->menuSelection++;
                } else {
                    game->menuSelection = 0;
                }
            }

            // Menu selection
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                switch (game->menuSelection) {
                    case MENU_PLAY:
                        game->state = STATE_PLAYING;
                        GameReset(game);
                        break;
                    case MENU_LEVEL:
                        // Level selection (for now, just start game)
                        game->state = STATE_PLAYING;
                        GameReset(game);
                        break;
                    case MENU_HOW_TO_PLAY:
                        game->showHowToPlay = true;
                        break;
                    default:
                        break;
                }
            }
            break;

        case STATE_PLAYING:
            // Handle player input
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                PlayerMoveLeft(&game->player);
            }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                PlayerMoveRight(&game->player);
            }
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                PlayerJump(&game->player);
            }

            // Update player
            PlayerUpdate(&game->player, deltaTime);

            // Update obstacles
            ObstaclesUpdate(game, deltaTime);

            // Update coins
            CoinsUpdate(game, deltaTime);

            // Check collisions
            if (ObstacleCheckCollision(game)) {
                game->state = STATE_GAME_OVER;
                if (game->score > game->highScore) {
                    game->highScore = game->score;
                }
            }

            // Check coin collection
            CoinCheckCollection(game);

            // Update distance and speed
            game->distance += game->speed * deltaTime;
            if (game->speed < MAX_SPEED) {
                game->speed += SPEED_INCREMENT * deltaTime;
            }

            // Spawn new obstacles
            game->obstacleSpawnTimer += deltaTime;
            if (game->obstacleSpawnTimer >= game->obstacleSpawnInterval) {
                ObstacleSpawn(game);
                game->obstacleSpawnTimer = 0.0f;
                // Decrease spawn interval for difficulty
                if (game->obstacleSpawnInterval > 0.5f) {
                    game->obstacleSpawnInterval -= 0.01f;
                }
            }

            // Spawn new coins
            game->coinSpawnTimer += deltaTime;
            if (game->coinSpawnTimer >= game->coinSpawnInterval) {
                CoinSpawn(game);
                game->coinSpawnTimer = 0.0f;
            }
            break;

        case STATE_GAME_OVER:
            // Wait for player input to restart
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                game->state = STATE_PLAYING;
                GameReset(game);
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                game->state = STATE_MENU;
            }
            break;
    }
}

void GameReset(Game* game) {
    // Reset player
    PlayerInit(&game->player);

    // Clear obstacles
    game->obstacleCount = 0;
    if (game->obstacles) {
        memset(game->obstacles, 0, sizeof(Obstacle) * game->obstacleCapacity);
    }

    // Clear coins
    game->coinCount = 0;
    if (game->coins) {
        memset(game->coins, 0, sizeof(Coin) * game->coinCapacity);
    }

    // Reset game parameters
    game->score = 0;
    game->distance = 0.0f;
    game->speed = INITIAL_SPEED;
    game->obstacleSpawnTimer = 0.0f;
    game->coinSpawnTimer = 0.0f;
    game->obstacleSpawnInterval = 1.5f;
    game->coinSpawnInterval = 0.8f;
}

void GameCleanup(Game* game) {
    // Free obstacles array
    if (game->obstacles) {
        free(game->obstacles);
        game->obstacles = NULL;
    }
    game->obstacleCount = 0;
    game->obstacleCapacity = 0;

    // Free coins array
    if (game->coins) {
        free(game->coins);
        game->coins = NULL;
    }
    game->coinCount = 0;
    game->coinCapacity = 0;
}

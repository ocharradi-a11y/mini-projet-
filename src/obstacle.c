#include "game.h"
#include <stdlib.h>
#include <string.h>

void ObstaclesInit(Game* game) {
    game->obstacleCapacity = INITIAL_OBSTACLES;
    game->obstacleCount = 0;
    game->obstacles = (Obstacle*)malloc(sizeof(Obstacle) * game->obstacleCapacity);
    if (game->obstacles) {
        memset(game->obstacles, 0, sizeof(Obstacle) * game->obstacleCapacity);
    }
}

void ObstaclesUpdate(Game* game, float deltaTime) {
    for (int i = 0; i < game->obstacleCount; i++) {
        if (game->obstacles[i].active) {
            // Move obstacle toward player (decrease z)
            game->obstacles[i].z -= game->speed * deltaTime;

            // Deactivate if passed player
            if (game->obstacles[i].z < -100.0f) {
                game->obstacles[i].active = false;
            }
        }
    }

    // Compact array - remove inactive obstacles
    int writeIndex = 0;
    for (int i = 0; i < game->obstacleCount; i++) {
        if (game->obstacles[i].active) {
            if (writeIndex != i) {
                game->obstacles[writeIndex] = game->obstacles[i];
            }
            writeIndex++;
        }
    }
    game->obstacleCount = writeIndex;
}

void ObstacleSpawn(Game* game) {
    // Expand array if needed
    if (game->obstacleCount >= game->obstacleCapacity) {
        int newCapacity = game->obstacleCapacity * 2;
        Obstacle* newArray = (Obstacle*)realloc(game->obstacles, sizeof(Obstacle) * newCapacity);
        if (newArray) {
            game->obstacles = newArray;
            game->obstacleCapacity = newCapacity;
        } else {
            return;  // Failed to allocate
        }
    }

    // Create new obstacle
    Obstacle* obs = &game->obstacles[game->obstacleCount];
    obs->lane = rand() % LANE_COUNT;
    obs->z = MAX_DEPTH;
    obs->isLow = (rand() % 3 == 0);  // 33% chance of low obstacle
    obs->height = obs->isLow ? 40.0f : 80.0f;
    obs->active = true;

    game->obstacleCount++;
}

void ObstaclesCleanup(Game* game) {
    if (game->obstacles) {
        free(game->obstacles);
        game->obstacles = NULL;
    }
    game->obstacleCount = 0;
    game->obstacleCapacity = 0;
}

bool ObstacleCheckCollision(Game* game) {
    Player* player = &game->player;

    for (int i = 0; i < game->obstacleCount; i++) {
        Obstacle* obs = &game->obstacles[i];

        if (!obs->active) continue;

        // Check if obstacle is at player's depth
        if (obs->z > 50.0f || obs->z < -20.0f) continue;

        // Check if same lane
        if (obs->lane != player->lane) continue;

        // Check if player is jumping over low obstacle
        if (obs->isLow && player->y < player->baseY - 60.0f) {
            continue;  // Player jumped over
        }

        // Collision detected
        return true;
    }

    return false;
}

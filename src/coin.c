#include "game.h"
#include <stdlib.h>
#include <string.h>

void CoinsInit(Game* game) {
    game->coinCapacity = INITIAL_COINS;
    game->coinCount = 0;
    game->coins = (Coin*)malloc(sizeof(Coin) * game->coinCapacity);
    if (game->coins) {
        memset(game->coins, 0, sizeof(Coin) * game->coinCapacity);
    }
}

void CoinsUpdate(Game* game, float deltaTime) {
    for (int i = 0; i < game->coinCount; i++) {
        if (game->coins[i].active && !game->coins[i].collected) {
            // Move coin toward player
            game->coins[i].z -= game->speed * deltaTime;

            // Deactivate if passed player
            if (game->coins[i].z < -100.0f) {
                game->coins[i].active = false;
            }
        }
    }

    // Compact array - remove inactive coins
    int writeIndex = 0;
    for (int i = 0; i < game->coinCount; i++) {
        if (game->coins[i].active && !game->coins[i].collected) {
            if (writeIndex != i) {
                game->coins[writeIndex] = game->coins[i];
            }
            writeIndex++;
        }
    }
    game->coinCount = writeIndex;
}

void CoinSpawn(Game* game) {
    // Expand array if needed
    if (game->coinCount >= game->coinCapacity) {
        int newCapacity = game->coinCapacity * 2;
        Coin* newArray = (Coin*)realloc(game->coins, sizeof(Coin) * newCapacity);
        if (newArray) {
            game->coins = newArray;
            game->coinCapacity = newCapacity;
        } else {
            return;  // Failed to allocate
        }
    }

    // Create new coin
    Coin* coin = &game->coins[game->coinCount];
    coin->lane = rand() % LANE_COUNT;
    coin->z = MAX_DEPTH;
    coin->collected = false;
    coin->active = true;

    game->coinCount++;
}

void CoinsCleanup(Game* game) {
    if (game->coins) {
        free(game->coins);
        game->coins = NULL;
    }
    game->coinCount = 0;
    game->coinCapacity = 0;
}

void CoinCheckCollection(Game* game) {
    Player* player = &game->player;

    for (int i = 0; i < game->coinCount; i++) {
        Coin* coin = &game->coins[i];

        if (!coin->active || coin->collected) continue;

        // Check if coin is at player's depth (with some tolerance)
        if (coin->z > 80.0f || coin->z < -30.0f) continue;

        // Check if same lane
        if (coin->lane != player->lane) continue;

        // Coin collected!
        coin->collected = true;
        coin->active = false;
        game->score += 10;
    }
}

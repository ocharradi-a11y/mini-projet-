#include "game.h"
#include <math.h>

// Gravity and jump constants
#define GRAVITY 1500.0f
#define JUMP_FORCE -600.0f
#define MOVE_SPEED 800.0f

void PlayerInit(Player* player) {
    player->lane = 1;  // Start in center lane
    player->x = PlayerGetLaneX(1);
    player->targetX = player->x;
    player->baseY = PLAYER_Y;
    player->y = player->baseY;
    player->isJumping = false;
    player->jumpVelocity = 0.0f;
    player->width = 50.0f;
    player->height = 80.0f;
}

void PlayerUpdate(Player* player, float deltaTime) {
    // Smooth lane transition
    if (player->x != player->targetX) {
        float diff = player->targetX - player->x;
        float move = MOVE_SPEED * deltaTime;

        if (fabsf(diff) < move) {
            player->x = player->targetX;
        } else if (diff > 0) {
            player->x += move;
        } else {
            player->x -= move;
        }
    }

    // Jump physics
    if (player->isJumping) {
        player->jumpVelocity += GRAVITY * deltaTime;
        player->y += player->jumpVelocity * deltaTime;

        // Land on ground
        if (player->y >= player->baseY) {
            player->y = player->baseY;
            player->isJumping = false;
            player->jumpVelocity = 0.0f;
        }
    }
}

void PlayerMoveLeft(Player* player) {
    if (player->lane > 0) {
        player->lane--;
        player->targetX = PlayerGetLaneX(player->lane);
    }
}

void PlayerMoveRight(Player* player) {
    if (player->lane < LANE_COUNT - 1) {
        player->lane++;
        player->targetX = PlayerGetLaneX(player->lane);
    }
}

void PlayerJump(Player* player) {
    if (!player->isJumping) {
        player->isJumping = true;
        player->jumpVelocity = JUMP_FORCE;
    }
}

float PlayerGetLaneX(int lane) {
    switch (lane) {
        case 0: return LANE_LEFT;
        case 1: return LANE_CENTER;
        case 2: return LANE_RIGHT;
        default: return LANE_CENTER;
    }
}

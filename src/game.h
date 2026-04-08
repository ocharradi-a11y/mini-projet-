#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdbool.h>

// Screen dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Game constants
#define LANE_COUNT 3
#define INITIAL_SPEED 300.0f
#define MAX_SPEED 800.0f
#define SPEED_INCREMENT 10.0f
#define INITIAL_OBSTACLES 10
#define INITIAL_COINS 10

// Lane positions (X coordinates)
#define LANE_WIDTH 120
#define LANE_LEFT   (SCREEN_WIDTH / 2 - LANE_WIDTH)
#define LANE_CENTER (SCREEN_WIDTH / 2)
#define LANE_RIGHT  (SCREEN_WIDTH / 2 + LANE_WIDTH)

// Depth constants for pseudo-3D
#define HORIZON_Y 200.0f
#define PLAYER_Y 500.0f
#define MAX_DEPTH 1000.0f

// Game states
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

// Forward declarations
typedef struct Player Player;
typedef struct Obstacle Obstacle;
typedef struct Coin Coin;

// Player structure
struct Player {
    int lane;               // 0=left, 1=center, 2=right
    float x;                // Current X position
    float targetX;          // Target X for smooth movement
    float y;                // Y position (for jumping)
    float baseY;            // Ground Y position
    bool isJumping;
    float jumpVelocity;
    float width;
    float height;
};

// Obstacle structure
struct Obstacle {
    int lane;
    float z;                // Depth (distance from player)
    float height;           // Obstacle height
    bool active;
    bool isLow;             // Can be jumped over
};

// Coin structure
struct Coin {
    int lane;
    float z;                // Depth
    bool collected;
    bool active;
};

// Menu options
typedef enum {
    MENU_PLAY,
    MENU_LEVEL,
    MENU_HOW_TO_PLAY,
    MENU_COUNT
} MenuOption;

// Main game structure
typedef struct {
    GameState state;
    Player player;

    // Menu state
    MenuOption menuSelection;
    bool showHowToPlay;

    // Dynamic arrays for obstacles and coins
    Obstacle* obstacles;
    int obstacleCount;
    int obstacleCapacity;

    Coin* coins;
    int coinCount;
    int coinCapacity;

    // Scoring
    int score;
    int highScore;
    float distance;

    // Game parameters
    float speed;
    float obstacleSpawnTimer;
    float coinSpawnTimer;
    float obstacleSpawnInterval;
    float coinSpawnInterval;
} Game;

// Game functions
void GameInit(Game* game);
void GameUpdate(Game* game, float deltaTime);
void GameCleanup(Game* game);
void GameReset(Game* game);

// Player functions
void PlayerInit(Player* player);
void PlayerUpdate(Player* player, float deltaTime);
void PlayerMoveLeft(Player* player);
void PlayerMoveRight(Player* player);
void PlayerJump(Player* player);
float PlayerGetLaneX(int lane);

// Obstacle functions
void ObstaclesInit(Game* game);
void ObstaclesUpdate(Game* game, float deltaTime);
void ObstacleSpawn(Game* game);
void ObstaclesCleanup(Game* game);
bool ObstacleCheckCollision(Game* game);

// Coin functions
void CoinsInit(Game* game);
void CoinsUpdate(Game* game, float deltaTime);
void CoinSpawn(Game* game);
void CoinsCleanup(Game* game);
void CoinCheckCollection(Game* game);

// Renderer functions
void RenderGame(Game* game);
void RenderMenu(Game* game);
void RenderPlaying(Game* game);
void RenderGameOver(Game* game);
void RenderRoad(void);
void RenderPlayer(Player* player);
void RenderObstacles(Game* game);
void RenderCoins(Game* game);
void RenderHUD(Game* game);

#endif // GAME_H

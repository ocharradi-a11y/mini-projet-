#include "game.h"
#include <stdio.h>
#include <math.h>

// Neon Runner Color Palette
#define BG_TOP        (Color){ 5, 5, 15, 255 }       // Deep dark blue-black
#define BG_BOTTOM     (Color){ 15, 10, 30, 255 }     // Slightly purple dark
#define ROAD_COLOR    (Color){ 10, 10, 20, 255 }     // Near black road
#define NEON_CYAN     (Color){ 0, 255, 255, 255 }    // Electric cyan
#define NEON_CYAN_DIM (Color){ 0, 150, 180, 255 }    // Dimmer cyan
#define NEON_BLUE     (Color){ 50, 100, 255, 255 }   // Electric blue
#define NEON_RED      (Color){ 255, 50, 80, 255 }    // Laser red
#define NEON_PINK     (Color){ 255, 50, 150, 255 }   // Neon pink
#define NEON_PURPLE   (Color){ 180, 50, 255, 255 }   // Neon purple
#define PLAYER_COLOR  (Color){ 0, 200, 255, 255 }    // Cyan player
#define PLAYER_GLOW   (Color){ 0, 150, 255, 100 }    // Player glow
#define OBSTACLE_COLOR (Color){ 255, 50, 80, 255 }   // Red laser
#define COIN_COLOR    (Color){ 0, 255, 255, 255 }    // Cyan energy
#define HUD_BG        (Color){ 0, 0, 0, 180 }        // HUD background
#define HUD_BORDER    (Color){ 0, 200, 255, 255 }    // HUD border glow

// Helper function to calculate perspective scale
static float GetPerspectiveScale(float z) {
    if (z <= 0) return 1.0f;
    float scale = 1.0f - (z / MAX_DEPTH) * 0.8f;
    if (scale < 0.1f) scale = 0.1f;
    return scale;
}

// Helper function to get Y position based on depth
static float GetPerspectiveY(float z) {
    float t = z / MAX_DEPTH;
    return HORIZON_Y + (PLAYER_Y - HORIZON_Y) * (1.0f - t);
}

// Helper function to get X position based on lane and depth
static float GetPerspectiveX(int lane, float z) {
    float scale = GetPerspectiveScale(z);
    float centerX = SCREEN_WIDTH / 2.0f;
    float laneOffset = (lane - 1) * LANE_WIDTH * scale;
    return centerX + laneOffset;
}

void RenderGame(Game* game) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (game->state) {
        case STATE_MENU:
            RenderMenu(game);
            break;
        case STATE_PLAYING:
            RenderPlaying(game);
            break;
        case STATE_GAME_OVER:
            RenderGameOver(game);
            break;
    }

    EndDrawing();
}

void RenderMenu(Game* game) {
    // Draw dark gradient background
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float t = (float)y / SCREEN_HEIGHT;
        Color color = {
            (unsigned char)(BG_TOP.r + (BG_BOTTOM.r - BG_TOP.r) * t),
            (unsigned char)(BG_TOP.g + (BG_BOTTOM.g - BG_TOP.g) * t),
            (unsigned char)(BG_TOP.b + (BG_BOTTOM.b - BG_TOP.b) * t),
            255
        };
        DrawLine(0, y, SCREEN_WIDTH, y, color);
    }

    // Draw perspective road preview
    float roadTopWidth = 40.0f;
    float roadBottomWidth = 300.0f;
    int roadY = 280;

    // Road surface
    Vector2 roadPoints[4] = {
        { SCREEN_WIDTH / 2.0f - roadTopWidth / 2, roadY },
        { SCREEN_WIDTH / 2.0f + roadTopWidth / 2, roadY },
        { SCREEN_WIDTH / 2.0f + roadBottomWidth / 2, SCREEN_HEIGHT },
        { SCREEN_WIDTH / 2.0f - roadBottomWidth / 2, SCREEN_HEIGHT }
    };
    DrawTriangle(roadPoints[0], roadPoints[2], roadPoints[1], (Color){10, 10, 20, 255});
    DrawTriangle(roadPoints[0], roadPoints[3], roadPoints[2], (Color){10, 10, 20, 255});

    // Neon lane lines
    for (int g = 2; g >= 0; g--) {
        Color gc = { 0, 255, 255, (unsigned char)(40 - g * 12) };
        DrawLineEx((Vector2){SCREEN_WIDTH / 2.0f - roadTopWidth / 2, roadY},
            (Vector2){SCREEN_WIDTH / 2.0f - roadBottomWidth / 2, SCREEN_HEIGHT}, 2.0f + g * 2, gc);
        DrawLineEx((Vector2){SCREEN_WIDTH / 2.0f + roadTopWidth / 2, roadY},
            (Vector2){SCREEN_WIDTH / 2.0f + roadBottomWidth / 2, SCREEN_HEIGHT}, 2.0f + g * 2, gc);
    }
    DrawLineEx((Vector2){SCREEN_WIDTH / 2.0f - roadTopWidth / 2, roadY},
        (Vector2){SCREEN_WIDTH / 2.0f - roadBottomWidth / 2, SCREEN_HEIGHT}, 2.0f, NEON_CYAN);
    DrawLineEx((Vector2){SCREEN_WIDTH / 2.0f + roadTopWidth / 2, roadY},
        (Vector2){SCREEN_WIDTH / 2.0f + roadBottomWidth / 2, SCREEN_HEIGHT}, 2.0f, NEON_CYAN);

    // Title with glow effect - "NEON SURF"
    const char* title = "NEON SURF";
    int titleWidth = MeasureText(title, 70);
    int titleX = (SCREEN_WIDTH - titleWidth) / 2;
    int titleY = 60;

    // Multiple glow layers
    for (int g = 6; g >= 0; g--) {
        Color glowColor = { 0, 255, 255, (unsigned char)(25 - g * 3) };
        DrawText(title, titleX - g, titleY - g, 70, glowColor);
        DrawText(title, titleX + g, titleY - g, 70, glowColor);
        DrawText(title, titleX - g, titleY + g, 70, glowColor);
        DrawText(title, titleX + g, titleY + g, 70, glowColor);
    }
    // Core text
    DrawText(title, titleX, titleY, 70, WHITE);

    // Subtitle
    const char* subtitle = "ENDLESS RUNNER";
    int subtitleWidth = MeasureText(subtitle, 20);
    DrawText(subtitle, (SCREEN_WIDTH - subtitleWidth) / 2, 140, 20, NEON_CYAN_DIM);

    // Menu buttons
    const char* menuLabels[3] = { "COMMENCER A JOUER", "NIVEAU", "COMMENT JOUER" };
    int btnWidth = 280;
    int btnHeight = 45;
    int btnStartY = 180;
    int btnSpacing = 60;

    for (int i = 0; i < 3; i++) {
        int btnX = (SCREEN_WIDTH - btnWidth) / 2;
        int btnY = btnStartY + i * btnSpacing;
        bool isSelected = (game->menuSelection == (MenuOption)i);

        // Selection indicator and glow
        Color btnGlowColor = isSelected ? NEON_CYAN : (Color){ 0, 150, 180, 100 };
        Color btnBorderColor = isSelected ? NEON_CYAN : (Color){ 0, 150, 180, 180 };
        Color btnTextColor = isSelected ? WHITE : (Color){ 150, 200, 220, 255 };

        // Button glow (stronger when selected)
        int glowLayers = isSelected ? 6 : 3;
        for (int g = glowLayers; g >= 0; g--) {
            int alpha = isSelected ? (50 - g * 7) : (25 - g * 7);
            if (alpha < 0) alpha = 0;
            DrawRectangleLines(btnX - g, btnY - g, btnWidth + g * 2, btnHeight + g * 2,
                (Color){ btnGlowColor.r, btnGlowColor.g, btnGlowColor.b, (unsigned char)alpha });
        }

        // Button background
        Color bgColor = isSelected ? (Color){ 0, 30, 40, 220 } : (Color){ 0, 0, 0, 180 };
        DrawRectangle(btnX, btnY, btnWidth, btnHeight, bgColor);
        DrawRectangleLines(btnX, btnY, btnWidth, btnHeight, btnBorderColor);

        // Selection arrow indicator
        if (isSelected) {
            DrawText(">", btnX - 25, btnY + 12, 24, NEON_CYAN);
            DrawText("<", btnX + btnWidth + 8, btnY + 12, 24, NEON_CYAN);
        }

        // Button text
        int textWidth = MeasureText(menuLabels[i], 22);
        int textX = (SCREEN_WIDTH - textWidth) / 2;
        int textY = btnY + 12;

        // Text glow when selected
        if (isSelected) {
            DrawText(menuLabels[i], textX - 1, textY - 1, 22, (Color){ 0, 255, 255, 60 });
            DrawText(menuLabels[i], textX + 1, textY + 1, 22, (Color){ 0, 255, 255, 60 });
        }
        DrawText(menuLabels[i], textX, textY, 22, btnTextColor);
    }

    // Navigation hint
    const char* navHint = "W/S ou FLECHES pour naviguer - ENTREE pour selectionner";
    int navWidth = MeasureText(navHint, 14);
    DrawText(navHint, (SCREEN_WIDTH - navWidth) / 2, 365, 14, (Color){ 100, 150, 180, 180 });

    // Controls panel (smaller, at bottom)
    int ctrlY = 400;
    DrawRectangle(220, ctrlY - 5, 360, 110, (Color){ 0, 0, 0, 150 });
    DrawRectangleLines(220, ctrlY - 5, 360, 110, (Color){ 0, 150, 200, 100 });

    DrawText("CONTROLES", 355, ctrlY + 5, 16, NEON_CYAN_DIM);
    DrawText("A / GAUCHE  - Aller a gauche", 240, ctrlY + 30, 14, (Color){ 150, 200, 255, 200 });
    DrawText("D / DROITE  - Aller a droite", 240, ctrlY + 50, 14, (Color){ 150, 200, 255, 200 });
    DrawText("ESPACE / W  - Sauter", 240, ctrlY + 70, 14, (Color){ 150, 200, 255, 200 });

    // High score with glow
    if (game->highScore > 0) {
        char hsText[32];
        sprintf(hsText, "MEILLEUR SCORE: %d", game->highScore);
        int hsWidth = MeasureText(hsText, 22);
        DrawText(hsText, (SCREEN_WIDTH - hsWidth) / 2 - 1, 529, 22, (Color){ 255, 200, 0, 80 });
        DrawText(hsText, (SCREEN_WIDTH - hsWidth) / 2, 530, 22, (Color){ 255, 200, 100, 255 });
    }

    // How to Play overlay
    if (game->showHowToPlay) {
        // Darken background
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 10, 220 });

        // Panel
        int panelW = 500;
        int panelH = 350;
        int panelX = (SCREEN_WIDTH - panelW) / 2;
        int panelY = (SCREEN_HEIGHT - panelH) / 2;

        // Panel glow
        for (int g = 4; g >= 0; g--) {
            DrawRectangleLines(panelX - g, panelY - g, panelW + g * 2, panelH + g * 2,
                (Color){ 0, 255, 255, (unsigned char)(40 - g * 8) });
        }
        DrawRectangle(panelX, panelY, panelW, panelH, (Color){ 5, 10, 20, 250 });
        DrawRectangleLines(panelX, panelY, panelW, panelH, NEON_CYAN);

        // Title
        const char* htpTitle = "COMMENT JOUER";
        int htpTitleW = MeasureText(htpTitle, 32);
        DrawText(htpTitle, (SCREEN_WIDTH - htpTitleW) / 2, panelY + 20, 32, NEON_CYAN);

        // Instructions
        int instrY = panelY + 70;
        DrawText("OBJECTIF:", panelX + 30, instrY, 20, NEON_PINK);
        DrawText("Courez le plus loin possible en evitant", panelX + 30, instrY + 30, 16, WHITE);
        DrawText("les obstacles et collectez les orbes d'energie!", panelX + 30, instrY + 50, 16, WHITE);

        DrawText("CONTROLES:", panelX + 30, instrY + 90, 20, NEON_PINK);
        DrawText("A / FLECHE GAUCHE  - Se deplacer a gauche", panelX + 30, instrY + 120, 16, (Color){ 180, 220, 255, 255 });
        DrawText("D / FLECHE DROITE  - Se deplacer a droite", panelX + 30, instrY + 145, 16, (Color){ 180, 220, 255, 255 });
        DrawText("ESPACE / W / HAUT  - Sauter", panelX + 30, instrY + 170, 16, (Color){ 180, 220, 255, 255 });
        DrawText("ESC                - Menu / Quitter", panelX + 30, instrY + 195, 16, (Color){ 180, 220, 255, 255 });

        // Close hint
        const char* closeHint = "Appuyez sur ENTREE ou ECHAP pour fermer";
        int closeW = MeasureText(closeHint, 16);
        DrawText(closeHint, (SCREEN_WIDTH - closeW) / 2, panelY + panelH - 35, 16, (Color){ 100, 200, 150, 200 });
    }
}

void RenderPlaying(Game* game) {
    // Draw dark gradient sky
    for (int y = 0; y < (int)HORIZON_Y; y++) {
        float t = (float)y / HORIZON_Y;
        Color color = {
            (unsigned char)(BG_TOP.r + (BG_BOTTOM.r - BG_TOP.r) * t),
            (unsigned char)(BG_TOP.g + (BG_BOTTOM.g - BG_TOP.g) * t),
            (unsigned char)(BG_TOP.b + (BG_BOTTOM.b - BG_TOP.b) * t),
            255
        };
        DrawLine(0, y, SCREEN_WIDTH, y, color);
    }

    // Render road
    RenderRoad();

    // Render coins (behind obstacles)
    RenderCoins(game);

    // Render obstacles
    RenderObstacles(game);

    // Render player
    RenderPlayer(&game->player);

    // Render HUD
    RenderHUD(game);
}

void RenderGameOver(Game* game) {
    // Draw dimmed background
    RenderPlaying(game);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 10, 200 });

    // Game Over text with red glow
    const char* gameOver = "GAME OVER";
    int goWidth = MeasureText(gameOver, 60);
    int goX = (SCREEN_WIDTH - goWidth) / 2;

    // Red glow layers
    for (int g = 5; g >= 0; g--) {
        Color glowColor = { 255, 50, 80, (unsigned char)(25 - g * 4) };
        DrawText(gameOver, goX - g, 180 - g, 60, glowColor);
        DrawText(gameOver, goX + g, 180 - g, 60, glowColor);
        DrawText(gameOver, goX - g, 180 + g, 60, glowColor);
        DrawText(gameOver, goX + g, 180 + g, 60, glowColor);
    }
    DrawText(gameOver, goX, 180, 60, NEON_RED);

    // Score panel
    int panelW = 280;
    int panelH = 100;
    int panelX = (SCREEN_WIDTH - panelW) / 2;
    int panelY = 260;

    for (int g = 3; g >= 0; g--) {
        DrawRectangleLines(panelX - g, panelY - g, panelW + g * 2, panelH + g * 2,
            (Color){ 0, 255, 255, (unsigned char)(30 - g * 7) });
    }
    DrawRectangle(panelX, panelY, panelW, panelH, (Color){ 0, 0, 0, 200 });
    DrawRectangleLines(panelX, panelY, panelW, panelH, NEON_CYAN);

    // Score
    char scoreText[32];
    sprintf(scoreText, "SCORE: %d", game->score);
    int scoreWidth = MeasureText(scoreText, 36);
    DrawText(scoreText, (SCREEN_WIDTH - scoreWidth) / 2, panelY + 15, 36, NEON_CYAN);

    // High score
    char hsText[32];
    sprintf(hsText, "HIGH SCORE: %d", game->highScore);
    int hsWidth = MeasureText(hsText, 24);
    DrawText(hsText, (SCREEN_WIDTH - hsWidth) / 2, panelY + 60, 24, (Color){ 255, 200, 100, 255 });

    // Restart button
    int btnW = 260;
    int btnH = 45;
    int btnX = (SCREEN_WIDTH - btnW) / 2;
    int btnY = 400;

    for (int g = 3; g >= 0; g--) {
        DrawRectangleLines(btnX - g, btnY - g, btnW + g * 2, btnH + g * 2,
            (Color){ 0, 255, 100, (unsigned char)(30 - g * 7) });
    }
    DrawRectangle(btnX, btnY, btnW, btnH, (Color){ 0, 0, 0, 180 });
    DrawRectangleLines(btnX, btnY, btnW, btnH, (Color){ 0, 255, 100, 255 });

    const char* restart = "PRESS ENTER TO RESTART";
    int restartWidth = MeasureText(restart, 18);
    DrawText(restart, (SCREEN_WIDTH - restartWidth) / 2, btnY + 14, 18, (Color){ 0, 255, 100, 255 });

    // Menu hint
    const char* menu = "ESC for Menu";
    int menuWidth = MeasureText(menu, 18);
    DrawText(menu, (SCREEN_WIDTH - menuWidth) / 2, 470, 18, (Color){ 100, 150, 200, 200 });
}

void RenderRoad(void) {
    // Draw dark city background on sides
    DrawRectangle(0, (int)HORIZON_Y, SCREEN_WIDTH, SCREEN_HEIGHT - (int)HORIZON_Y, (Color){5, 5, 10, 255});

    // Draw road with perspective (trapezoid)
    float roadTopWidth = 60.0f;
    float roadBottomWidth = 400.0f;

    Vector2 roadPoints[4] = {
        { SCREEN_WIDTH / 2.0f - roadTopWidth / 2, HORIZON_Y },
        { SCREEN_WIDTH / 2.0f + roadTopWidth / 2, HORIZON_Y },
        { SCREEN_WIDTH / 2.0f + roadBottomWidth / 2, SCREEN_HEIGHT },
        { SCREEN_WIDTH / 2.0f - roadBottomWidth / 2, SCREEN_HEIGHT }
    };

    DrawTriangle(roadPoints[0], roadPoints[2], roadPoints[1], ROAD_COLOR);
    DrawTriangle(roadPoints[0], roadPoints[3], roadPoints[2], ROAD_COLOR);

    // Draw neon edge lines (outer glow)
    for (int glow = 3; glow >= 0; glow--) {
        Color glowColor = (Color){ 0, 255, 255, (unsigned char)(60 - glow * 15) };
        float glowOffset = glow * 2.0f;

        // Left edge line
        DrawLineEx(
            (Vector2){ SCREEN_WIDTH / 2.0f - roadTopWidth / 2 - glowOffset, HORIZON_Y },
            (Vector2){ SCREEN_WIDTH / 2.0f - roadBottomWidth / 2 - glowOffset, SCREEN_HEIGHT },
            2.0f + glow,
            glowColor
        );
        // Right edge line
        DrawLineEx(
            (Vector2){ SCREEN_WIDTH / 2.0f + roadTopWidth / 2 + glowOffset, HORIZON_Y },
            (Vector2){ SCREEN_WIDTH / 2.0f + roadBottomWidth / 2 + glowOffset, SCREEN_HEIGHT },
            2.0f + glow,
            glowColor
        );
    }
    // Core bright edge lines
    DrawLineEx(
        (Vector2){ SCREEN_WIDTH / 2.0f - roadTopWidth / 2, HORIZON_Y },
        (Vector2){ SCREEN_WIDTH / 2.0f - roadBottomWidth / 2, SCREEN_HEIGHT },
        2.0f, NEON_CYAN
    );
    DrawLineEx(
        (Vector2){ SCREEN_WIDTH / 2.0f + roadTopWidth / 2, HORIZON_Y },
        (Vector2){ SCREEN_WIDTH / 2.0f + roadBottomWidth / 2, SCREEN_HEIGHT },
        2.0f, NEON_CYAN
    );

    // Draw lane divider lines (neon cyan continuous lines)
    for (int i = 0; i < 2; i++) {
        float laneOffset = (i == 0) ? -LANE_WIDTH / 3.0f : LANE_WIDTH / 3.0f;

        // Draw glow layers
        for (int glow = 2; glow >= 0; glow--) {
            Color glowColor = (Color){ 0, 255, 255, (unsigned char)(40 - glow * 12) };

            for (float z = 0; z < MAX_DEPTH; z += 5.0f) {
                float y1 = GetPerspectiveY(z);
                float y2 = GetPerspectiveY(z + 5.0f);
                float scale1 = GetPerspectiveScale(z);
                float scale2 = GetPerspectiveScale(z + 5.0f);

                float x1 = SCREEN_WIDTH / 2.0f + laneOffset * scale1 * 3.0f;
                float x2 = SCREEN_WIDTH / 2.0f + laneOffset * scale2 * 3.0f;

                DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, (2.0f + glow * 2) * scale1, glowColor);
            }
        }

        // Core bright line
        for (float z = 0; z < MAX_DEPTH; z += 5.0f) {
            float y1 = GetPerspectiveY(z);
            float y2 = GetPerspectiveY(z + 5.0f);
            float scale1 = GetPerspectiveScale(z);
            float scale2 = GetPerspectiveScale(z + 5.0f);

            float x1 = SCREEN_WIDTH / 2.0f + laneOffset * scale1 * 3.0f;
            float x2 = SCREEN_WIDTH / 2.0f + laneOffset * scale2 * 3.0f;

            DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 2.0f * scale1, NEON_CYAN);
        }
    }

    // Draw some background city buildings (silhouettes with lit windows)
    for (int i = 0; i < 8; i++) {
        int buildingX = 50 + i * 100;
        int buildingHeight = 60 + (i % 3) * 40;
        int buildingWidth = 40 + (i % 2) * 20;

        // Left side buildings
        if (buildingX < SCREEN_WIDTH / 2 - 220) {
            DrawRectangle(buildingX, (int)HORIZON_Y - buildingHeight, buildingWidth, buildingHeight, (Color){8, 8, 15, 255});
            // Random lit windows
            for (int w = 0; w < 3; w++) {
                for (int h = 0; h < buildingHeight / 15; h++) {
                    if ((i + w + h) % 3 == 0) {
                        DrawRectangle(buildingX + 5 + w * 12, (int)HORIZON_Y - buildingHeight + 5 + h * 15, 8, 10,
                            (Color){ 100 + (w * 30), 150 + (h * 20), 255, 150 });
                    }
                }
            }
        }
        // Right side buildings
        int rightX = SCREEN_WIDTH - buildingX - buildingWidth;
        if (rightX > SCREEN_WIDTH / 2 + 220) {
            DrawRectangle(rightX, (int)HORIZON_Y - buildingHeight, buildingWidth, buildingHeight, (Color){8, 8, 15, 255});
            for (int w = 0; w < 3; w++) {
                for (int h = 0; h < buildingHeight / 15; h++) {
                    if ((i + w + h) % 3 == 0) {
                        DrawRectangle(rightX + 5 + w * 12, (int)HORIZON_Y - buildingHeight + 5 + h * 15, 8, 10,
                            (Color){ 100 + (w * 30), 150 + (h * 20), 255, 150 });
                    }
                }
            }
        }
    }
}

void RenderPlayer(Player* player) {
    float width = player->width;
    float height = player->height;
    int px = (int)(player->x - width / 2);
    int py = (int)(player->y - height);

    // Outer glow effect
    for (int g = 5; g >= 0; g--) {
        Color glowColor = { 0, 200, 255, (unsigned char)(30 - g * 5) };
        DrawRectangle(
            px - g * 2,
            py - g * 2,
            (int)width + g * 4,
            (int)height + g * 4,
            glowColor
        );
    }

    // Player body - dark with neon edges
    DrawRectangle(px, py, (int)width, (int)height, (Color){ 20, 30, 50, 255 });

    // Neon outline
    DrawRectangleLines(px, py, (int)width, (int)height, NEON_CYAN);

    // Inner glow lines (cyber style)
    DrawLine(px + 5, py + (int)(height * 0.2f), px + (int)width - 5, py + (int)(height * 0.2f),
        (Color){ 0, 255, 255, 150 });
    DrawLine(px + 5, py + (int)(height * 0.5f), px + (int)width - 5, py + (int)(height * 0.5f),
        (Color){ 0, 255, 255, 100 });
    DrawLine(px + 5, py + (int)(height * 0.8f), px + (int)width - 5, py + (int)(height * 0.8f),
        (Color){ 0, 255, 255, 80 });

    // Visor (futuristic helmet)
    int visorY = (int)(player->y - height * 0.75f);
    int visorHeight = (int)(height * 0.2f);

    // Visor glow
    for (int g = 2; g >= 0; g--) {
        DrawRectangle(px + 5 - g, visorY - g, (int)width - 10 + g * 2, visorHeight + g * 2,
            (Color){ 0, 255, 255, (unsigned char)(40 - g * 12) });
    }
    DrawRectangle(px + 5, visorY, (int)width - 10, visorHeight, (Color){ 0, 200, 255, 200 });

    // Trail effect when moving/jumping
    if (player->isJumping || player->x != player->targetX) {
        for (int t = 0; t < 3; t++) {
            int trailY = (int)(player->y + 5 + t * 8);
            float trailAlpha = 80 - t * 25;
            DrawRectangle(px + 10, trailY, (int)width - 20, 3,
                (Color){ 0, 255, 255, (unsigned char)trailAlpha });
        }
    }
}

void RenderObstacles(Game* game) {
    // Sort by depth (furthest first) - simple bubble sort for small arrays
    for (int i = 0; i < game->obstacleCount - 1; i++) {
        for (int j = 0; j < game->obstacleCount - i - 1; j++) {
            if (game->obstacles[j].z < game->obstacles[j + 1].z) {
                Obstacle temp = game->obstacles[j];
                game->obstacles[j] = game->obstacles[j + 1];
                game->obstacles[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < game->obstacleCount; i++) {
        Obstacle* obs = &game->obstacles[i];
        if (!obs->active || obs->z < 0 || obs->z > MAX_DEPTH) continue;

        float scale = GetPerspectiveScale(obs->z);
        float y = GetPerspectiveY(obs->z);
        float x = GetPerspectiveX(obs->lane, obs->z);

        float width = 60.0f * scale;
        float height = obs->height * scale;

        // Neon laser barrier style
        Color laserColor = obs->isLow ? NEON_PINK : NEON_RED;
        Color glowColor = obs->isLow ?
            (Color){ 255, 50, 150, 60 } : (Color){ 255, 50, 80, 60 };

        // Draw glow effect (multiple layers)
        for (int glow = 4; glow >= 0; glow--) {
            float glowSize = glow * 3.0f * scale;
            Color gc = glowColor;
            gc.a = (unsigned char)(40 - glow * 8);

            DrawRectangle(
                (int)(x - width / 2 - glowSize),
                (int)(y - height - glowSize),
                (int)(width + glowSize * 2),
                (int)(height + glowSize * 2),
                gc
            );
        }

        // Draw laser barrier poles (left and right)
        float poleWidth = 6.0f * scale;
        float poleHeight = height + 10.0f * scale;

        // Left pole
        DrawRectangle(
            (int)(x - width / 2 - poleWidth / 2),
            (int)(y - poleHeight),
            (int)poleWidth,
            (int)poleHeight,
            (Color){ 40, 40, 50, 255 }
        );
        // Right pole
        DrawRectangle(
            (int)(x + width / 2 - poleWidth / 2),
            (int)(y - poleHeight),
            (int)poleWidth,
            (int)poleHeight,
            (Color){ 40, 40, 50, 255 }
        );
        // Pole top lights
        DrawCircle((int)(x - width / 2), (int)(y - poleHeight), 4.0f * scale, laserColor);
        DrawCircle((int)(x + width / 2), (int)(y - poleHeight), 4.0f * scale, laserColor);

        // Draw horizontal laser beams
        int beamCount = obs->isLow ? 2 : 3;
        for (int b = 0; b < beamCount; b++) {
            float beamY = y - height + (height / (beamCount + 1)) * (b + 1);

            // Glow layers for beam
            for (int g = 3; g >= 0; g--) {
                Color bgc = glowColor;
                bgc.a = (unsigned char)(50 - g * 12);
                DrawLineEx(
                    (Vector2){ x - width / 2, beamY },
                    (Vector2){ x + width / 2, beamY },
                    (4.0f + g * 3.0f) * scale,
                    bgc
                );
            }
            // Core beam
            DrawLineEx(
                (Vector2){ x - width / 2, beamY },
                (Vector2){ x + width / 2, beamY },
                3.0f * scale,
                laserColor
            );
        }

        // Add energy particles around the barrier (flickering effect)
        for (int p = 0; p < 3; p++) {
            float px = x - width / 2 + (width / 3.0f) * p + (width / 6.0f);
            float py = y - height / 2 + ((p * 7) % 5 - 2) * scale * 3;
            DrawCircle((int)px, (int)py, 2.0f * scale, Fade(laserColor, 0.5f + (p % 2) * 0.3f));
        }
    }
}

void RenderCoins(Game* game) {
    for (int i = 0; i < game->coinCount; i++) {
        Coin* coin = &game->coins[i];
        if (!coin->active || coin->collected || coin->z < 0 || coin->z > MAX_DEPTH) continue;

        float scale = GetPerspectiveScale(coin->z);
        float y = GetPerspectiveY(coin->z);
        float x = GetPerspectiveX(coin->lane, coin->z);

        float radius = 12.0f * scale;
        float coinY = y - 30 * scale;

        // Draw outer glow (multiple layers for bloom effect)
        for (int glow = 5; glow >= 0; glow--) {
            float glowRadius = radius + glow * 4.0f * scale;
            Color glowColor = { 0, 255, 255, (unsigned char)(30 - glow * 5) };
            DrawCircle((int)x, (int)coinY, (int)glowRadius, glowColor);
        }

        // Inner energy sphere (gradient effect with rings)
        DrawCircle((int)x, (int)coinY, (int)radius, (Color){ 0, 200, 255, 200 });
        DrawCircle((int)x, (int)coinY, (int)(radius * 0.7f), (Color){ 100, 255, 255, 220 });
        DrawCircle((int)x, (int)coinY, (int)(radius * 0.4f), (Color){ 200, 255, 255, 255 });

        // Outer ring
        DrawCircleLines((int)x, (int)coinY, (int)radius, NEON_CYAN);
        DrawCircleLines((int)x, (int)coinY, (int)(radius * 1.2f), (Color){ 0, 255, 255, 100 });

        // Energy particles orbiting
        float time = GetTime() * 3.0f + i * 0.5f;
        for (int p = 0; p < 3; p++) {
            float angle = time + p * (3.14159f * 2.0f / 3.0f);
            float px = x + cosf(angle) * radius * 1.5f;
            float py = coinY + sinf(angle) * radius * 0.8f;
            DrawCircle((int)px, (int)py, 2.0f * scale, (Color){ 0, 255, 255, 200 });
        }

        // Data cube effect (small square in center)
        int cubeSize = (int)(radius * 0.3f);
        DrawRectangle((int)(x - cubeSize / 2), (int)(coinY - cubeSize / 2),
            cubeSize, cubeSize, (Color){ 255, 255, 255, 200 });
    }
}

void RenderHUD(Game* game) {
    // Score panel with neon border
    // Glow effect for border
    for (int g = 3; g >= 0; g--) {
        DrawRectangleLines(10 - g, 10 - g, 200 + g * 2, 90 + g * 2,
            (Color){ 0, 255, 255, (unsigned char)(40 - g * 10) });
    }
    DrawRectangle(10, 10, 200, 90, HUD_BG);
    DrawRectangleLines(10, 10, 200, 90, HUD_BORDER);

    // Score with glow effect
    char scoreText[32];
    sprintf(scoreText, "SCORE: %d", game->score);
    // Text glow
    DrawText(scoreText, 18, 18, 26, (Color){ 0, 255, 255, 60 });
    DrawText(scoreText, 20, 20, 26, NEON_CYAN);

    // Distance
    char distText[32];
    sprintf(distText, "DIST: %dm", (int)(game->distance / 10));
    DrawText(distText, 20, 55, 20, (Color){ 0, 200, 255, 200 });

    // Speed indicator panel
    // Glow effect
    for (int g = 2; g >= 0; g--) {
        DrawRectangleLines(SCREEN_WIDTH - 130 - g, 10 - g, 120 + g * 2, 40 + g * 2,
            (Color){ 0, 255, 100, (unsigned char)(30 - g * 10) });
    }
    DrawRectangle(SCREEN_WIDTH - 130, 10, 120, 40, HUD_BG);
    DrawRectangleLines(SCREEN_WIDTH - 130, 10, 120, 40, (Color){ 0, 255, 100, 255 });

    char speedText[32];
    sprintf(speedText, "SPD: %.0f", game->speed);
    DrawText(speedText, SCREEN_WIDTH - 120, 18, 22, (Color){ 0, 255, 100, 255 });

    // Energy bar (based on speed)
    float speedPercent = (game->speed - INITIAL_SPEED) / (MAX_SPEED - INITIAL_SPEED);
    if (speedPercent < 0) speedPercent = 0;
    if (speedPercent > 1) speedPercent = 1;

    int barWidth = 180;
    int barHeight = 8;
    int barX = 10;
    int barY = 105;

    DrawRectangle(barX, barY, barWidth, barHeight, (Color){ 20, 20, 30, 200 });
    DrawRectangle(barX, barY, (int)(barWidth * speedPercent), barHeight,
        (Color){ (unsigned char)(speedPercent * 255), (unsigned char)(255 - speedPercent * 155), 100, 255 });
    DrawRectangleLines(barX, barY, barWidth, barHeight, (Color){ 0, 200, 255, 150 });
}

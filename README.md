# Endless Runner

A Subway Surfers-style endless runner game developed in C using Raylib graphics library.

## Description

Endless Runner is a 2.5D (pseudo-3D) endless runner game where the player runs forward automatically on a 3-lane track. The player must avoid obstacles by switching lanes or jumping, while collecting coins to increase their score. The game features increasing difficulty as the player progresses.

## Features

- **3-Lane System**: Switch between left, center, and right lanes
- **Jump Mechanic**: Jump over low obstacles
- **Coin Collection**: Collect coins to increase your score
- **Dynamic Difficulty**: Speed increases as you progress
- **High Score Tracking**: Tracks your best score during the session
- **Menu System**: Start screen and game over screen with restart option

## Controls

| Key | Action |
|-----|--------|
| A / Left Arrow | Move to the left lane |
| D / Right Arrow | Move to the right lane |
| Space / W / Up Arrow | Jump |
| Enter | Start game / Restart |
| ESC | Return to menu |

## Requirements

- Windows operating system
- w64devkit (MinGW-w64 GCC compiler) - included in project
- Raylib 5.0 library (64-bit) - included in project

## Compilation

### Quick Start (Windows)

1. Open a terminal (Git Bash or Command Prompt with bash)
2. Navigate to the project directory
3. Set up the compiler environment:
   ```bash
   export PATH="$(pwd)/w64devkit/bin:$PATH"
   ```
4. Compile the project:
   ```bash
   make
   ```
5. Run the game:
   ```bash
   ./endless_runner.exe
   ```

### Manual Compilation

```bash
export PATH="$(pwd)/w64devkit/bin:$PATH"

gcc -Wall -Wextra -std=c99 -I./include -c src/main.c -o obj/main.o
gcc -Wall -Wextra -std=c99 -I./include -c src/game.c -o obj/game.o
gcc -Wall -Wextra -std=c99 -I./include -c src/player.c -o obj/player.o
gcc -Wall -Wextra -std=c99 -I./include -c src/obstacle.c -o obj/obstacle.o
gcc -Wall -Wextra -std=c99 -I./include -c src/coin.c -o obj/coin.o
gcc -Wall -Wextra -std=c99 -I./include -c src/renderer.c -o obj/renderer.o

gcc obj/*.o -o endless_runner.exe -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm
```

## Project Structure

```
mini-project/
├── include/              # Header files
│   ├── raylib.h         # Raylib graphics library
│   ├── raymath.h        # Raylib math utilities
│   └── rlgl.h           # Raylib OpenGL abstraction
├── lib/                  # Libraries
│   └── libraylib.a      # Raylib static library (64-bit)
├── src/                  # Source code
│   ├── main.c           # Entry point and game loop
│   ├── game.h           # Game structures and declarations
│   ├── game.c           # Game logic implementation
│   ├── player.c         # Player movement and physics
│   ├── obstacle.c       # Obstacle spawning and collision
│   ├── coin.c           # Coin collection system
│   └── renderer.c       # All rendering functions
├── obj/                  # Compiled object files
├── w64devkit/           # MinGW-w64 compiler toolkit
├── Makefile             # Build automation
├── README.md            # This file
└── endless_runner.exe   # Compiled game
```

## Technical Details

- **Language**: C (C99 standard)
- **Graphics**: Raylib 5.0
- **Compiler**: GCC 14.1.0 (w64devkit)
- **Resolution**: 800x600 pixels
- **Target FPS**: 60

### Memory Management

The project implements proper memory management:
- Dynamic arrays for obstacles and coins using `malloc`/`realloc`/`free`
- All allocated memory is freed in `GameCleanup()`
- No memory leaks (verified with address sanitizer)

## Gameplay Tips

1. Start in the center lane for maximum maneuverability
2. Jump over low obstacles (orange colored)
3. Avoid tall obstacles (red colored) by switching lanes
4. Collect coins for points - each coin is worth 10 points
5. The game speeds up over time - stay alert!

## Credits

- **Raylib**: https://www.raylib.com/ - Simple and easy-to-use game programming library
- **w64devkit**: https://github.com/skeeto/w64devkit - Portable C/C++ development kit for Windows

## License

This project is created for educational purposes as part of a university mini-project (TP).

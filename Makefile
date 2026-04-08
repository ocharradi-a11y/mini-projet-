# Endless Runner - Makefile
# IMPORTANT: Before running make, add w64devkit to PATH:
#   export PATH="$(pwd)/w64devkit/bin:$PATH"

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./include
LDFLAGS = -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/game.c \
          $(SRC_DIR)/player.c \
          $(SRC_DIR)/obstacle.c \
          $(SRC_DIR)/coin.c \
          $(SRC_DIR)/renderer.c

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Output executable
TARGET = endless_runner.exe

# Default target
all: $(OBJ_DIR) $(TARGET)

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Link executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Debug build with sanitizers
debug: CFLAGS += -g -fsanitize=address
debug: LDFLAGS += -fsanitize=address
debug: clean all

# Run the game
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean debug run

####################### Makefile for Chess Project #######################################

# Compiler settings
CC = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# SDL2 flags via pkg-config (preferred) or direct paths
SDL_FLAGS = $(shell pkg-config --cflags sdl2 SDL2_image SDL2_mixer 2>/dev/null || echo "-I/opt/homebrew/include/SDL2")
SDL_LIBS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_mixer 2>/dev/null || echo "-L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_mixer")

# Include paths
INCLUDES = -Isrc/headers $(SDL_FLAGS)

# macOS specific flags
MACOS_LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework Carbon

# Makefile settings
MAINAPP = chess
TESTAPP = chess_test
SRCDIR = src
OBJDIR = obj

# Source files
MAIN_SRC = main.cpp

# Object files
MAIN_OBJ = $(OBJDIR)/main.o

# Create object directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

# Compiler and linker commands
$(OBJDIR)/%.o: %.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Default target
all: $(MAINAPP)

# Main application target
$(MAINAPP): $(MAIN_OBJ)
	$(CC) $(CXXFLAGS) -o $@ $^ $(SDL_LIBS) $(MACOS_LIBS)

# Test application target
test: $(OBJDIR)/test.o
	$(CC) $(CXXFLAGS) -o $(TESTAPP) $^ $(SDL_LIBS) $(MACOS_LIBS)

# Cleaning rules
clean:
	rm -rf $(OBJDIR) $(MAINAPP) $(TESTAPP)

# Run target
run: $(MAINAPP)
	./$(MAINAPP)

# Phony targets
.PHONY: all clean test run help

# Help target
help:
	@echo "Chess Project Makefile Help"
	@echo "------------------------"
	@echo "Targets:"
	@echo "  all            : Build the chess application (default)"
	@echo "  test           : Build the test application"
	@echo "  clean          : Remove object files and executables"
	@echo "  run            : Build and run the chess application"
	@echo "  help           : Display this help message"
	@echo ""
	@echo "The Makefile is configured to use SDL2 libraries installed via Homebrew."
	@echo "If you haven't installed them yet, run 'brew install sdl2 sdl2_image sdl2_mixer'"
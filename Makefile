####################### Makefile Template #######################################

# Compiler settings
CC = g++
CXXFLAGS = -std=c++17 -Wall
INCLUDES = -Isrc/include/SDL2 -Isrc/headers
LDFLAGS = -Lsrc/lib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer

# Makefile settings
MAINAPP = chess
TESTAPP = chess_test
EXT = .cpp
SRCDIR = src
OBJDIR = obj

# Source files
MAIN_SRC = $(wildcard *.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
TEST_SRC = test.cpp $(filter-out main.cpp, $(wildcard $(SRCDIR)/**/*.cpp))

# Object files for main application
MAIN_OBJ = $(patsubst %$(EXT), $(OBJDIR)/%.o, $(notdir $(filter-out test.cpp, $(MAIN_SRC))))

# Object files for test
TEST_OBJ = $(patsubst %$(EXT), $(OBJDIR)/%.o, $(notdir $(filter-out main.cpp, $(TEST_SRC))))

# Dependency files
MAIN_DEP = $(MAIN_OBJ:.o=.d)
TEST_DEP = $(TEST_OBJ:.o=.d)

# Compiler and linker commands
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Default target
all: $(MAINAPP)

# Main application target
$(MAINAPP): $(MAIN_OBJ)
	$(CC) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LIBS)

# Test application target
test: $(TEST_OBJ)
	$(CC) -o $(TESTAPP) $^ $(LDFLAGS) $(LIBS)

# Include the dependency files
-include $(MAIN_DEP)
-include $(TEST_DEP)

# Cleaning rules
clean:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.d $(MAINAPP) $(TESTAPP)

# Phony targets for cleanliness
.PHONY: all clean test
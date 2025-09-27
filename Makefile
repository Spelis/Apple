TARGET   := apple
CXX      := g++
CC 		 := gcc
CXXFLAGS := -Wall -Wextra -O2 -Isrc -Iraylib/src
LDFLAGS  := -Lraylib/src -lraylib
TARGETOS := Unknown
UNAME_S := $(shell uname -s)

SRC_DIR  := src
SRC      := $(shell find $(SRC_DIR) -name '*.cpp' -o -name '*.c')
OBJ      := $(SRC:$(SRC_DIR)/%.cpp=build/cpp/%.o)
OBJ      := $(OBJ:$(SRC_DIR)/%.c=build/c/%.o)

ifeq ($(OS),Windows_NT)
    TARGETOS := Windows
    CXX := g++
    CC := gcc
    CXXFLAGS += -std=c++17
    LDFLAGS := -Lraylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -lkernel32
else
    ifeq ($(UNAME_S),Linux)
        TARGETOS := Linux
        CXX := g++
        CC := gcc
        ifdef MINGW
            TARGETOS := Mingw
            CXX := x86_64-w64-mingw32-g++
            CC := x86_64-w64-mingw32-gcc
            LDFLAGS := -Lraylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -lkernel32
        endif
    endif
    ifeq ($(UNAME_S),Darwin)
        TARGETOS := MacOS
        CXX := clang++
        CC := clang
        LDFLAGS := -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
    endif
endif

all: dirs build/$(TARGET)

dirs:
	@echo "OS: $(TARGETOS)"
	@echo "CURRENT CC: $(CXX) $(CC)"
	@echo "CXXFLAGS/LDFLAGS: $(CXXFLAGS) $(LDFLAGS)"
	@echo "TARGETING: $(SRC) -> $(OBJ)"
	@mkdir -p build build/c build/cpp
	@mkdir -p $(sort $(dir $(OBJ)))

build/$(TARGET): $(OBJ)
	@echo "LINK: $(OBJ) -> $(TARGET)"
	@$(CXX) $(OBJ) $(CXXFLAGS) $(LDFLAGS) -o $@

build/cpp/%.o: $(SRC_DIR)/%.cpp
	@echo "CXX: $< -> $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

build/c/%.o: $(SRC_DIR)/%.c
	@echo "CC: $< -> $@"
	@$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "DEL: build/*"
	@rm -rf build

raylib: raylib/
	@if [ -d raylib ]; then \
		echo "Raylib already exists"; \
	else \
		git clone https://github.com/raysan5/raylib; \
	fi

	$(MAKE) PLATFORM=PLATFORM_DESKTOP -C raylib/src

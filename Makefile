CXX = g++
CXXFLAGS = -Wall -std=c++17 -Isrc
LDFLAGS = -lsodium

SRC = $(shell find src -name "*.cpp")

OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

TARGET = gestor

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

build/%.o :src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET)
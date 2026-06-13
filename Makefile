# Thin convenience wrapper around the CMake build.
# Dependencies (uWebSockets, uSockets, jsoncpp) are fetched into ./third_party
# by CMake; OpenSSL and zlib are located from the system. No apt install needed.

BUILD_DIR ?= build

.PHONY: all build release run clean rebuild

all: build

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=RelWithDebInfo
	cmake --build $(BUILD_DIR) -j

release:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) -j

# Run from the repo root so apps.json (read by relative path) is found.
run: build
	./$(BUILD_DIR)/rofrof

clean:
	rm -rf $(BUILD_DIR)

# Remove build output and all fetched dependencies.
distclean:
	rm -rf $(BUILD_DIR) third_party

rebuild: clean build

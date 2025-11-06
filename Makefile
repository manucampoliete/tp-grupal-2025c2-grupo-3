.PHONY: all test clean editor client common server build

TARGET = taller_client
BUILD_DIR = build

debug:
	mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR) --target $(TARGET)

release:
	mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) --target $(TARGET)

run: debug
	./$(BUILD_DIR)/$(TARGET)

compile-debug:
	mkdir -p build/
	cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug $(EXTRA_GENERATE)
	cmake --build  build/ $(EXTRA_COMPILE)

compile-release:
	mkdir -p build/
	cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release $(EXTRA_GENERATE)
	cmake --build  build/ $(EXTRA_COMPILE)

run-tests: compile-debug
	./build/taller_tests

clean:
	rm -rf $(BUILD_DIR)

all: clean debug

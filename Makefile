.PHONY: all test clean editor client common server build

BUILD_DIR = build

compile-debug:
	mkdir -p $(BUILD_DIR)/
	cmake -S . -B ./$(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug $(EXTRA_GENERATE)
	cmake --build  $(BUILD_DIR)/ $(EXTRA_COMPILE)

compile-release:
	mkdir -p $(BUILD_DIR)/
	cmake -S . -B ./$(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release $(EXTRA_GENERATE)
	cmake --build  $(BUILD_DIR)/ $(EXTRA_COMPILE)

run-tests: compile-debug
	./$(BUILD_DIR)/taller_tests

all: clean run-tests

clean:
	rm -rf $(BUILD_DIR)/

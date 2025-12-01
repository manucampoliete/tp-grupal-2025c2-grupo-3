.PHONY: test clean editor client common server build

BUILD_DIR = build

compile-debug:
	mkdir -p $(BUILD_DIR)/
	cmake -S . -B ./$(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug $(EXTRA_GENERATE)
	cmake --build  $(BUILD_DIR)/ $(EXTRA_COMPILE)

run-tests: compile-debug
	./$(BUILD_DIR)/needForSpeed2DTests

clean:
	rm -rf $(BUILD_DIR)/

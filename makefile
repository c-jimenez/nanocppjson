######################################################
#                  Helper makefile                   #
######################################################

# Default target
default: all

# Root directory containing the top level CMakeLists.txt file
ROOT_DIR:=$(PWD)

# Build directory
BUILD_DIR:=$(ROOT_DIR)/build

# Generated binary directory
BIN_DIR:=$(ROOT_DIR)/bin

# Code coverage output directory
COVERAGE_DIR:=$(ROOT_DIR)/coverage

# Make options
#VERBOSE="VERBOSE=1"
PARALLEL_BUILD?=-j 6

# Build type can be either Debug or Release
#BUILD_TYPE?=Release
BUILD_TYPE?=Debug

# Silent makefile
.SILENT:

# Format code
format:
	@echo "Formatting source code..."
	@find ./src -name '*.h' -or -name '*.cpp' | xargs clang-format -i
	@find ./inc -name '*.h' -or -name '*.cpp' | xargs clang-format -i
	@echo "Formatting done!"

# Build/clean all targets
all: build
clean:
	@-rm -rf $(BIN_DIR)
	@-rm -rf $(BUILD_DIR)
	@-rm -rf $(COVERAGE_DIR)
	@echo "Build cleaned!"

# Build targets
build: $(BUILD_DIR)/Makefile
	@echo "Starting build..."
	@mkdir -p $(BIN_DIR)
	make --silent -C $(BUILD_DIR) $(VERBOSE) $(PARALLEL_BUILD)
	@echo "Build done!"

$(BUILD_DIR)/Makefile:
	@echo "Generating makefiles..."
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	cd $(BUILD_DIR) && cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) -D BIN_DIR=$(BIN_DIR) $(ROOT_DIR)

# Test targets (only for x64 builds)
tests: $(BUILD_DIR)/Makefile
	@echo "Starting tests..."
	make --silent -C $(BUILD_DIR) test ARGS='--output-on-failure -T test --no-compress-output'
	@echo "Tests done!"

coverage:
	@echo "Generating coverage report..."
	@mkdir -p $(COVERAGE_DIR)
	cd 3rdparty/fastcov && python3 fastcov.py  --search-directory $(BUILD_DIR) --branch-coverage --exceptional-branch-coverage --lcov --output $(COVERAGE_DIR)/coverage.info
	lcov --ignore-errors unused --remove $(COVERAGE_DIR)/coverage.info '/usr/*' '$(ROOT_DIR)/**/_deps/**' '$(ROOT_DIR)/3rdparty/**' '$(ROOT_DIR)/tests/*.cpp' --rc lcov_branch_coverage=1 -o $(COVERAGE_DIR)/filtered_coverage.info
	eval "cd tools && python3 branch_coverage_filter.py $(COVERAGE_DIR)/filtered_coverage.info $(COVERAGE_DIR)/filtered_branch_coverage.info"
	genhtml $(COVERAGE_DIR)/filtered_branch_coverage.info --config-file $(ROOT_DIR)/.lcovrc --branch-coverage --demangle-cpp --output-directory $(COVERAGE_DIR)
	@echo "Coverage report done!"

# Phony targets
.PHONY: coverage

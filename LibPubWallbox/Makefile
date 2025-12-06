# Makefile for IsoStackCtrlProtocol library
# Author: Build automation
# Date: November 25, 2025

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
AR = ar
ARFLAGS = rcs

# Library files
LIB_NAME = libIsoStackCtrl.a
OBJ_FILES = IsoStackCtrlProtocol.o
SRC_FILES = IsoStackCtrlProtocol.cpp
HEADER_FILES = IsoStackCtrlProtocol.h

# Test program
TEST_PROG = test_iso
TEST_SRC = test_iso.cpp

.PHONY: all clean test help

# Default target
all: $(LIB_NAME)
	@echo "✓ Library built successfully: $(LIB_NAME)"

# Build the static library
$(LIB_NAME): $(OBJ_FILES)
	$(AR) $(ARFLAGS) $@ $^
	@echo "✓ Static library created: $@"

# Compile object files
%.o: %.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "✓ Compiled: $<"

# Build and run test
test: $(LIB_NAME) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -L. -lIsoStackCtrl -o $(TEST_PROG)
	@echo "✓ Test program compiled"
	@echo ""
	@echo "=== Running tests ==="
	./$(TEST_PROG)

# Clean build artifacts
clean:
	rm -f $(OBJ_FILES) $(LIB_NAME) $(TEST_PROG)
	@echo "✓ Cleaned build artifacts"

# Show help
help:
	@echo "IsoStackCtrlProtocol Library Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build the library"
	@echo "  make test     - Build and run tests"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make help     - Show this help"
	@echo ""
	@echo "Output:"
	@echo "  $(LIB_NAME)   - Static library file"
	@echo "  $(OBJ_FILES)  - Object files"

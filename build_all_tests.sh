#!/bin/bash
# Build script for SolarMonitor Tests (Linux/macOS)
# This script compiles all test files and the main executable

set -e

cd "$(dirname "$0")"

echo "==================================="
echo "Building SolarMonitor Project"
echo "==================================="

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Compile all test files
echo ""
echo -e "${BLUE}[1/3] Compiling test_inverter_gtest.cpp...${NC}"
g++ -fdiagnostics-color=always -g -std=c++17 -I include \
    tests/test_inverter_gtest.cpp src/Inverter.cpp src/Logger.cpp \
    -o test_inverter -lgtest -lgtest_main -pthread

if [ $? -eq 0 ]; then
    echo -e "${GREEN}[1/3] OK - test_inverter created${NC}"
else
    echo -e "${RED}ERROR: Failed to compile test_inverter_gtest.cpp${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}[2/3] Compiling test_full_system_gtest.cpp...${NC}"
g++ -fdiagnostics-color=always -g -std=c++17 -I include \
    tests/test_full_system_gtest.cpp src/Inverter.cpp src/Logger.cpp \
    -o test_full_system -lgtest -lgtest_main -pthread

if [ $? -eq 0 ]; then
    echo -e "${GREEN}[2/3] OK - test_full_system created${NC}"
else
    echo -e "${RED}ERROR: Failed to compile test_full_system_gtest.cpp${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}[3/3] Compiling main SolarMonitor executable...${NC}"
g++ -fdiagnostics-color=always -g -std=c++17 -I include \
    src/main.cpp src/Inverter.cpp src/Logger.cpp \
    -o SolarMonitor -pthread

if [ $? -eq 0 ]; then
    echo -e "${GREEN}[3/3] OK - SolarMonitor created${NC}"
else
    echo -e "${RED}ERROR: Failed to compile SolarMonitor${NC}"
    exit 1
fi

echo ""
echo "==================================="
echo -e "${GREEN}Build Complete!${NC}"
echo "==================================="
echo ""
echo "Executables created:"
echo "  - test_inverter"
echo "  - test_full_system"
echo "  - SolarMonitor"
echo ""
echo "To run tests:"
echo "  ./test_inverter"
echo "  ./test_full_system"
echo ""

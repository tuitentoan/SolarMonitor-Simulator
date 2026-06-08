@echo off
REM Build script for SolarMonitor Tests
REM This script compiles all test files and the main executable

cd /d "%~dp0"

echo ===================================
echo Building SolarMonitor Project
echo ===================================

REM Compile all test files
echo.
echo [1/3] Compiling test_inverter_gtest.cpp...
g++ -fdiagnostics-color=always -g -std=c++17 -I include ^
    tests/test_inverter_gtest.cpp src/Inverter.cpp src/Logger.cpp ^
    -o test_inverter.exe -lgtest -lgtest_main -pthread

if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to compile test_inverter_gtest.cpp
    exit /b 1
)

echo [1/3] OK - test_inverter.exe created

echo.
echo [2/3] Compiling test_full_system_gtest.cpp...
g++ -fdiagnostics-color=always -g -std=c++17 -I include ^
    tests/test_full_system_gtest.cpp src/Inverter.cpp src/Logger.cpp ^
    -o test_full_system.exe -lgtest -lgtest_main -pthread

if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to compile test_full_system_gtest.cpp
    exit /b 1
)

echo [2/3] OK - test_full_system.exe created

echo.
echo [3/3] Compiling main SolarMonitor executable...
g++ -fdiagnostics-color=always -g -std=c++17 -I include ^
    src/main.cpp src/Inverter.cpp src/Logger.cpp ^
    -o SolarMonitor.exe -pthread

if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to compile SolarMonitor
    exit /b 1
)

echo [3/3] OK - SolarMonitor.exe created

echo.
echo ===================================
echo Build Complete!
echo ===================================
echo.
echo Executables created:
echo   - test_inverter.exe
echo   - test_full_system.exe
echo   - SolarMonitor.exe
echo.
echo To run tests:
echo   .\test_inverter.exe
echo   .\test_full_system.exe
echo.

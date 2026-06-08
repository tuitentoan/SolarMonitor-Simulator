cmake_minimum_required(VERSION 3.12)
project(SolarMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/Inverter.cpp
    src/Logger.cpp
)

# Create main executable
add_executable(SolarMonitor src/main.cpp ${SOURCES})
target_include_directories(SolarMonitor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Google Test Configuration
enable_testing()

# Find or fetch Google Test
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    # If GTest not found, we'll assume it's available through system or manual installation
    # For manual setup, you can uncomment the following:
    # include(FetchContent)
    # FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip)
    # FetchContent_MakeAvailable(googletest)
endif()

# Test executable for Inverter
add_executable(test_inverter_gtest tests/test_inverter_gtest.cpp ${SOURCES})
target_include_directories(test_inverter_gtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(GTest_FOUND)
    target_link_libraries(test_inverter_gtest PRIVATE GTest::gtest GTest::gtest_main)
else()
    target_link_libraries(test_inverter_gtest PRIVATE gtest gtest_main)
endif()
add_test(NAME InverterTests COMMAND test_inverter_gtest)

# Test executable for Full System
add_executable(test_full_system_gtest tests/test_full_system_gtest.cpp ${SOURCES})
target_include_directories(test_full_system_gtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(GTest_FOUND)
    target_link_libraries(test_full_system_gtest PRIVATE GTest::gtest GTest::gtest_main)
else()
    target_link_libraries(test_full_system_gtest PRIVATE gtest gtest_main)
endif()
add_test(NAME FullSystemTests COMMAND test_full_system_gtest)

# Optional: Add pthread on Unix-like systems for thread safety
if(UNIX)
    target_link_libraries(test_full_system_gtest PRIVATE pthread)
    target_link_libraries(test_inverter_gtest PRIVATE pthread)
    target_link_libraries(SolarMonitor PRIVATE pthread)
endif()

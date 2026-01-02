cmake_minimum_required(VERSION 3.14)
project(FinanceManager)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCE_FILES
    src/FileStorage.cpp
    src/TransactionRepository.cpp
    src/StatisticsService.cpp
    src/NotificationService.cpp
    src/ImportExportService.cpp
    src/TransactionController.cpp
)

# Main executable
add_executable(finance_manager src/main.cpp ${SOURCE_FILES})

# Google Test setup
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test executables
add_executable(test_transaction_repository
    tests/test_transaction_repository.cpp
    src/FileStorage.cpp
    src/TransactionRepository.cpp
)

add_executable(test_statistics_service
    tests/test_statistics_service.cpp
    src/FileStorage.cpp
    src/TransactionRepository.cpp
    src/StatisticsService.cpp
)

add_executable(test_integration
    tests/test_integration.cpp
    src/FileStorage.cpp
    src/TransactionRepository.cpp
    src/StatisticsService.cpp
)

# Link Google Test to test executables
target_link_libraries(test_transaction_repository GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(test_statistics_service GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(test_integration GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

# Register tests
include(GoogleTest)
gtest_discover_tests(test_transaction_repository)
gtest_discover_tests(test_statistics_service)
gtest_discover_tests(test_integration)

# Add code coverage support (optional, for Linux/Mac)
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    endif()
endif()

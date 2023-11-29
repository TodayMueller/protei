cmake_minimum_required(VERSION 3.11)
project(locator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.9.2
)
FetchContent_MakeAvailable(spdlog)

add_executable(locator "main.cpp" "main.h" "Zone.cpp" "Subscriber.cpp" "Subscriber.h" "Zone.h"  "Triggers.h" "Triggers.cpp"  "Locator.cpp" "Locator.h" "Logger.cpp" "Logger.h")
target_link_libraries(locator PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(locator PRIVATE spdlog::spdlog)

add_executable(test_locator "unitTests.cpp" "Zone.cpp" "Subscriber.cpp" "Zone.h"  "Triggers.h" "Triggers.cpp"  "Locator.cpp" "Locator.h" "Logger.cpp" "Logger.h")
target_link_libraries(test_locator PRIVATE gtest_main nlohmann_json::nlohmann_json)
target_link_libraries(test_locator PRIVATE gtest_main spdlog::spdlog)

include(GoogleTest)
gtest_discover_tests(test_locator)
cmake_minimum_required(VERSION 3.20)
project(smallchat)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
        http-ev
        GIT_REPOSITORY /Users/ziyihuang/source/libhttp-ev
)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.7.3
)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
FetchContent_MakeAvailable(http-ev json)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} http-ev nlohmann_json::nlohmann_json)

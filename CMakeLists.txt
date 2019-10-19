cmake_minimum_required(VERSION 3.1)
project(newland)

set(SFML_STATIC_LIBRARIES TRUE)
find_package(
    SFML 2.5
    COMPONENTS graphics audio
    REQUIRED)

add_definitions(-DTMX_STATIC)
include_directories(libraries)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/newland/*.cpp ${PROJECT_SOURCE_DIR}/libraries/*.cpp
     ${PROJECT_SOURCE_DIR}/libraries/*.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio)

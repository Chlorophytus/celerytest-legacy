cmake_minimum_required(VERSION 3.15)

# You can change the name of the project
project(celerytest VERSION 0.1.0.3)

# First we configure the project
configure_file(include/${PROJECT_NAME}_cfg.in include/${PROJECT_NAME}_cfg.hpp)

# Find GLEW
find_package(GLEW REQUIRED)
# Find Lua
find_package(Lua REQUIRED)
# Find the SDL2 package
find_package(SDL2 REQUIRED)

# GLEW, Lua, SDL2 headers
include_directories(${GLEW_INCLUDE_DIRS} ${LUA_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

# If you add classes you should add them here
add_executable(${PROJECT_NAME} src/${PROJECT_NAME}.cpp src/${PROJECT_NAME}_interwork.cpp src/${PROJECT_NAME}_lua.cpp)

# Link to GLEW, Lua, and SDL2
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES} ${LUA_LIBRARIES} ${SDL2_LIBRARIES})
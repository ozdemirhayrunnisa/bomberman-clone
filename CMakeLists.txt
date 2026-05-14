cmake_minimum_required(VERSION 3.16)
project(BombermanClone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics)

add_executable(BombermanClone
    src/main.cpp
    src/Game.cpp
    src/Map.cpp
    src/Player.cpp
    src/Bomb.cpp
    src/Explosion.cpp
)

target_include_directories(BombermanClone PRIVATE src)
target_link_libraries(BombermanClone PRIVATE SFML::Graphics)

if(WIN32)
    add_custom_command(TARGET BombermanClone POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:BombermanClone>
            $<TARGET_FILE_DIR:BombermanClone>
        COMMAND_EXPAND_LISTS
    )
endif()

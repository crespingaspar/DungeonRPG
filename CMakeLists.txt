cmake_minimum_required(VERSION 3.16)
project(DungeonRPG)

set(CMAKE_CXX_STANDARD 17)
set(SFML_DIR "D:/SFML/SFML-2.6.1/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(DungeonRPG WIN32
    main.cpp
    Game.cpp
    Player.cpp
    Map.cpp
    Enemy.cpp
    Bullet.cpp
    Item.cpp
    Boss.cpp
    Minimap.cpp
    MainMenu.cpp
    SlotSelect.cpp
    Settings.cpp
)

target_include_directories(DungeonRPG PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(DungeonRPG PRIVATE sfml-graphics sfml-window sfml-system)
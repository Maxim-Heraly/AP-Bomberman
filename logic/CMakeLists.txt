# Logic library: intentionally has ZERO graphics dependencies, so it can be
# compiled and unit-tested in complete isolation (see section 3.1, "Code
# Design" - "you should be able to compile this logic library without
# having SFML installed").
add_library(bomberman_logic STATIC
    src/entities/EntityModel.cpp
    src/entities/Character.cpp
    src/entities/Bot.cpp
    src/entities/Bomb.cpp
    src/entities/Wall.cpp
    src/entities/FirePowerUp.cpp
    src/entities/BombPowerUp.cpp
    src/entities/SkatesPowerUp.cpp
    src/World.cpp
    src/Score.cpp
)

target_include_directories(bomberman_logic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(bomberman_logic PUBLIC cxx_std_17)

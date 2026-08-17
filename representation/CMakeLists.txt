find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_library(bomberman_representation STATIC
    src/Game.cpp
    src/ConcreteFactory.cpp
    src/AudioManager.cpp
    src/states/StateManager.cpp
    src/states/MenuState.cpp
    src/states/PlayState.cpp
    src/views/CharacterView.cpp
    src/views/BombView.cpp
    src/views/WallView.cpp
    src/views/PowerUpView.cpp
    src/views/ScoreView.cpp
)

target_include_directories(bomberman_representation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(bomberman_representation PUBLIC
    bomberman_logic
    sfml-graphics sfml-window sfml-system sfml-audio
)

target_compile_features(bomberman_representation PUBLIC cxx_std_17)

# Lets code reference assets/ regardless of the working directory the
# executable happens to be launched from, e.g.:
#   texture.loadFromFile(std::string(ASSET_DIR) + "/spritesheets/bomberman_spritesheet.png")
# See assets/spritesheets/README.md.
target_compile_definitions(bomberman_representation PUBLIC
    ASSET_DIR="${CMAKE_SOURCE_DIR}/assets"
)

add_executable(bomberman src/main.cpp)
target_link_libraries(bomberman PRIVATE bomberman_representation)

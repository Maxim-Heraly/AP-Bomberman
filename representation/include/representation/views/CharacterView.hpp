#pragma once

#include "logic/entities/Character.hpp"
#include "representation/views/EntityView.hpp"
#include <array>
#include <memory>

namespace bomberman::representation {

/**
 * @brief Identifies which character spritesheet variant should be used.
 */
enum class CharacterSpriteVariant { Player, Bot1, Bot2, Bot3 };

/**
 * @brief Contains all animation frames needed to display a character.
 *
 * Idle and walking animations are direction-dependent, while winning and
 * death animations use their own frame sequences.
 */
struct CharacterAnimationSet {
    std::array<sf::IntRect, 4> idle;                          ///< One idle frame for each direction.
    std::array<std::array<sf::IntRect, 4>, 4> walk;          ///< Four walking frames for each direction.
    std::array<sf::IntRect, 12> win;                          ///< Frames used for the victory animation.
    std::array<sf::IntRect, 12> dead;                         ///< Frames used for the death animation.
};

/**
 * @brief Creates the animation frame set corresponding to a character variant.
 */
CharacterAnimationSet makeCharacterAnimationSet(CharacterSpriteVariant variant);

/**
 * @brief View responsible for rendering and animating a Character model.
 *
 * The view observes character events and switches between idle, walking,
 * victory and death animations accordingly.
 */
class CharacterView : public EntityView {
public:
    /**
     * @brief Creates a character view using its model, texture and animation frames.
     */
    CharacterView(std::shared_ptr<const logic::Character> model,
                  std::shared_ptr<sf::Texture> texture,
                  const CharacterAnimationSet& animationSet);

    /**
     * @brief Advances the active animation according to the elapsed time.
     */
    void update(float deltaTime) override;

    /**
     * @brief Reacts to character events such as death and victory.
     */
    void onNotify(const logic::Subject& source, logic::EventType event) override;

    /**
     * @brief Selects the appropriate animation frame and draws the character.
     */
    void draw(sf::RenderWindow& window, const Camera& camera) override;

    /**
     * @brief Returns the rendering layer used for characters.
     */
    [[nodiscard]] int getDrawLayer() const override { return 1; }

    /**
     * @brief Starts the character's victory animation.
     */
    void setWon();

    /**
     * @brief Starts the character's death animation.
     */
    void setDead();

private:
    std::shared_ptr<const logic::Character> model; ///< Character model providing position, movement and facing state.
    sf::Sprite sprite;                             ///< SFML sprite used to render the character.
    std::shared_ptr<sf::Texture> texture;          ///< Shared spritesheet texture used by the sprite.
    CharacterAnimationSet animationSet;            ///< Sprite rectangles for all character animations.

    float frameTimer{0.f};                         ///< Accumulated time since the current animation frame started.
    std::size_t frameIndex{0};                     ///< Index of the currently displayed animation frame.
    logic::Direction lastFacing{logic::Direction::Down}; ///< Last direction used to detect facing changes.
    bool won{false};                               ///< Whether the victory animation is currently active.
    bool dead{false};                              ///< Whether the death animation is currently active.
};

} // namespace bomberman::representation
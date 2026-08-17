#pragma once

#include <SFML/Audio.hpp>

namespace bomberman::representation {

/**
 * @brief Singleton owning all background music and one-shot sound effects.
 *
 * Lives in representation (not logic) since it depends on SFML. Mirrors
 * logic::Stopwatch / logic::Random: a single self-contained instance,
 * callable from anywhere (MenuState, PlayState, ...).
 *
 * Guarantees menu music and gameplay music never overlap: both
 * playMenuMusic() and playGameplayMusic() stop whatever background track
 * is currently playing before starting their own.
 */
class AudioManager {
public:
    static AudioManager& getInstance() {
        static AudioManager instance;
        return instance;
    }

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    /// Starts looping the menu theme. Stops gameplay music first if it was
    /// playing. No-ops if the menu theme is already playing.
    void playMenuMusic();

    /// Starts looping the gameplay theme. Stops menu music first if it was
    /// playing. No-ops if the gameplay theme is already playing.
    void playGameplayMusic();

    /// Stops whichever background track (menu or gameplay) is currently
    /// playing, if any. Does not touch the victory/loss stingers.
    void stopMusic();

    /// One-shot stingers for the end of a run. Both stop the gameplay/menu
    /// music first, per design: no background track should keep playing
    /// under the result sound.
    void playVictorySound();
    void playLossSound();

private:
    AudioManager();

    enum class Track { None, Menu, Gameplay };
    void playLoopingTrack(sf::Music& music, Track which);

    sf::Music menuMusic;
    sf::Music gameplayMusic;
    Track currentTrack{Track::None};

    // SoundBuffer must outlive the Sound that references it - hence
    // declared first, so it's destroyed last (reverse construction order).
    sf::SoundBuffer victoryBuffer;
    sf::SoundBuffer lossBuffer;
    sf::Sound victorySound;
    sf::Sound lossSound;
};

} // namespace bomberman::representation
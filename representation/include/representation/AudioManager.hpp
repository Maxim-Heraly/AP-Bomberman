#pragma once

#include <SFML/Audio.hpp>

namespace bomberman::representation {

/**
 * @brief Singleton responsible for managing background music and sound effects.
 */
class AudioManager {
public:
    /// Returns the single shared AudioManager instance.
    static AudioManager& getInstance() {
        static AudioManager instance;
        return instance;
    }

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    /// Starts the looping menu music, unless it is already playing.
    void playMenuMusic();

    /// Starts the looping gameplay music, unless it is already playing.
    void playGameplayMusic();

    /// Stops all currently playing music and sound effects.
    void stopMusic();

    /// Stops other audio and plays the victory sound effect.
    void playVictorySound();

    /// Stops other audio and plays the loss sound effect.
    void playLossSound();

private:
    /// Loads all music and sound-effect resources used by the manager.
    AudioManager();

    /// Identifies which background music track is currently active.
    enum class Track { None, Menu, Gameplay };

    /// Switches to the given music track, avoiding unnecessary restarts.
    void playLoopingTrack(sf::Music& music, Track which);

    /// Background music for the main menu, streamed from disk.
    sf::Music menuMusic;

    /// Background music used during gameplay, streamed from disk.
    sf::Music gameplayMusic;

    /// Tracks which background music is currently selected.
    Track currentTrack{Track::None};

    // SoundBuffer must outlive the Sound that references it - hence
    // declared first, so it's destroyed last (reverse construction order).
    /// Audio data for the victory sound effect.
    sf::SoundBuffer victoryBuffer;

    /// Audio data for the loss sound effect.
    sf::SoundBuffer lossBuffer;

    /// Plays the loaded victory sound effect.
    sf::Sound victorySound;

    /// Plays the loaded loss sound effect.
    sf::Sound lossSound;
};

} // namespace bomberman::representation
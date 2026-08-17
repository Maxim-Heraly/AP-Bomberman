#include "representation/AudioManager.hpp"
#include <iostream>

namespace bomberman::representation {

AudioManager::AudioManager() {
    // Background tracks stream from disk (sf::Music) rather than loading
    // fully into memory - appropriate for looping tracks that can be long.
    if (!menuMusic.openFromFile("../../assets/audio/menu_music.mp3")) {
        std::cerr << "AudioManager: failed to load menu music\n";
    }
    menuMusic.setLoop(true);

    if (!gameplayMusic.openFromFile("../../assets/audio/play_music.mp3")) {
        std::cerr << "AudioManager: failed to load gameplay music\n";
    }
    gameplayMusic.setLoop(true);

    // Short one-shot stingers use SoundBuffer+Sound instead of Music: small
    // enough to load fully into memory, and fire instantly with no
    // streaming start-up latency.
    if (!victoryBuffer.loadFromFile("../../assets/audio/victory.mp3")) {
        std::cerr << "AudioManager: failed to load victory sound\n";
    }
    victorySound.setBuffer(victoryBuffer);

    if (!lossBuffer.loadFromFile("../../assets/audio/game-over.mp3")) {
        std::cerr << "AudioManager: failed to load loss sound\n";
    }
    lossSound.setBuffer(lossBuffer);
}

void AudioManager::playLoopingTrack(sf::Music& music, Track which) {
    if (currentTrack == which && music.getStatus() == sf::Music::Playing) {
        return; // Already the active track - don't restart it from 0.
    }
    stopMusic(); // Stop whatever else was playing first, so tracks never overlap.
    music.play();
    currentTrack = which;
}

void AudioManager::playMenuMusic() {
    playLoopingTrack(menuMusic, Track::Menu);
}

void AudioManager::playGameplayMusic() {
    playLoopingTrack(gameplayMusic, Track::Gameplay);
}

void AudioManager::stopMusic() {
    // Only one of these is ever actually playing, but stopping both
    // unconditionally is cheap and stays correct even if a future
    // call-site forgets to track state itself.
    menuMusic.stop();
    gameplayMusic.stop();
    victorySound.stop();
    lossSound.stop();
    currentTrack = Track::None;
}

void AudioManager::playVictorySound() {
    stopMusic();
    lossSound.stop();
    victorySound.play();
}

void AudioManager::playLossSound() {
    stopMusic();
    victorySound.stop();
    lossSound.play();
}

} // namespace bomberman::representation
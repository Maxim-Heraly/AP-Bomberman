#include "logic/Score.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace bomberman::logic {
void Score::onNotify(const Subject& source, const EventType event) {
    //only events that directly affect the score are handled here, other events are ignored
    switch (event) {
    case EventType::PlayerDestroyedBlock:
        addBlockDestroyed();
        break;
    case EventType::PlayerCollectedPowerUp:
        addPowerUpCollected();
        break;
    case EventType::EnemyKilled:
        addEnemyKilled();
        break;
    case EventType::PlayerWon:
        addPlayerWon();
        break;
    case EventType::PlayerLost:
        addPlayerLost();
        break;
    default:
        break; // Moved / BombPlaced / BombExploded / Died aren't scored directly
    }
}

void Score::loadHighScores(const std::string& path) {
    highScores.clear();
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return; // No scores saved yet (e.g. first run ever) - that's expected, not an error.
        }
        std::string line;
        while (std::getline(file, line)) {
            const auto comma = line.find(',');
            if (comma == std::string::npos) {
                continue; // Skip malformed lines rather than crashing on a hand-edited/corrupt file.
            }
            HighScoreEntry entry;
            entry.name = line.substr(0, comma);
            try {
                entry.score = std::stoi(line.substr(comma + 1));
            } catch (const std::exception&) {
                continue;
            }
            highScores.push_back(entry);
        }
        std::sort(highScores.begin(), highScores.end(),
                  [](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score > b.score; });
        if (highScores.size() > 5) {
            highScores.resize(5);
        }
    } catch (const std::exception& e) {
        std::cerr << "Score::loadHighScores: unexpected error reading '" << path << "': " << e.what() << '\n';
        highScores.clear();
    }
}

void Score::saveHighScores(const std::string& path) const {
    // Insert this run's score into a *copy* of the current top-5, then keep
    // only the best 5 entries, so a low score simply doesn't make the cut.
    std::vector<HighScoreEntry> updated = highScores;
    updated.push_back({"Player", currentScore});
    std::sort(updated.begin(), updated.end(),
              [](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score > b.score; });
    if (updated.size() > 5) {
        updated.resize(5);
    }

    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("could not open '" + path + "' for writing");
        }
        for (const auto& entry : updated) {
            file << entry.name << ',' << entry.score << '\n';
        }
    } catch (const std::exception& e) {
        std::cerr << "Score::saveHighScores: " << e.what() << '\n';
    }
}

} // namespace bomberman::logic

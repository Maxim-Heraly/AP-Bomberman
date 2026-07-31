#pragma once

#include "logic/patterns/Observer.hpp"
#include <string>
#include <vector>

namespace bomberman::logic {

struct HighScoreEntry {
    std::string name;
    int score{0};
};

/**
 * @brief Observer that turns game events into a score, and persists the
 * top-5 high scores to disk between runs (section 3.1, "Score").
 *
 * TODO: pick point values per event (e.g. BlockDestroyed +10, PowerUpCollected
 * +25, an enemy Died (not the Player) +100, PlayerWon a big bonus,
 * PlayerLost a penalty - see section 2.1, "Scoring"). Also track "time
 * alive" - you'll likely want to poll Stopwatch on every Tick event rather
 * than compute it from wall-clock time directly.
 */
class Score : public Observer {
public:
    void onNotify(const Subject& source, EventType event) override; // TODO

    int getCurrentScore() const { return currentScore; }

    /// TODO: read/write e.g. a simple "name,score" text format to disk. Wrap
    /// file I/O in try/catch (section 3.2, "Include exception handling").
    void loadHighScores(const std::string& path);
    void saveHighScores(const std::string& path) const;

    const std::vector<HighScoreEntry>& getTopFive() const { return highScores; }

private:
    int currentScore{0};
    std::vector<HighScoreEntry> highScores; // TODO: keep sorted descending, capped at 5 entries.
};

} // namespace bomberman::logic

#pragma once

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
 */
class Score {
public:

    [[nodiscard]] int getCurrentScore() const { return currentScore; }
    void addEnemyKilled() { currentScore += 100; }
    void addBlockDestroyed() { currentScore += 10; }
    void addPowerUpCollected() { currentScore += 25; }
    void addPlayerWon() { currentScore += 500; }
    void addPlayerLost() { currentScore -= 100; }

    /// TODO: read/write e.g. a simple "name,score" text format to disk. Wrap
    /// file I/O in try/catch (section 3.2, "Include exception handling").
    void loadHighScores(const std::string& path);
    void saveHighScores(const std::string& path) const;

    [[nodiscard]] const std::vector<HighScoreEntry>& getTopFive() const { return highScores; }

private:
    int currentScore{0};

    std::vector<HighScoreEntry> highScores; // TODO: keep sorted descending, capped at 5 entries.
};

} // namespace bomberman::logic

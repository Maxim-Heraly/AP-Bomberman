#pragma once

#include <memory>

#include "logic/patterns/Observer.hpp"
#include <string>
#include <vector>

namespace bomberman::logic {

/**
 * @brief Represents one entry in the persistent high-score table.
 */
struct HighScoreEntry {
    std::string name; ///< Name or initials of the player who achieved the score.
    int score{0};     ///< Score achieved by the player.
};

/**
 * @brief Observer that converts game events into score changes and manages
 *        the persistent top-five high-score table.
 */
class Score : public Observer {
public:
    /**
     * @brief Returns the singleton Score instance.
     */
    static std::shared_ptr<Score> getInstance() {
        static auto instance = std::shared_ptr<Score>(new Score);
        return instance;
    }

    Score(const Score&) = delete;
    Score& operator=(const Score&) = delete;

    /**
     * @brief Updates the current score when a relevant game event occurs.
     */
    void onNotify(const Subject& source, EventType event) override;

    /**
     * @brief Returns the player's current score.
     */
    [[nodiscard]] int getCurrentScore() const { return currentScore; }

    /**
     * @brief Replaces the current score with the given value.
     */
    void setCurrentScore(const int score) { currentScore = score; }

    /**
     * @brief Adds 100 points for defeating an enemy.
     */
    void addEnemyKilled() { currentScore += 100; }

    /**
     * @brief Adds 10 points for destroying a block.
     */
    void addBlockDestroyed() { currentScore += 10; }

    /**
     * @brief Adds 25 points for collecting a power-up.
     */
    void addPowerUpCollected() { currentScore += 25; }

    /**
     * @brief Adds 500 points when the player wins.
     */
    void addPlayerWon() { currentScore += 500; }

    /**
     * @brief Removes 100 points when the player loses.
     */
    void addPlayerLost() { currentScore -= 100; }

    /**
     * @brief Adds one point for each scored game tick.
     */
    void addTick() { currentScore += 1; }

    /**
     * @brief Loads the saved high scores from a comma-separated text file.
     */
    void loadHighScores(const std::string& path);

    /**
     * @brief Saves the current score together with the best existing scores.
     */
    void saveHighScores(const std::string& path) const;

    /**
     * @brief Returns the current top-five high-score entries.
     */
    [[nodiscard]] const std::vector<HighScoreEntry>& getTopFive() const { return highScores; }

private:
    /**
     * @brief Private constructor used to enforce the singleton pattern.
     */
    Score() : currentScore{0} {}

    int currentScore{0}; ///< Score accumulated during the current game.

    std::vector<HighScoreEntry> highScores; ///< Saved high scores, sorted from highest to lowest.
};

} // namespace bomberman::logic
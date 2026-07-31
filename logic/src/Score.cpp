#include "logic/Score.hpp"
#include <fstream>

namespace bomberman::logic {

void Score::onNotify(const Subject& /*source*/, EventType event) {
    // TODO: switch (event) { case EventType::BlockDestroyed: currentScore_ += ...; ... }
    // See section 2.1 "Scoring" for the required factors.
    (void)event;
}

void Score::loadHighScores(const std::string& path) {
    // A missing file on the very first run is expected, not an error - only
    // report genuinely unexpected failures (section 3.2, "exception handling").
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return;
        }
        // TODO: parse lines (e.g. "name,score") into highScores_, then sort
        // descending by score and keep only the top 5 (section 2.1, "Scoring").
    } catch (const std::exception& /*e*/) {
        // TODO: surface e.what() through whatever error-reporting you settle on.
    }
}

void Score::saveHighScores(const std::string& path) const {
    // TODO: write highScores_ to disk (e.g. "name,score" per line), wrapped
    // in try/catch, so a new top-5 entry survives between runs of the game.
    (void)path;
}

} // namespace bomberman::logic

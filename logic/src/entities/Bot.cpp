#include "logic/entities/Bot.hpp"
#include "logic/World.hpp"

namespace bomberman::logic {

void Bot::decideNextMove(const World& /*world*/) {
    // TODO: implement the required AI behaviours, see Bot.hpp for the full
    // list. A simple approach: implement each behaviour as its own private
    // helper (tryFlee(), tryCollectPowerUp(), tryBreakWalls(), tryAttack())
    // and call them in priority order - the first one that "does something"
    // (e.g. returns true) wins for this tick.
}

} // namespace bomberman::logic

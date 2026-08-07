#pragma once

#include "logic/patterns/Observer.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace bomberman::logic {

/**
 * @brief Abstract Subject interface.
 * Design note: observers are stored as weak_ptr so a Subject never extends
 * an Observer's lifetime. Concretely: whoever creates a View (the
 * ConcreteFactory, see representation/ConcreteFactory.hpp) keeps the
 * shared_ptr<EntityView> alive - the Model only ever borrows it.
 */
class Subject {
public:
    virtual ~Subject() = default;

    void attach(const std::shared_ptr<Observer>& observer) {
        observers.push_back(observer);
    }

    void detach(const std::shared_ptr<Observer>& observer) {
        observers.erase(
            std::remove_if(observers.begin(), observers.end(),
                [&observer](const std::weak_ptr<Observer>& weak) {
                    return weak.expired() || weak.lock() == observer;
                }),
            observers.end());
    }

protected:
    /// Notify every still-alive Observer. Call this from subclasses whenever
    /// their state changes in a way Views/Score should know about, e.g.
    /// `notify(EventType::Moved);` at the end of Character::update().
    void notify(EventType event) {
        for (auto it = observers.begin(); it != observers.end();) {
            if (auto observer = it->lock()) {
                observer->onNotify(*this, event);
                ++it;
            } else {
                it = observers.erase(it); // Clean up expired observers as we go.
            }
        }
    }

private:
    std::vector<std::weak_ptr<Observer>> observers;
};

} // namespace bomberman::logic

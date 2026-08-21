# Bomberman — Advanced Programming Project 2025-2026

**Name:** Maxim Heraly \\
**Student number:** 20232193 \\
**GitHub repo:** https://github.com/Maxim-Heraly/AP-Bomberman.git


## Project layout

```
logic/            SFML-free game-logic static library (bomberman_logic)
  include/logic/  public headers
  src/            implementation
representation/  SFML-based views, factory, states and the Game class
  include/representation/
  src/
tests/            small, dependency-free tests for the logic library
assets/           game assets (spritesheets, fonts, sounds, arenas)
.circleci/config.yml   CI build (install deps, cmake, build, ctest)
class diagram/       class diagram of the project with README for explanation
```

## Design decisions

The class diagram can be found in the 'class diagram' directory.

Some choices I made in the design of the project are:
1. making score and audiomanager a singleton:

Since there is only 1 score in the game (player's score), it makes sense to make it a singleton. 
This way, we can easily access the score from anywhere in the code without having to pass it around as a parameter.
Same goes for the audiomanager, since there is only 1 audio manager in the game, it makes sense to make it a singleton as well.

2. Observer lifetime: weak_ptr in Subject, but shared_ptr in the View

Views keep the observers alive, so they are stored as shared_ptr in the View. 
However, the Subject does not keep the observers alive, so they are stored as weak_ptr in the Subject. 
This way, we avoid circular references and memory leaks.
This also allows a bomb marked dead by world and therefore erased from entities to still get the blastprofile for the animation.

## Extensions implemented

1. I added music to the menu, the game, and sound effects for a win or loss.
2. Next to the required death animation, I also added a win animation which was deemed 'extra'.

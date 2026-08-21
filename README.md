# Bomberman — Advanced Programming Project 2025-2026

**Name:** Maxim Heraly
**Student number:** 20232193
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
assets/spritesheets/   put your spritesheet(s) here — see the README there
.circleci/config.yml   CI build (install deps, cmake, build, ctest)
```

## Design decisions

The class diagram can be found in the 'class diagram' directory.

Some choices I made in the design of the project are:
1. making score a singleton:

Since there is only 1 score in the game (player's score), it makes sense to make it a singleton. 
This way, we can easily access the score from anywhere in the code without having to pass it around as a parameter.

2. 

## Extensions implemented

1. I added music to the menu, the game, and sound effects for a win or loss.
2. Next to the required death animation, I also added a win animation which was deemed 'extra'.

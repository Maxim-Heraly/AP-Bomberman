# Fonts

`DejaVuSans.ttf` is bundled here so the game's UI text (scoreboard, HUD,
buttons) doesn't depend on whatever fonts happen to be installed on the
machine it's built/run on. DejaVu Sans is released under a permissive,
freely-redistributable license (Bitstream Vera-derived) - see
https://dejavu-fonts.github.io/License.html.

Loaded once by `bomberman::representation::ResourceManager` (see
`representation/include/representation/ResourceManager.hpp`) via the same
`ASSET_DIR` compile-time macro used for spritesheets.

TODO: if you'd rather use a different/custom font (e.g. something more
game-y/pixel-art styled to match your spritesheet), just replace this file
and update `ResourceManager`'s file name accordingly.

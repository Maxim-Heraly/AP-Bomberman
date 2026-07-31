# Spritesheets

Place your spritesheet image(s) here, e.g. `bomberman_spritesheet.png` (see
Figure 2 in the assignment for an example sheet - not every sprite on it
needs to be used, and you're free to use a different sheet entirely).

## How the code should read this

`representation/CMakeLists.txt` defines a compile-time `ASSET_DIR` macro
pointing at this project's `assets/` folder, so your code can build a path
that works regardless of the working directory the executable happens to be
launched from:

```cpp
sf::Texture texture;
if (!texture.loadFromFile(std::string(ASSET_DIR) + "/spritesheets/bomberman_spritesheet.png")) {
    throw std::runtime_error("Could not load spritesheet");
}
```

To draw a single frame from the sheet, use `sf::Sprite::setTextureRect` with
an `sf::IntRect` computed from the frame's row/column and a fixed frame
width/height (in pixels):

```cpp
constexpr int frameWidth = 32, frameHeight = 32; // TODO: match your actual sheet
sprite.setTextureRect(sf::IntRect(column * frameWidth, row * frameHeight, frameWidth, frameHeight));
```

## TODO before you start on the Views

- Decide on a fixed frame size for your sheet and note it here, along with
  the row/column of each animation you plan to use (walk x4 directions,
  death, bomb-tick, explosion, powerups, ...).
- Consider a small `SpriteSheet` / `AnimationSet` helper class (not required
  by the assignment, but keeps row/column bookkeeping out of your View
  classes) if you end up with many animation states per entity.
- Load each `sf::Texture` **once** (e.g. cached in `ConcreteFactory`, or a
  small texture-manager) and reuse/share it across all Views of the same
  entity type, rather than reloading it from disk per instance.

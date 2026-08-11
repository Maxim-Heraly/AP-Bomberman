#pragma once

namespace bomberman::logic {
    constexpr int kArenaColumns = 15;
    constexpr int kArenaRows = 13;
    constexpr float kTileWidth = 2.f / static_cast<float>(kArenaColumns);
    constexpr float kTileHeight = 2.f / static_cast<float>(kArenaRows);
}
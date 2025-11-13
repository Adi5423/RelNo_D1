// Noise.hpp
// ----------
// Single include header for the RelNo_D1 library
// Gives access to all available noise map types.
// Usage:
//   #include "Noise.hpp"
//   auto noise = Noise::create_whitenoise(...);

#pragma once

namespace Noise {
    // Output mode for create_* functions
    enum class OutputMode {
        None,   // Just return the noise map, don't display or save
        Image,  // Save as image file
        Map     // Display preview in terminal (WhiteNoise only)
    };
}

#include "NoiseMaps/WhiteNoise/include/WhiteNoise.hpp"
#include "NoiseMaps/PerlinNoise/include/PerlinNoise.hpp"
#include "NoiseMaps/SimplexNoise/include/SimplexNoise.hpp"
#include "NoiseMaps/TerrainNoise/include/TerrainNoise.hpp"
#include "NoiseMaps/CaveNoise/include/CaveNoise.hpp"
#include "NoiseMaps/TilemapExport/include/TilemapExport.hpp"

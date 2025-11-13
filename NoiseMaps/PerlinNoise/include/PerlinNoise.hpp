// PerlinNoise.hpp
// ----------------
// A simple, dependency-free Perlin noise generator module.
//
// Usage:
//  #include "Noise.hpp"
//  auto map = Noise::create_perlinnoise(256, 256, 40.0f, 5, 1.0f, 0.5f, 2.0f, 0.0f, 42, "image", "perlin_noise.png");

#pragma once
#include <vector>
#include <string>

namespace Noise {

    // Forward declare OutputMode (from Noise.hpp)
    enum class OutputMode;

    class PerlinNoise {
    private:
        std::vector<int> p; // permutation table

    public:
        explicit PerlinNoise(int seed = -1);
        static float fade(float t);
        static float lerp(float a, float b, float t);
        static float grad(int hash, float x, float y);
        // Core 2D Perlin noise function: returns [0,1]
        float noise(float x, float y) const;
    };

    // Single-value sampling API: Sample noise at specific coordinates without generating full map
    float sample_perlin(
        float x,
        float y,
        float scale,
        int octaves,
        float frequency,
        float persistence,
        float lacunarity,
        float base = 0.0f,
        int seed = -1
    );

    std::vector<std::vector<float>> generate_perlin_map(
        int width,
        int height,
        float scale,
        int octaves,
        float frequency,
        float persistence,
        float lacunarity,
        float base,
        int seed = -1
    );

    // Save to grayscale PNG or JPEG (auto-detected from extension)
    // If outputDir is empty, uses default ImageOutput/ directory
    void save_perlin_image(const std::vector<std::vector<float>>& noise,
        const std::string& filename = "perlin_noise.png",
        const std::string& outputDir = "");

    /* Entry wrapper 
        - int width, height: output resolution
        - float scale : inverse zoom(higher->smoother / larger features)
        - int octaves : number of layers
        - float frequency : base frequency multiplier
        - float persistence : amplitude decay per octave
        - float lacunarity : frequency growth per octave
        - float base : global offset for shifting the pattern
        - int seed : for randomization based as seed number predefined by functions
        - mode : OutputMode::None, Image, or Map
        - outputDir : custom output directory (empty = default ImageOutput/) */

    std::vector<std::vector<float>> create_perlinnoise(
        int width,
        int height,
        float scale,
        int octaves,
        float frequency,
        float persistence,
        float lacunarity,
        float base,
        int seed = -1,
        OutputMode mode = OutputMode::Image,
        const std::string& filename = "perlin_noise.png",
        const std::string& outputDir = ""
    );

} // namespace Noise

// SimplexNoise.hpp
// ----------------
// Lightweight, self-contained 2D Simplex Noise implementation.
//
// Usage:
//   #include "Noise.hpp"
//   auto map = Noise::create_simplexnoise(512, 512, 40.0f, 5, 0.5f, 2.2f, 0.0f, 42, "image", "SimplexNoise.png");

#pragma once
#include <vector>
#include <string>

namespace Noise {

    // Forward declare OutputMode (from Noise.hpp)
    enum class OutputMode;

    class SimplexNoise {
    private:
        std::vector<int> perm;
        const float grad3[8][2] = {
            {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}
        };

        static constexpr float F2 = 0.36602540378f;  // (sqrt(3)-1)/2
        static constexpr float G2 = 0.2113248654f;  // (3-sqrt(3))/6

    public:
        explicit SimplexNoise(int seed = -1);
        float noise2D(float xin, float yin) const;
    };

    // Single-value sampling API: Sample noise at specific coordinates without generating full map
    float sample_simplex(
        float x,
        float y,
        float scale,
        int octaves,
        float persistence,
        float lacunarity,
        float base = 0.0f,
        int seed = -1
    );

    // Generate multi-octave Simplex noise map
    std::vector<std::vector<float>> generate_simplex_map(
        int width,
        int height,
        float scale,
        int octaves,
        float persistence,
        float lacunarity,
        float base = 0.0f,
        int seed = -1
    );

    // Save to grayscale PNG or JPEG (auto-detected from extension)
    // If outputDir is empty, uses default ImageOutput/ directory
    void save_simplex_image(const std::vector<std::vector<float>>& noise,
        const std::string& filename = "simplex_noise.png",
        const std::string& outputDir = "");

    // Entry wrapper � same structure as other noise types
    std::vector<std::vector<float>> create_simplexnoise(
        int width,
        int height,
        float scale,
        int octaves,
        float persistence,
        float lacunarity,
        float base = 0.0f,
        int seed = -1,
        OutputMode mode = OutputMode::Image,
        const std::string& filename = "simplex_noise.png",
        const std::string& outputDir = ""
    );

} // namespace Noise

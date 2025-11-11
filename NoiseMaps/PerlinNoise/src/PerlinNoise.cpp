#include "PerlinNoise.hpp"
#include <random>
#include <cmath>
#include <iostream>
#include <algorithm> // for std::shuffle
#include <filesystem>
#include "stb_image_write.h"

namespace Noise {

    // ---------------------------------------------------------
    // Constructor: initializes permutation table
    // ---------------------------------------------------------
    PerlinNoise::PerlinNoise(int seed) {
        p.resize(256);
        for (int i = 0; i < 256; ++i)
            p[i] = i;

        if (seed >= 0) {
            std::mt19937 rng(seed);
            std::shuffle(p.begin(), p.end(), rng);
        }
        else {
            std::random_device rd;
            std::mt19937 rng(rd());
            std::shuffle(p.begin(), p.end(), rng);
        }

        // duplicate for overflow safety
        p.insert(p.end(), p.begin(), p.end());
    }

    // ---------------------------------------------------------
    // Fade, Lerp, Grad functions (as per Ken Perlin)
    // ---------------------------------------------------------
    float PerlinNoise::fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    float PerlinNoise::lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    float PerlinNoise::grad(int hash, float x, float y) {
        int h = hash & 3;
        float u = (h < 2) ? x : y;
        float v = (h < 2) ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }

    // ---------------------------------------------------------
    // Core 2D Perlin noise value in [0,1]
    // ---------------------------------------------------------
    float PerlinNoise::noise(float x, float y) const {
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;

        float xf = x - std::floor(x);
        float yf = y - std::floor(y);

        float u = fade(xf);
        float v = fade(yf);

        int aa = p[p[X] + Y];
        int ab = p[p[X] + Y + 1];
        int ba = p[p[X + 1] + Y];
        int bb = p[p[X + 1] + Y + 1];

        float x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
        float x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);
        return (lerp(x1, x2, v) + 1.0f) / 2.0f;
    }

    // ---------------------------------------------------------
    // Multi-octave map generator
    // ---------------------------------------------------------
    std::vector<std::vector<float>> generate_perlin_map(
        int width,
        int height,
        float scale,
        int octaves,
        float frequency,
        float persistence,
        float lacunarity,
        float base,
        int seed
    ) {
        // Validate parameters
        if (width <= 0)
            throw std::invalid_argument("width must be > 0, got: " + std::to_string(width));
        if (height <= 0)
            throw std::invalid_argument("height must be > 0, got: " + std::to_string(height));
        if (scale <= 0.0f)
            throw std::invalid_argument("scale must be > 0, got: " + std::to_string(scale));
        if (octaves < 1)
            throw std::invalid_argument("octaves must be >= 1, got: " + std::to_string(octaves));
        if (frequency <= 0.0f)
            throw std::invalid_argument("frequency must be > 0, got: " + std::to_string(frequency));
        if (persistence < 0.0f || persistence > 1.0f)
            throw std::invalid_argument("persistence must be in [0,1], got: " + std::to_string(persistence));
        if (lacunarity <= 0.0f)
            throw std::invalid_argument("lacunarity must be > 0, got: " + std::to_string(lacunarity));

        PerlinNoise generator(seed);
        std::vector<std::vector<float>> noise(height, std::vector<float>(width, 0.0f));

        float amplitude = 1.0f;
        float maxAmplitude = 0.0f;
        float freq = frequency;

        for (int o = 0; o < octaves; ++o) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    float nx = (x + base) / scale * freq;
                    float ny = (y + base) / scale * freq;
                    noise[y][x] += generator.noise(nx, ny) * amplitude;
                }
            }
            maxAmplitude += amplitude;
            amplitude *= persistence;
            freq *= lacunarity;
        }

        // Normalize to [0,1] - consistent with SimplexNoise approach
        // Perlin noise() already returns [0,1], so just divide by max amplitude
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                noise[y][x] /= maxAmplitude;

        return noise;
    }

    // ---------------------------------------------------------
    // Save Perlin map to grayscale PNG or JPEG (auto-detected from extension)
    // ---------------------------------------------------------
    void save_perlin_image(const std::vector<std::vector<float>>& noise, const std::string& filename, const std::string& outputDir) {
        if (noise.empty() || noise[0].empty()) {
            throw std::invalid_argument("Cannot save empty noise map.");
        }

        int height = noise.size();
        int width = noise[0].size();

        std::vector<unsigned char> imgData(width * height);
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                imgData[y * width + x] = static_cast<unsigned char>(noise[y][x] * 255.0f);

        // Determine output directory: use custom or default
        std::filesystem::path outDir;
        if (outputDir.empty()) {
            outDir = std::filesystem::current_path().parent_path() / "ImageOutput";
        } else {
            outDir = outputDir;
        }
        std::filesystem::create_directories(outDir);
        std::filesystem::path outFile = outDir / filename;
        std::string extension = outFile.extension().string();
        
        // Convert extension to lowercase for comparison
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        int result = 0;
        if (extension == ".jpg" || extension == ".jpeg") {
            // Save as JPEG with quality 90 (range: 1-100, higher = better quality)
            result = stbi_write_jpg(outFile.string().c_str(), width, height, 1, imgData.data(), 90);
        } else {
            // Default to PNG
            result = stbi_write_png(outFile.string().c_str(), width, height, 1, imgData.data(), width);
        }

        if (result == 0) {
            throw std::runtime_error("Failed to write image file: " + outFile.string());
        }

        std::cout << "[OK] Perlin noise image saved at: " << outFile.string() << "\n";
    }

    // ---------------------------------------------------------
    // Wrapper like Python's create_perlinnoise()
    // ---------------------------------------------------------
    std::vector<std::vector<float>> create_perlinnoise(
        int width,
        int height,
        float scale,
        int octaves,
        float frequency,
        float persistence,
        float lacunarity,
        float base,
        int seed,
        OutputMode mode,
        const std::string& filename,
        const std::string& outputDir
    ) {
        auto noise = generate_perlin_map(width, height, scale, octaves, frequency, persistence, lacunarity, base, seed);

        switch (mode) {
            case OutputMode::Image:
                save_perlin_image(noise, filename, outputDir);
                break;
            case OutputMode::None:
                // Do nothing, just return the noise
                break;
            case OutputMode::Map:
                // PerlinNoise doesn't support terminal preview
                // Fall through to None behavior
                break;
        }

        return noise;
    }

} // namespace Noise

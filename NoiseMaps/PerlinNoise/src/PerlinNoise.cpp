#include "PerlinNoise.hpp"
#include <random>
#include <cmath>
#include <iostream>
#include <algorithm> // for std::shuffle, std::clamp
#include <filesystem>
//#define STB_IMAGE_WRITE_IMPLEMENTATION
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
        if (scale <= 0.0f)
            throw std::invalid_argument("scale must be > 0");
        if (octaves < 1)
            throw std::invalid_argument("octaves must be >= 1");

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
    // Save Perlin map to grayscale PNG
    // ---------------------------------------------------------
    void save_perlin_image(const std::vector<std::vector<float>>& noise, const std::string& filename) {
        if (noise.empty() || noise[0].empty()) {
            throw std::invalid_argument("Cannot save empty noise map.");
        }

        int height = noise.size();
        int width = noise[0].size();

        std::vector<unsigned char> imgData(width * height);
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                imgData[y * width + x] = static_cast<unsigned char>(noise[y][x] * 255.0f);

        std::filesystem::path outputDir = std::filesystem::current_path().parent_path() / "ImageOutput";
        std::filesystem::create_directories(outputDir);
        std::filesystem::path outFile = outputDir / filename;

        int result = stbi_write_png(outFile.string().c_str(), width, height, 1, imgData.data(), width);
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
        const std::string& showMap,
        const std::string& filename
    ) {
        auto noise = generate_perlin_map(width, height, scale, octaves, frequency, persistence, lacunarity, base, seed);

        if (showMap == "image")
            save_perlin_image(noise, filename);
        else if (showMap != "none")
            throw std::invalid_argument("Invalid showMap value. Use 'image' or 'none'.");

        return noise;
    }

} // namespace Noise

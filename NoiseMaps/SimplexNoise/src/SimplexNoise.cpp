#include "Noise.hpp"  // full OutputMode definition
#include "SimplexNoise.hpp"
#include <random>
#include <cmath>
#include <iostream>
#include <algorithm> // for std::shuffle, std::clamp
#include <filesystem>
#include "stb_image_write.h"

#ifndef __cpp_lib_clamp
namespace std {
    template<class T>
    const T& clamp(const T& v, const T& lo, const T& hi) {
        return (v < lo) ? lo : (hi < v ? hi : v);
    }
}
#endif


namespace Noise {

    // ---------------------------------------------------------
    // Constructor � creates permutation table
    // ---------------------------------------------------------
    SimplexNoise::SimplexNoise(int seed) {
        perm.resize(512);
        std::vector<int> p(256);
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

        for (int i = 0; i < 512; ++i)
            perm[i] = p[i & 255];
    }

    // ---------------------------------------------------------
    // 2D Simplex noise: returns value in [-1, 1]
    // ---------------------------------------------------------
    float SimplexNoise::noise2D(float xin, float yin) const {
        float s = (xin + yin) * F2;
        int i = static_cast<int>(std::floor(xin + s));
        int j = static_cast<int>(std::floor(yin + s));

        float t = (i + j) * G2;
        float X0 = i - t;
        float Y0 = j - t;
        float x0 = xin - X0;
        float y0 = yin - Y0;

        int i1, j1;
        if (x0 > y0) { i1 = 1; j1 = 0; }
        else { i1 = 0; j1 = 1; }

        float x1 = x0 - i1 + G2;
        float y1 = y0 - j1 + G2;
        float x2 = x0 - 1.0f + 2.0f * G2;
        float y2 = y0 - 1.0f + 2.0f * G2;

        int ii = i & 255;
        int jj = j & 255;
        int gi0 = perm[ii + perm[jj]] % 8;
        int gi1 = perm[ii + i1 + perm[jj + j1]] % 8;
        int gi2 = perm[ii + 1 + perm[jj + 1]] % 8;

        float n0, n1, n2;
        n0 = n1 = n2 = 0.0f;

        float t0 = 0.5f - x0 * x0 - y0 * y0;
        if (t0 >= 0.0f) {
            t0 *= t0;
            n0 = t0 * t0 * (grad3[gi0][0] * x0 + grad3[gi0][1] * y0);
        }

        float t1 = 0.5f - x1 * x1 - y1 * y1;
        if (t1 >= 0.0f) {
            t1 *= t1;
            n1 = t1 * t1 * (grad3[gi1][0] * x1 + grad3[gi1][1] * y1);
        }

        float t2 = 0.5f - x2 * x2 - y2 * y2;
        if (t2 >= 0.0f) {
            t2 *= t2;
            n2 = t2 * t2 * (grad3[gi2][0] * x2 + grad3[gi2][1] * y2);
        }

        // Scale constant for 2D
        return 70.0f * (n0 + n1 + n2);
    }

    // ---------------------------------------------------------
    // Single-value sampling: Sample Simplex noise at specific coordinates
    // ---------------------------------------------------------
    float sample_simplex(
        float x,
        float y,
        float scale,
        int octaves,
        float persistence,
        float lacunarity,
        float base,
        int seed
    ) {
        // Validate parameters
        if (scale <= 0.0f)
            throw std::invalid_argument("scale must be > 0, got: " + std::to_string(scale));
        if (octaves < 1)
            throw std::invalid_argument("octaves must be >= 1, got: " + std::to_string(octaves));
        if (persistence < 0.0f || persistence > 1.0f)
            throw std::invalid_argument("persistence must be in [0,1], got: " + std::to_string(persistence));
        if (lacunarity <= 0.0f)
            throw std::invalid_argument("lacunarity must be > 0, got: " + std::to_string(lacunarity));

        SimplexNoise noiseGen(seed);
        
        float value = 0.0f;
        float amplitude = 1.0f;
        float maxAmp = 0.0f;
        float frequency = 1.0f;

        for (int o = 0; o < octaves; ++o) {
            float nx = (x + base) / scale * frequency;
            float ny = (y + base) / scale * frequency;
            value += noiseGen.noise2D(nx, ny) * amplitude;
            
            maxAmp += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        // Normalize to [0,1]
        return (value / maxAmp) * 0.5f + 0.5f;
    }

    // ---------------------------------------------------------
    // Multi-octave Simplex map generator
    // ---------------------------------------------------------
    std::vector<std::vector<float>> generate_simplex_map(
        int width,
        int height,
        float scale,
        int octaves,
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
        if (persistence < 0.0f || persistence > 1.0f)
            throw std::invalid_argument("persistence must be in [0,1], got: " + std::to_string(persistence));
        if (lacunarity <= 0.0f)
            throw std::invalid_argument("lacunarity must be > 0, got: " + std::to_string(lacunarity));

        SimplexNoise noiseGen(seed);
        std::vector<std::vector<float>> noise(height, std::vector<float>(width, 0.0f));

        float amplitude = 1.0f;
        float maxAmp = 0.0f;
        float frequency = 1.0f;

        for (int o = 0; o < octaves; ++o) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    float nx = (x + base) / scale * frequency;
                    float ny = (y + base) / scale * frequency;
                    noise[y][x] += noiseGen.noise2D(nx, ny) * amplitude;
                }
            }
            maxAmp += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        // Normalize to [0,1]
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                noise[y][x] = (noise[y][x] / maxAmp) * 0.5f + 0.5f;

        return noise;
    }

    // ---------------------------------------------------------
    // Save as grayscale PNG or JPEG (auto-detected from extension)
    // ---------------------------------------------------------
    void save_simplex_image(const std::vector<std::vector<float>>& noise, const std::string& filename, const std::string& outputDir) {
        if (noise.empty() || noise[0].empty()) {
            throw std::invalid_argument("Cannot save empty noise map.");
        }

        int height = noise.size();
        int width = noise[0].size();

        std::vector<unsigned char> img(width * height);
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                img[y * width + x] = static_cast<unsigned char>(std::clamp(noise[y][x], 0.0f, 1.0f) * 255.0f);

        // Determine output directory: use custom or default
        std::filesystem::path outDir;
        if (outputDir.empty()) {
            outDir = std::filesystem::current_path().parent_path() / "ImageOutput";
        }
        else {
            outDir = outputDir;
        }
        std::filesystem::create_directories(outDir);
        std::filesystem::path outputFile = outDir / filename;
        std::string extension = outputFile.extension().string();

        // Convert extension to lowercase for comparison
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        int result = 0;
        if (extension == ".jpg" || extension == ".jpeg") {
            // Save as JPEG with quality 90 (range: 1-100, higher = better quality)
            result = stbi_write_jpg(outputFile.string().c_str(), width, height, 1, img.data(), 90);
        }
        else {
            // Default to PNG
            result = stbi_write_png(outputFile.string().c_str(), width, height, 1, img.data(), width);
        }

        if (result == 0) {
            throw std::runtime_error("Failed to write image file: " + outputFile.string());
        }

        std::cout << "[OK] Simplex noise image saved at: " << outputFile.string() << "\n";
    }

    // ---------------------------------------------------------
    // Wrapper � same API pattern as others
    // ---------------------------------------------------------
    std::vector<std::vector<float>> create_simplexnoise(
        int width,
        int height,
        float scale,
        int octaves,
        float persistence,
        float lacunarity,
        float base,
        int seed,
        OutputMode mode,
        const std::string& filename,
        const std::string& outputDir
    ) {
        auto noise = generate_simplex_map(width, height, scale, octaves, persistence, lacunarity, base, seed);

        switch (mode) {
        case OutputMode::Image:
            save_simplex_image(noise, filename, outputDir);
            break;
        case OutputMode::None:
            // Do nothing, just return the noise
            break;
        case OutputMode::Map:
            // SimplexNoise doesn't support terminal preview
            // Fall through to None behavior
            break;
        }

        return noise;
    }

} // namespace Noise

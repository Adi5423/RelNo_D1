// WhiteNoise.cpp
#include "WhiteNoise.hpp"
#include <iostream>
#include <random>
#include <fstream>
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <filesystem>

namespace Noise {

    // -------------------------------------------------------------
    // Generate white noise: returns a 2D vector of floats [0,1]
    // -------------------------------------------------------------
    std::vector<std::vector<float>> WhiteNoise::generate(int width, int height, int seed) {
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument("Width and height must be positive.");
        }

        std::vector<std::vector<float>> noise(height, std::vector<float>(width));

        // Random number generator setup
        std::mt19937 rng(seed >= 0 ? seed : std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        // Fill with random values
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                noise[y][x] = dist(rng);

        return noise;
    }

    // -------------------------------------------------------------
    // Show preview in terminal (optional)
    // -------------------------------------------------------------
    void WhiteNoise::show(const std::vector<std::vector<float>>& noise) {
        std::cout << "\n[Preview of White Noise Map]\n";
        int previewH = std::min((int)noise.size(), 10);
        int previewW = std::min((int)noise[0].size(), 20);

        for (int y = 0; y < previewH; ++y) {
            for (int x = 0; x < previewW; ++x) {
                char c = (noise[y][x] > 0.5f) ? '#' : '.';
                std::cout << c;
            }
            std::cout << "\n";
        }
        std::cout << "[...] (Preview truncated)\n";
    }

    // -------------------------------------------------------------
    // Save as grayscale PNG (0–255)
    // -------------------------------------------------------------
    void WhiteNoise::save(const std::vector<std::vector<float>>& noise, const std::string& filename) {
        if (noise.empty() || noise[0].empty()) {
            throw std::invalid_argument("Cannot save empty noise map.");
        }

        int height = noise.size();
        int width = noise[0].size();

        std::vector<unsigned char> imgData(width * height);

        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                imgData[y * width + x] = static_cast<unsigned char>(noise[y][x] * 255.0f);

        // Ensure ImageOutput directory exists at project root
        std::filesystem::path outputDir = std::filesystem::current_path().parent_path() / "ImageOutput";
        std::filesystem::create_directories(outputDir);

        // Save file there
        std::filesystem::path outputFile = outputDir / filename;
        int result = stbi_write_png(outputFile.string().c_str(), width, height, 1, imgData.data(), width);

        if (result == 0) {
            throw std::runtime_error("Failed to write image file: " + outputFile.string());
        }

        std::cout << "[OK] White noise image saved at: " << outputFile.string() << "\n";
    }

    // -------------------------------------------------------------
    // Python-style wrapper
    // -------------------------------------------------------------
    std::vector<std::vector<float>> create_whitenoise(int width, int height, int seed,
        const std::string& showMap, const std::string& filename) {
        auto noise = WhiteNoise::generate(width, height, seed);

        if (showMap == "map")
            WhiteNoise::show(noise);
        else if (showMap == "image")
            WhiteNoise::save(noise, filename);
        else if (showMap != "none")
            throw std::invalid_argument("Invalid showMap value. Use 'map', 'image', or 'none'.");

        return noise;
    }

} // namespace Noise

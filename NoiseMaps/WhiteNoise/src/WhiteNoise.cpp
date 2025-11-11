// WhiteNoise.cpp
#include "WhiteNoise.hpp"
#include <iostream>
#include <random>
#include "stb_image_write.h"
#include <filesystem>

namespace Noise {

    // -------------------------------------------------------------
    // Generate white noise: returns a 2D vector of floats [0,1]
    // -------------------------------------------------------------
    std::vector<std::vector<float>> WhiteNoise::generate(int width, int height, int seed) {
        // Validate parameters
        if (width <= 0) {
            throw std::invalid_argument("width must be > 0, got: " + std::to_string(width));
        }
        if (height <= 0) {
            throw std::invalid_argument("height must be > 0, got: " + std::to_string(height));
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
        if (noise.empty() || noise[0].empty()) {
            throw std::invalid_argument("Cannot show empty noise map.");
        }

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
    // Save as grayscale PNG or JPEG (auto-detected from extension)
    // -------------------------------------------------------------
    void WhiteNoise::save(const std::vector<std::vector<float>>& noise, const std::string& filename, const std::string& outputDir) {
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

        // Save file there
        std::filesystem::path outputFile = outDir / filename;
        std::string extension = outputFile.extension().string();
        
        // Convert extension to lowercase for comparison
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        int result = 0;
        if (extension == ".jpg" || extension == ".jpeg") {
            // Save as JPEG with quality 90 (range: 1-100, higher = better quality)
            result = stbi_write_jpg(outputFile.string().c_str(), width, height, 1, imgData.data(), 90);
        } else {
            // Default to PNG
            result = stbi_write_png(outputFile.string().c_str(), width, height, 1, imgData.data(), width);
        }

        if (result == 0) {
            throw std::runtime_error("Failed to write image file: " + outputFile.string());
        }

        std::cout << "[OK] White noise image saved at: " << outputFile.string() << "\n";
    }

    // -------------------------------------------------------------
    // Python-style wrapper
    // -------------------------------------------------------------
    std::vector<std::vector<float>> create_whitenoise(int width, int height, int seed,
        OutputMode mode, const std::string& filename, const std::string& outputDir) {
        auto noise = WhiteNoise::generate(width, height, seed);

        switch (mode) {
            case OutputMode::Map:
                WhiteNoise::show(noise);
                break;
            case OutputMode::Image:
                WhiteNoise::save(noise, filename, outputDir);
                break;
            case OutputMode::None:
                // Do nothing, just return the noise
                break;
        }

        return noise;
    }

} // namespace Noise

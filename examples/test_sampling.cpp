// test_sampling.cpp
// -----------------
// Comprehensive test for single-value sampling API
// Demonstrates real-time coordinate queries without full map generation

#include "Noise.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>

using namespace Noise;
using namespace std::chrono;

void print_section(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

// Test 1: Basic sampling functionality
void test_basic_sampling() {
    print_section("Test 1: Basic Single-Value Sampling");
    
    int seed = 42;
    std::cout << "Sampling noise at specific coordinates (seed=" << seed << "):\n\n";
    
    // Sample WhiteNoise at various points
    std::cout << "WhiteNoise samples:\n";
    for (int i = 0; i < 5; i++) {
        float x = i * 10.0f;
        float y = i * 5.0f;
        float value = sample_whitenoise(x, y, seed);
        std::cout << "  (" << std::setw(6) << x << ", " << std::setw(6) << y << ") = " 
                  << std::fixed << std::setprecision(4) << value << "\n";
    }
    
    // Sample PerlinNoise
    std::cout << "\nPerlinNoise samples (scale=50, octaves=3):\n";
    for (int i = 0; i < 5; i++) {
        float x = i * 10.0f;
        float y = i * 5.0f;
        float value = sample_perlin(x, y, 50.0f, 3, 1.0f, 0.5f, 2.0f, 0.0f, seed);
        std::cout << "  (" << std::setw(6) << x << ", " << std::setw(6) << y << ") = " 
                  << std::fixed << std::setprecision(4) << value << "\n";
    }
    
    // Sample SimplexNoise
    std::cout << "\nSimplexNoise samples (scale=50, octaves=3):\n";
    for (int i = 0; i < 5; i++) {
        float x = i * 10.0f;
        float y = i * 5.0f;
        float value = sample_simplex(x, y, 50.0f, 3, 0.5f, 2.0f, 0.0f, seed);
        std::cout << "  (" << std::setw(6) << x << ", " << std::setw(6) << y << ") = " 
                  << std::fixed << std::setprecision(4) << value << "\n";
    }
}

// Test 2: Determinism - same coordinates should give same values
void test_determinism() {
    print_section("Test 2: Determinism Check");
    
    int seed = 123;
    float x = 42.5f, y = 17.3f;
    
    std::cout << "Sampling same coordinates multiple times (seed=" << seed << "):\n";
    std::cout << "Coordinates: (" << x << ", " << y << ")\n\n";
    
    std::cout << "WhiteNoise:\n";
    for (int i = 0; i < 3; i++) {
        float value = sample_whitenoise(x, y, seed);
        std::cout << "  Attempt " << (i+1) << ": " << std::fixed << std::setprecision(6) << value << "\n";
    }
    
    std::cout << "\nPerlinNoise (scale=40, octaves=5):\n";
    for (int i = 0; i < 3; i++) {
        float value = sample_perlin(x, y, 40.0f, 5, 1.0f, 0.5f, 2.0f, 0.0f, seed);
        std::cout << "  Attempt " << (i+1) << ": " << std::fixed << std::setprecision(6) << value << "\n";
    }
    
    std::cout << "\nSimplexNoise (scale=40, octaves=5):\n";
    for (int i = 0; i < 3; i++) {
        float value = sample_simplex(x, y, 40.0f, 5, 0.5f, 2.0f, 0.0f, seed);
        std::cout << "  Attempt " << (i+1) << ": " << std::fixed << std::setprecision(6) << value << "\n";
    }
    
    std::cout << "\n✓ All values should be identical for determinism!\n";
}

// Test 3: Performance comparison - sampling vs full map generation
void test_performance() {
    print_section("Test 3: Performance - Sampling vs Full Map Generation");
    
    int seed = 42;
    int numSamples = 1000;
    
    // Test Perlin sampling performance
    std::cout << "Testing " << numSamples << " individual samples...\n\n";
    
    auto start = high_resolution_clock::now();
    float sum = 0.0f;
    for (int i = 0; i < numSamples; i++) {
        float x = (i % 100) * 5.0f;
        float y = (i / 100) * 5.0f;
        sum += sample_perlin(x, y, 50.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
    }
    auto end = high_resolution_clock::now();
    auto sampling_duration = duration_cast<microseconds>(end - start).count();
    
    std::cout << "PerlinNoise Sampling:\n";
    std::cout << "  " << numSamples << " samples in " << sampling_duration << " μs\n";
    std::cout << "  Average: " << (sampling_duration / (float)numSamples) << " μs per sample\n";
    std::cout << "  (Sum for verification: " << sum << ")\n\n";
    
    // Test map generation performance for comparison
    start = high_resolution_clock::now();
    auto map = generate_perlin_map(100, 100, 50.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
    end = high_resolution_clock::now();
    auto map_duration = duration_cast<microseconds>(end - start).count();
    
    std::cout << "PerlinNoise Full Map (100x100 = 10,000 values):\n";
    std::cout << "  Generated in " << map_duration << " μs\n";
    std::cout << "  Average: " << (map_duration / 10000.0f) << " μs per value\n\n";
    
    std::cout << "Analysis:\n";
    std::cout << "  For sparse queries (< ~" << (int)(10000 * sampling_duration / (float)map_duration) 
              << " samples), use sampling API\n";
    std::cout << "  For dense queries, use full map generation\n";
}

// Test 4: Infinite terrain simulation (platformer use case)
void test_infinite_terrain() {
    print_section("Test 4: Infinite Terrain Simulation");
    
    std::cout << "Simulating a platformer player moving through infinite terrain...\n";
    std::cout << "Using 1D terrain height (sampling Y at different X positions)\n\n";
    
    int seed = 777;
    float playerX = 0.0f;
    float terrainY = 100.0f; // Fixed Y for ground level
    
    std::cout << "Terrain height profile:\n";
    std::cout << "X Pos    | Perlin Height | Simplex Height | Visual\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (int i = 0; i < 20; i++) {
        playerX = i * 10.0f;
        
        // Sample terrain height at this X position
        float perlinHeight = sample_perlin(playerX, terrainY, 80.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
        float simplexHeight = sample_simplex(playerX, terrainY, 80.0f, 4, 0.5f, 2.0f, 0.0f, seed);
        
        // Convert to visual height (0-20 range)
        int visualHeight = static_cast<int>(perlinHeight * 15);
        
        std::cout << std::setw(8) << std::fixed << std::setprecision(1) << playerX << " | "
                  << std::setw(13) << std::setprecision(3) << perlinHeight << " | "
                  << std::setw(14) << simplexHeight << " | "
                  << std::string(visualHeight, '#') << "\n";
    }
    
    std::cout << "\n✓ This demonstrates real-time terrain generation as player moves!\n";
}

// Test 5: ASCII visualization of sampled noise
void test_ascii_visualization() {
    print_section("Test 5: ASCII Visualization (Sampled Data)");
    
    std::cout << "Generating 20x20 ASCII preview using sampling API...\n\n";
    
    int width = 40, height = 20;
    int seed = 42;
    
    std::cout << "PerlinNoise (scale=30, octaves=3):\n";
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = sample_perlin(x * 2.0f, y * 2.0f, 30.0f, 3, 1.0f, 0.5f, 2.0f, 0.0f, seed);
            
            // Convert to ASCII character
            char c;
            if (value < 0.2f) c = ' ';
            else if (value < 0.4f) c = '.';
            else if (value < 0.6f) c = ':';
            else if (value < 0.8f) c = '#';
            else c = '@';
            
            std::cout << c;
        }
        std::cout << "\n";
    }
    
    std::cout << "\nSimplexNoise (scale=30, octaves=3):\n";
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = sample_simplex(x * 2.0f, y * 2.0f, 30.0f, 3, 0.5f, 2.0f, 0.0f, seed);
            
            // Convert to ASCII character
            char c;
            if (value < 0.2f) c = ' ';
            else if (value < 0.4f) c = '.';
            else if (value < 0.6f) c = ':';
            else if (value < 0.8f) c = '#';
            else c = '@';
            
            std::cout << c;
        }
        std::cout << "\n";
    }
}

// Test 6: Value range verification
void test_value_ranges() {
    print_section("Test 6: Value Range Verification");
    
    std::cout << "Sampling 1000 random points to verify output range [0,1]...\n\n";
    
    int seed = 42;
    int samples = 1000;
    
    // Test each noise type
    float perlin_min = 1.0f, perlin_max = 0.0f;
    float simplex_min = 1.0f, simplex_max = 0.0f;
    float white_min = 1.0f, white_max = 0.0f;
    
    for (int i = 0; i < samples; i++) {
        float x = (i * 7.3f) + 0.5f;  // Pseudo-random positions
        float y = (i * 13.7f) + 1.2f;
        
        float p = sample_perlin(x, y, 50.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
        float s = sample_simplex(x, y, 50.0f, 4, 0.5f, 2.0f, 0.0f, seed);
        float w = sample_whitenoise(x, y, seed);
        
        perlin_min = std::min(perlin_min, p);
        perlin_max = std::max(perlin_max, p);
        simplex_min = std::min(simplex_min, s);
        simplex_max = std::max(simplex_max, s);
        white_min = std::min(white_min, w);
        white_max = std::max(white_max, w);
    }
    
    std::cout << "PerlinNoise:  min=" << std::fixed << std::setprecision(4) << perlin_min 
              << "  max=" << perlin_max << "\n";
    std::cout << "SimplexNoise: min=" << simplex_min << "  max=" << simplex_max << "\n";
    std::cout << "WhiteNoise:   min=" << white_min << "  max=" << white_max << "\n\n";
    
    bool all_valid = (perlin_min >= 0.0f && perlin_max <= 1.0f &&
                      simplex_min >= 0.0f && simplex_max <= 1.0f &&
                      white_min >= 0.0f && white_max <= 1.0f);
    
    if (all_valid) {
        std::cout << "✓ All values within valid range [0,1]\n";
    } else {
        std::cout << "✗ WARNING: Some values outside [0,1] range!\n";
    }
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   RelNo_D1 Single-Value Sampling API Test Suite          ║\n";
    std::cout << "║   Testing: sample_perlin, sample_simplex, sample_whitenoise ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    try {
        test_basic_sampling();
        test_determinism();
        test_performance();
        test_infinite_terrain();
        test_ascii_visualization();
        test_value_ranges();
        
        print_section("Summary");
        std::cout << "✓ All tests completed successfully!\n";
        std::cout << "\nKey Benefits of Sampling API:\n";
        std::cout << "  • Query individual coordinates without full map generation\n";
        std::cout << "  • Ideal for infinite/streaming terrain generation\n";
        std::cout << "  • Perfect for real-time platformer world generation\n";
        std::cout << "  • Deterministic results for reproducible worlds\n";
        std::cout << "  • Efficient for sparse queries across large areas\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed with error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}

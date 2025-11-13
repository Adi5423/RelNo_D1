// CaveNoise.cpp
// -------------
// Implementation of cave/boolean noise generator

#include "CaveNoise.hpp"
#include "stb_image_write.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <iostream>

namespace Noise {

// Forward declare Perlin functions we need
float sample_perlin(float x, float y, float scale, int octaves,
                   float amplitude, float persistence, float lacunarity,
                   float base, int seed);
                   
std::vector<std::vector<float>> generate_perlin_map(
    int width, int height, float scale, int octaves,
    float amplitude, float persistence, float lacunarity,
    float base, int seed);


// ============================================================================
// Preset Configurations
// ============================================================================

CaveParams CaveParams::preset_open_caverns() {
    CaveParams p;
    p.scale = 40.0f;
    p.octaves = 2;
    p.threshold = 0.45f;
    p.smoothingIterations = 4;
    p.birthLimit = 4;
    p.deathLimit = 3;
    p.minRegionSize = 100;
    return p;
}

CaveParams CaveParams::preset_tight_tunnels() {
    CaveParams p;
    p.scale = 20.0f;
    p.octaves = 4;
    p.threshold = 0.55f;
    p.smoothingIterations = 2;
    p.birthLimit = 5;
    p.deathLimit = 2;
    p.minRegionSize = 30;
    return p;
}

CaveParams CaveParams::preset_swiss_cheese() {
    CaveParams p;
    p.scale = 15.0f;
    p.octaves = 3;
    p.threshold = 0.52f;
    p.smoothingIterations = 1;
    p.birthLimit = 4;
    p.deathLimit = 4;
    p.minRegionSize = 20;
    return p;
}

CaveParams CaveParams::preset_vertical_shafts() {
    CaveParams p;
    p.scale = 25.0f;
    p.octaves = 3;
    p.persistence = 0.7f;
    p.lacunarity = 1.5f;  // Lower lacunarity for vertical emphasis
    p.threshold = 0.48f;
    p.smoothingIterations = 3;
    p.minRegionSize = 60;
    return p;
}

CaveParams CaveParams::preset_organic_caves() {
    CaveParams p;
    p.scale = 35.0f;
    p.octaves = 4;
    p.persistence = 0.55f;
    p.threshold = 0.5f;
    p.smoothingIterations = 5;
    p.birthLimit = 4;
    p.deathLimit = 3;
    p.minRegionSize = 80;
    return p;
}

// ============================================================================
// Core Sampling
// ============================================================================

float sample_cave_density(float x, float y, const CaveParams& params) {
    // Use Perlin noise for smooth cave generation
    float density = sample_perlin(x, y, params.scale, params.octaves,
                                   1.0f, params.persistence, params.lacunarity,
                                   0.0f, params.seed);
    return density;
}

bool sample_cave(float x, float y, const CaveParams& params) {
    float density = sample_cave_density(x, y, params);
    
    if (params.invertThreshold) {
        return density < params.threshold;  // Air where density is low
    } else {
        return density > params.threshold;  // Solid where density is high
    }
}

// ============================================================================
// Cellular Automata
// ============================================================================

int count_solid_neighbors(const std::vector<std::vector<bool>>& cave, int x, int y, int range) {
    int count = 0;
    int height = static_cast<int>(cave.size());
    int width = static_cast<int>(cave[0].size());
    
    for (int dy = -range; dy <= range; dy++) {
        for (int dx = -range; dx <= range; dx++) {
            if (dx == 0 && dy == 0) continue;  // Skip center
            
            int nx = x + dx;
            int ny = y + dy;
            
            // Treat out-of-bounds as solid (encourages cave edges)
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                count++;
            } else if (cave[ny][nx]) {
                count++;
            }
        }
    }
    
    return count;
}

void smooth_cave_cellular_automata(
    std::vector<std::vector<bool>>& cave,
    int iterations,
    int birthLimit,
    int deathLimit
) {
    int height = static_cast<int>(cave.size());
    int width = static_cast<int>(cave[0].size());
    
    for (int iter = 0; iter < iterations; iter++) {
        std::vector<std::vector<bool>> newCave = cave;
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int neighbors = count_solid_neighbors(cave, x, y);
                
                if (cave[y][x]) {
                    // Currently solid - check death limit
                    newCave[y][x] = (neighbors >= deathLimit);
                } else {
                    // Currently air - check birth limit
                    newCave[y][x] = (neighbors >= birthLimit);
                }
            }
        }
        
        cave = newCave;
    }
}

// ============================================================================
// Region Analysis (Flood Fill)
// ============================================================================

std::vector<std::vector<int>> find_regions(
    const std::vector<std::vector<bool>>& cave,
    bool findSolid
) {
    int height = static_cast<int>(cave.size());
    int width = static_cast<int>(cave[0].size());
    
    std::vector<std::vector<int>> regions(height, std::vector<int>(width, -1));
    int regionId = 0;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Skip if already visited or wrong type
            if (regions[y][x] != -1 || cave[y][x] != findSolid) {
                continue;
            }
            
            // Flood fill this region
            std::queue<std::pair<int, int>> queue;
            queue.push({x, y});
            regions[y][x] = regionId;
            
            while (!queue.empty()) {
                auto [cx, cy] = queue.front();
                queue.pop();
                
                // Check 4-connected neighbors
                const int dx[] = {0, 1, 0, -1};
                const int dy[] = {-1, 0, 1, 0};
                
                for (int i = 0; i < 4; i++) {
                    int nx = cx + dx[i];
                    int ny = cy + dy[i];
                    
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                        regions[ny][nx] == -1 && cave[ny][nx] == findSolid) {
                        regions[ny][nx] = regionId;
                        queue.push({nx, ny});
                    }
                }
            }
            
            regionId++;
        }
    }
    
    return regions;
}

void remove_small_regions(
    std::vector<std::vector<bool>>& cave,
    int minSize,
    bool removeAir
) {
    int height = static_cast<int>(cave.size());
    int width = static_cast<int>(cave[0].size());
    
    // Find all regions of the target type
    auto regions = find_regions(cave, !removeAir);  // Find solid if removing air pockets
    
    // Count region sizes
    std::map<int, int> regionSizes;
    for (const auto& row : regions) {
        for (int regionId : row) {
            if (regionId != -1) {
                regionSizes[regionId]++;
            }
        }
    }
    
    // Find largest region
    int largestRegion = -1;
    int largestSize = 0;
    for (const auto& [regionId, size] : regionSizes) {
        if (size > largestSize) {
            largestSize = size;
            largestRegion = regionId;
        }
    }
    
    // Remove small regions
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int regionId = regions[y][x];
            if (regionId != -1 && regionId != largestRegion) {
                if (regionSizes[regionId] < minSize) {
                    // Fill small region
                    cave[y][x] = removeAir ? true : false;
                }
            }
        }
    }
}

// ============================================================================
// Utility Conversions
// ============================================================================

std::vector<std::vector<float>> bool_to_float_map(
    const std::vector<std::vector<bool>>& boolMap
) {
    int height = static_cast<int>(boolMap.size());
    int width = static_cast<int>(boolMap[0].size());
    
    std::vector<std::vector<float>> floatMap(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            floatMap[y][x] = boolMap[y][x] ? 1.0f : 0.0f;
        }
    }
    
    return floatMap;
}

std::vector<std::vector<bool>> float_to_bool_map(
    const std::vector<std::vector<float>>& floatMap,
    float threshold,
    bool invert
) {
    int height = static_cast<int>(floatMap.size());
    int width = static_cast<int>(floatMap[0].size());
    
    std::vector<std::vector<bool>> boolMap(height, std::vector<bool>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool solid = floatMap[y][x] > threshold;
            boolMap[y][x] = invert ? !solid : solid;
        }
    }
    
    return boolMap;
}

// ============================================================================
// Core Generation Functions
// ============================================================================

std::vector<std::vector<float>> generate_cave_density(
    int width, int height,
    const CaveParams& params
) {
    // Generate base noise density map
    auto density = generate_perlin_map(width, height, params.scale, params.octaves,
                                       1.0f, params.persistence, params.lacunarity,
                                       0.0f, params.seed);
    return density;
}

std::vector<std::vector<bool>> generate_cave_boolmap(
    int width, int height,
    const CaveParams& params
) {
    // Step 1: Generate density map
    auto density = generate_cave_density(width, height, params);
    
    // Step 2: Convert to boolean using threshold
    auto cave = float_to_bool_map(density, params.threshold, params.invertThreshold);
    
    // Step 3: Apply cellular automata smoothing
    if (params.smoothingIterations > 0) {
        smooth_cave_cellular_automata(cave, params.smoothingIterations,
                                      params.birthLimit, params.deathLimit);
    }
    
    // Step 4: Remove small regions
    if (params.removeSmallRegions) {
        remove_small_regions(cave, params.minRegionSize, false);  // Remove small caves
        remove_small_regions(cave, params.minRegionSize, true);   // Fill small air pockets
    }
    
    return cave;
}

// ============================================================================
// Chunk Generation
// ============================================================================

std::vector<std::vector<bool>> generate_cave_chunk(
    int chunkX, int chunkY, int chunkSize,
    const CaveParams& params
) {
    std::vector<std::vector<bool>> chunk(chunkSize, std::vector<bool>(chunkSize));
    
    float worldOffsetX = static_cast<float>(chunkX * chunkSize);
    float worldOffsetY = static_cast<float>(chunkY * chunkSize);
    
    // Sample cave at each position in chunk
    for (int y = 0; y < chunkSize; y++) {
        for (int x = 0; x < chunkSize; x++) {
            float worldX = worldOffsetX + x;
            float worldY = worldOffsetY + y;
            
            chunk[y][x] = sample_cave(worldX, worldY, params);
        }
    }
    
    // Note: Cellular automata smoothing is not applied to individual chunks
    // as it requires neighbor information. Apply smoothing to full maps only.
    
    return chunk;
}

// ============================================================================
// High-Level API
// ============================================================================

std::vector<std::vector<bool>> create_cave(
    int width, int height,
    const CaveParams& params,
    CaveOutputMode mode,
    const std::string& filename,
    const std::string& outputDir
) {
    // Generate cave
    auto cave = generate_cave_boolmap(width, height, params);
    
    // Handle output
    if (mode == CaveOutputMode::Image) {
        // Convert to float map for image export
        auto floatMap = bool_to_float_map(cave);
        
        // Convert to grayscale image
        std::vector<unsigned char> imageData(width * height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                imageData[y * width + x] = static_cast<unsigned char>(floatMap[y][x] * 255);
            }
        }
        
        // Save image
        std::string fullPath = outputDir + "/" + filename;
        
        // Detect format from extension
        std::string ext = filename.substr(filename.find_last_of('.') + 1);
        bool success = false;
        
        if (ext == "png") {
            success = stbi_write_png(fullPath.c_str(), width, height, 1, imageData.data(), width);
        } else if (ext == "jpg" || ext == "jpeg") {
            success = stbi_write_jpg(fullPath.c_str(), width, height, 1, imageData.data(), 90);
        }
        
        if (success) {
            std::cout << "Cave saved to: " << fullPath << "\n";
        }
    }
    
    return cave;
}

} // namespace Noise

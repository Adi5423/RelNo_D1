// TilemapExport.cpp
// -----------------
// Implementation of tilemap export utilities

#include "TilemapExport.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>

// Platform-specific directory creation
#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #define MKDIR(path) mkdir(path, 0755)
#endif

namespace Noise {

// ============================================================================
// Utility Functions
// ============================================================================

bool ensure_directory_exists(const std::string& path) {
    MKDIR(path.c_str());
    return true;  // Directory created or already exists
}

int get_tile_for_height(float height, const std::map<float, int>& heightToTile) {
    // Find the appropriate tile ID for this height
    int tileId = 0;
    
    for (const auto& [threshold, id] : heightToTile) {
        if (height >= threshold) {
            tileId = id;
        } else {
            break;
        }
    }
    
    return tileId;
}

void print_tilemap_stats(const std::vector<std::vector<int>>& tilemap) {
    if (tilemap.empty() || tilemap[0].empty()) return;
    
    int height = static_cast<int>(tilemap.size());
    int width = static_cast<int>(tilemap[0].size());
    
    // Count tile types
    std::map<int, int> tileCounts;
    for (const auto& row : tilemap) {
        for (int tile : row) {
            tileCounts[tile]++;
        }
    }
    
    std::cout << "Tilemap Statistics:\n";
    std::cout << "  Dimensions: " << width << "x" << height << "\n";
    std::cout << "  Total tiles: " << (width * height) << "\n";
    std::cout << "  Unique tile types: " << tileCounts.size() << "\n";
    std::cout << "  Tile distribution:\n";
    
    for (const auto& [tileId, count] : tileCounts) {
        float percent = (count * 100.0f) / (width * height);
        std::cout << "    Tile " << tileId << ": " << count << " (" 
                  << std::fixed << std::setprecision(1) << percent << "%)\n";
    }
}

void print_tilemap_ascii(
    const std::vector<std::vector<int>>& tilemap,
    int maxWidth,
    int maxHeight
) {
    if (tilemap.empty() || tilemap[0].empty()) return;
    
    int height = std::min(static_cast<int>(tilemap.size()), maxHeight);
    int width = std::min(static_cast<int>(tilemap[0].size()), maxWidth);
    
    std::cout << "\nTilemap Preview (first " << width << "x" << height << "):\n";
    
    // Define ASCII characters for different tile types
    const char* tileChars = " .,:;+=*#@";
    int numChars = 10;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int tileId = tilemap[y][x];
            char c = tileChars[std::min(tileId, numChars - 1)];
            std::cout << c;
        }
        std::cout << "\n";
    }
}

// ============================================================================
// Core Conversion Functions
// ============================================================================

std::vector<std::vector<int>> noise_to_tilemap(
    const std::vector<std::vector<float>>& noiseMap,
    const TilemapConfig& config
) {
    int height = static_cast<int>(noiseMap.size());
    int width = static_cast<int>(noiseMap[0].size());
    
    std::vector<std::vector<int>> tilemap(height, std::vector<int>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            tilemap[y][x] = get_tile_for_height(noiseMap[y][x], config.heightToTile);
        }
    }
    
    return tilemap;
}

std::vector<std::vector<int>> cave_to_tilemap(
    const std::vector<std::vector<bool>>& caveMap,
    const TilemapConfig& config
) {
    int height = static_cast<int>(caveMap.size());
    int width = static_cast<int>(caveMap[0].size());
    
    std::vector<std::vector<int>> tilemap(height, std::vector<int>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            tilemap[y][x] = caveMap[y][x] ? config.solidTileId : config.airTileId;
        }
    }
    
    return tilemap;
}

std::vector<std::vector<int>> terrain_to_tilemap(
    const std::vector<float>& terrainHeights,
    int mapHeight,
    const TilemapConfig& config
) {
    int width = static_cast<int>(terrainHeights.size());
    std::vector<std::vector<int>> tilemap(mapHeight, std::vector<int>(width, config.airTileId));
    
    for (int x = 0; x < width; x++) {
        // Convert normalized height [0,1] to pixel coordinates
        int groundY = static_cast<int>(terrainHeights[x] * mapHeight);
        groundY = std::clamp(groundY, 0, mapHeight - 1);
        
        // Fill from ground down to bottom
        for (int y = groundY; y < mapHeight; y++) {
            tilemap[y][x] = config.solidTileId;
        }
    }
    
    return tilemap;
}

std::vector<std::vector<int>> custom_to_tilemap(
    const std::vector<std::vector<float>>& noiseMap,
    std::function<int(float, int, int)> converter
) {
    int height = static_cast<int>(noiseMap.size());
    int width = static_cast<int>(noiseMap[0].size());
    
    std::vector<std::vector<int>> tilemap(height, std::vector<int>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            tilemap[y][x] = converter(noiseMap[y][x], x, y);
        }
    }
    
    return tilemap;
}

// ============================================================================
// Auto-Tiling / Bitmasking
// ============================================================================

int calculate_bitmask(
    const std::vector<std::vector<int>>& tilemap,
    int x, int y,
    int solidTileId,
    bool include8Direction
) {
    int height = static_cast<int>(tilemap.size());
    int width = static_cast<int>(tilemap[0].size());
    
    auto isSolid = [&](int nx, int ny) {
        if (nx < 0 || nx >= width || ny < 0 || ny >= height) return false;
        return tilemap[ny][nx] == solidTileId;
    };
    
    int bitmask = 0;
    
    // 4-directional (NESW)
    if (isSolid(x, y - 1)) bitmask |= 1;   // North
    if (isSolid(x + 1, y)) bitmask |= 2;   // East
    if (isSolid(x, y + 1)) bitmask |= 4;   // South
    if (isSolid(x - 1, y)) bitmask |= 8;   // West
    
    if (include8Direction) {
        // Add diagonals
        if (isSolid(x + 1, y - 1)) bitmask |= 16;  // NE
        if (isSolid(x + 1, y + 1)) bitmask |= 32;  // SE
        if (isSolid(x - 1, y + 1)) bitmask |= 64;  // SW
        if (isSolid(x - 1, y - 1)) bitmask |= 128; // NW
    }
    
    return bitmask;
}

std::vector<std::vector<int>> apply_autotiling_16(
    const std::vector<std::vector<int>>& tilemap,
    int solidTileId
) {
    int height = static_cast<int>(tilemap.size());
    int width = static_cast<int>(tilemap[0].size());
    
    std::vector<std::vector<int>> autoTiled = tilemap;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (tilemap[y][x] == solidTileId) {
                autoTiled[y][x] = calculate_bitmask(tilemap, x, y, solidTileId, false);
            }
        }
    }
    
    return autoTiled;
}

std::vector<std::vector<int>> apply_autotiling_48(
    const std::vector<std::vector<int>>& tilemap,
    int solidTileId
) {
    int height = static_cast<int>(tilemap.size());
    int width = static_cast<int>(tilemap[0].size());
    
    std::vector<std::vector<int>> autoTiled = tilemap;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (tilemap[y][x] == solidTileId) {
                autoTiled[y][x] = calculate_bitmask(tilemap, x, y, solidTileId, true);
            }
        }
    }
    
    return autoTiled;
}

// ============================================================================
// Export Functions
// ============================================================================

bool export_to_csv(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const std::string& outputDir
) {
    ensure_directory_exists(outputDir);
    std::string fullPath = outputDir + "/" + filename;
    
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fullPath << "\n";
        return false;
    }
    
    for (size_t y = 0; y < tilemap.size(); y++) {
        for (size_t x = 0; x < tilemap[y].size(); x++) {
            file << tilemap[y][x];
            if (x < tilemap[y].size() - 1) file << ",";
        }
        file << "\n";
    }
    
    file.close();
    std::cout << "Tilemap exported to CSV: " << fullPath << "\n";
    return true;
}

bool export_to_json(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const TilemapConfig& config,
    const std::string& outputDir
) {
    ensure_directory_exists(outputDir);
    std::string fullPath = outputDir + "/" + filename;
    
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fullPath << "\n";
        return false;
    }
    
    int height = static_cast<int>(tilemap.size());
    int width = static_cast<int>(tilemap[0].size());
    
    file << "{\n";
    file << "  \"width\": " << width << ",\n";
    file << "  \"height\": " << height << ",\n";
    file << "  \"tileWidth\": " << config.tileWidth << ",\n";
    file << "  \"tileHeight\": " << config.tileHeight << ",\n";
    file << "  \"layerName\": \"" << config.layerName << "\",\n";
    file << "  \"tiles\": [\n";
    
    for (size_t y = 0; y < tilemap.size(); y++) {
        file << "    [";
        for (size_t x = 0; x < tilemap[y].size(); x++) {
            file << tilemap[y][x];
            if (x < tilemap[y].size() - 1) file << ", ";
        }
        file << "]";
        if (y < tilemap.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    std::cout << "Tilemap exported to JSON: " << fullPath << "\n";
    return true;
}

bool export_to_binary(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const std::string& outputDir
) {
    ensure_directory_exists(outputDir);
    std::string fullPath = outputDir + "/" + filename;
    
    std::ofstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fullPath << "\n";
        return false;
    }
    
    int height = static_cast<int>(tilemap.size());
    int width = static_cast<int>(tilemap[0].size());
    
    // Write header: width and height
    file.write(reinterpret_cast<const char*>(&width), sizeof(int));
    file.write(reinterpret_cast<const char*>(&height), sizeof(int));
    
    // Write tile data
    for (const auto& row : tilemap) {
        file.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(int));
    }
    
    file.close();
    std::cout << "Tilemap exported to binary: " << fullPath << "\n";
    return true;
}

bool export_to_unity(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const TilemapConfig& config,
    const std::string& outputDir
) {
    ensure_directory_exists(outputDir);
    std::string fullPath = outputDir + "/" + filename;
    
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fullPath << "\n";
        return false;
    }
    
    int height = static_cast<int>(tilemap.size());
    int width = static_cast<int>(tilemap[0].size());
    
    file << "{\n";
    file << "  \"name\": \"" << config.layerName << "\",\n";
    file << "  \"width\": " << width << ",\n";
    file << "  \"height\": " << height << ",\n";
    file << "  \"tileSize\": {\"x\": " << config.tileWidth << ", \"y\": " << config.tileHeight << "},\n";
    file << "  \"cells\": [\n";
    
    bool first = true;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (tilemap[y][x] != 0) {  // Skip empty tiles
                if (!first) file << ",\n";
                file << "    {\"x\": " << x << ", \"y\": " << y << ", \"tile\": " << tilemap[y][x] << "}";
                first = false;
            }
        }
    }
    
    file << "\n  ]\n";
    file << "}\n";
    
    file.close();
    std::cout << "Tilemap exported to Unity format: " << fullPath << "\n";
    return true;
}

bool export_to_godot(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const TilemapConfig& config,
    const std::string& outputDir
) {
    ensure_directory_exists(outputDir);
    std::string fullPath = outputDir + "/" + filename;
    
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fullPath << "\n";
        return false;
    }
    
    int height = static_cast<int>(tilemap.size());
    int width = static_cast<int>(tilemap[0].size());
    
    // Godot TSCN format
    file << "[gd_scene format=2]\n\n";
    file << "[node name=\"TileMap\" type=\"TileMap\"]\n";
    file << "cell_size = Vector2(" << config.tileWidth << ", " << config.tileHeight << ")\n";
    file << "format = 1\n";
    file << "tile_data = PoolIntArray(";
    
    // Godot uses a flat array format
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            file << " " << x << ", " << y << ", 0, " << tilemap[y][x] << ",";
        }
    }
    
    file << " )\n";
    
    file.close();
    std::cout << "Tilemap exported to Godot format: " << fullPath << "\n";
    return true;
}

bool export_to_tiled(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const TilemapConfig& config,
    const std::string& outputDir
) {
    ensure_directory_exists(outputDir);
    std::string fullPath = outputDir + "/" + filename;
    
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fullPath << "\n";
        return false;
    }
    
    int height = static_cast<int>(tilemap.size());
    int width = static_cast<int>(tilemap[0].size());
    
    // Tiled TMX format (XML)
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<map version=\"1.0\" orientation=\"orthogonal\" ";
    file << "width=\"" << width << "\" height=\"" << height << "\" ";
    file << "tilewidth=\"" << config.tileWidth << "\" tileheight=\"" << config.tileHeight << "\">\n";
    file << "  <layer name=\"" << config.layerName << "\" width=\"" << width << "\" height=\"" << height << "\">\n";
    file << "    <data encoding=\"csv\">\n";
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            file << tilemap[y][x];
            if (y < height - 1 || x < width - 1) file << ",";
        }
        file << "\n";
    }
    
    file << "    </data>\n";
    file << "  </layer>\n";
    file << "</map>\n";
    
    file.close();
    std::cout << "Tilemap exported to Tiled TMX format: " << fullPath << "\n";
    return true;
}

// ============================================================================
// High-Level All-in-One Functions
// ============================================================================

bool create_tilemap_from_noise(
    const std::vector<std::vector<float>>& noiseMap,
    const std::string& filename,
    TilemapFormat format,
    const TilemapConfig& config,
    const std::string& outputDir
) {
    auto tilemap = noise_to_tilemap(noiseMap, config);
    
    if (config.useAutoTiling) {
        tilemap = config.use16Tile ? 
            apply_autotiling_16(tilemap, config.solidTileId) :
            apply_autotiling_48(tilemap, config.solidTileId);
    }
    
    switch (format) {
        case TilemapFormat::CSV:
            return export_to_csv(tilemap, filename, outputDir);
        case TilemapFormat::JSON:
            return export_to_json(tilemap, filename, config, outputDir);
        case TilemapFormat::Binary:
            return export_to_binary(tilemap, filename, outputDir);
        case TilemapFormat::UnityTilemap:
            return export_to_unity(tilemap, filename, config, outputDir);
        case TilemapFormat::GodotTileMap:
            return export_to_godot(tilemap, filename, config, outputDir);
        case TilemapFormat::TiledTMX:
            return export_to_tiled(tilemap, filename, config, outputDir);
        default:
            return false;
    }
}

bool create_tilemap_from_cave(
    const std::vector<std::vector<bool>>& caveMap,
    const std::string& filename,
    TilemapFormat format,
    const TilemapConfig& config,
    const std::string& outputDir
) {
    auto tilemap = cave_to_tilemap(caveMap, config);
    
    if (config.useAutoTiling) {
        tilemap = config.use16Tile ? 
            apply_autotiling_16(tilemap, config.solidTileId) :
            apply_autotiling_48(tilemap, config.solidTileId);
    }
    
    switch (format) {
        case TilemapFormat::CSV:
            return export_to_csv(tilemap, filename, outputDir);
        case TilemapFormat::JSON:
            return export_to_json(tilemap, filename, config, outputDir);
        case TilemapFormat::Binary:
            return export_to_binary(tilemap, filename, outputDir);
        case TilemapFormat::UnityTilemap:
            return export_to_unity(tilemap, filename, config, outputDir);
        case TilemapFormat::GodotTileMap:
            return export_to_godot(tilemap, filename, config, outputDir);
        case TilemapFormat::TiledTMX:
            return export_to_tiled(tilemap, filename, config, outputDir);
        default:
            return false;
    }
}

bool create_tilemap_from_terrain(
    const std::vector<float>& terrainHeights,
    int mapHeight,
    const std::string& filename,
    TilemapFormat format,
    const TilemapConfig& config,
    const std::string& outputDir
) {
    auto tilemap = terrain_to_tilemap(terrainHeights, mapHeight, config);
    
    switch (format) {
        case TilemapFormat::CSV:
            return export_to_csv(tilemap, filename, outputDir);
        case TilemapFormat::JSON:
            return export_to_json(tilemap, filename, config, outputDir);
        case TilemapFormat::Binary:
            return export_to_binary(tilemap, filename, outputDir);
        case TilemapFormat::UnityTilemap:
            return export_to_unity(tilemap, filename, config, outputDir);
        case TilemapFormat::GodotTileMap:
            return export_to_godot(tilemap, filename, config, outputDir);
        case TilemapFormat::TiledTMX:
            return export_to_tiled(tilemap, filename, config, outputDir);
        default:
            return false;
    }
}

} // namespace Noise

// TilemapExport.hpp
// -----------------
// Utilities for converting noise maps to tilemap formats
// Supports CSV, JSON, binary, and game engine exports

#ifndef TILEMAPEXPORT_HPP
#define TILEMAPEXPORT_HPP

#include <vector>
#include <string>
#include <map>
#include <functional>

namespace Noise {

// Tile types for 2D platformers
enum class TileType {
    Air = 0,
    Solid = 1,
    Platform = 2,      // One-way platform
    Hazard = 3,        // Spikes, lava, etc.
    Ladder = 4,
    Water = 5,
    Ice = 6,
    Breakable = 7
};

// Export format options
enum class TilemapFormat {
    CSV,               // Comma-separated values
    JSON,              // JSON array format
    Binary,            // Raw binary data
    UnityTilemap,      // Unity Tilemap format
    GodotTileMap,      // Godot TileMap format
    TiledTMX           // Tiled map editor format
};

// Tilemap configuration
struct TilemapConfig {
    int tileWidth = 16;           // Tile size in pixels
    int tileHeight = 16;
    
    // Height-based tile mapping (for terrain)
    std::map<float, int> heightToTile;
    
    // Boolean-based tile mapping (for caves)
    int solidTileId = 1;
    int airTileId = 0;
    
    // Auto-tiling configuration
    bool useAutoTiling = false;    // Enable bitmasking/autotiling
    bool use16Tile = true;         // Use 16-tile bitmask (vs 48-tile)
    
    // Layer configuration
    std::string layerName = "Ground";
    int layerDepth = 0;
    
    // Constructor with defaults
    TilemapConfig() {
        // Default height thresholds for terrain
        heightToTile[0.0f] = 0;    // Deep water
        heightToTile[0.3f] = 1;    // Shallow water
        heightToTile[0.45f] = 2;   // Sand/beach
        heightToTile[0.55f] = 3;   // Grass
        heightToTile[0.70f] = 4;   // Rock
        heightToTile[0.85f] = 5;   // Mountain
        heightToTile[1.0f] = 6;    // Snow
    }
};

// ============================================================================
// Core Conversion Functions
// ============================================================================

// Convert float noise map to tile indices using height thresholds
std::vector<std::vector<int>> noise_to_tilemap(
    const std::vector<std::vector<float>>& noiseMap,
    const TilemapConfig& config
);

// Convert boolean cave map to tile indices
std::vector<std::vector<int>> cave_to_tilemap(
    const std::vector<std::vector<bool>>& caveMap,
    const TilemapConfig& config
);

// Convert 1D terrain heights to 2D tilemap (fills below terrain line)
std::vector<std::vector<int>> terrain_to_tilemap(
    const std::vector<float>& terrainHeights,
    int mapHeight,
    const TilemapConfig& config
);

// Custom conversion with lambda function
std::vector<std::vector<int>> custom_to_tilemap(
    const std::vector<std::vector<float>>& noiseMap,
    std::function<int(float, int, int)> converter
);

// ============================================================================
// Auto-Tiling / Bitmasking
// ============================================================================

// Apply 16-tile bitmask autotiling (4-directional neighbors)
std::vector<std::vector<int>> apply_autotiling_16(
    const std::vector<std::vector<int>>& tilemap,
    int solidTileId = 1
);

// Apply 48-tile bitmask autotiling (8-directional neighbors + corners)
std::vector<std::vector<int>> apply_autotiling_48(
    const std::vector<std::vector<int>>& tilemap,
    int solidTileId = 1
);

// Calculate bitmask value for a tile (0-15 for 16-tile, 0-255 for 48-tile)
int calculate_bitmask(
    const std::vector<std::vector<int>>& tilemap,
    int x, int y,
    int solidTileId,
    bool include8Direction = false
);

// ============================================================================
// Export Functions
// ============================================================================

// Export tilemap to CSV format
bool export_to_csv(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const std::string& outputDir = "TilemapOutput"
);

// Export tilemap to JSON format
bool export_to_json(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const TilemapConfig& config,
    const std::string& outputDir = "TilemapOutput"
);

// Export tilemap to binary format (raw tile indices)
bool export_to_binary(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const std::string& outputDir = "TilemapOutput"
);

// Export to Unity Tilemap JSON format
bool export_to_unity(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const TilemapConfig& config,
    const std::string& outputDir = "TilemapOutput"
);

// Export to Godot TileMap format (TSCN)
bool export_to_godot(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const TilemapConfig& config,
    const std::string& outputDir = "TilemapOutput"
);

// Export to Tiled TMX format
bool export_to_tiled(
    const std::vector<std::vector<int>>& tilemap,
    const std::string& filename,
    const TilemapConfig& config,
    const std::string& outputDir = "TilemapOutput"
);

// ============================================================================
// High-Level All-in-One Functions
// ============================================================================

// Convert noise map and export in one call
bool create_tilemap_from_noise(
    const std::vector<std::vector<float>>& noiseMap,
    const std::string& filename,
    TilemapFormat format,
    const TilemapConfig& config = TilemapConfig(),
    const std::string& outputDir = "TilemapOutput"
);

// Convert cave map and export in one call
bool create_tilemap_from_cave(
    const std::vector<std::vector<bool>>& caveMap,
    const std::string& filename,
    TilemapFormat format,
    const TilemapConfig& config = TilemapConfig(),
    const std::string& outputDir = "TilemapOutput"
);

// Convert terrain heights and export in one call
bool create_tilemap_from_terrain(
    const std::vector<float>& terrainHeights,
    int mapHeight,
    const std::string& filename,
    TilemapFormat format,
    const TilemapConfig& config = TilemapConfig(),
    const std::string& outputDir = "TilemapOutput"
);

// ============================================================================
// Utility Functions
// ============================================================================

// Get tile ID for a given height value using threshold map
int get_tile_for_height(float height, const std::map<float, int>& heightToTile);

// Create output directory if it doesn't exist
bool ensure_directory_exists(const std::string& path);

// Print tilemap statistics
void print_tilemap_stats(const std::vector<std::vector<int>>& tilemap);

// Visualize tilemap as ASCII (useful for debugging)
void print_tilemap_ascii(
    const std::vector<std::vector<int>>& tilemap,
    int maxWidth = 80,
    int maxHeight = 40
);

} // namespace Noise

#endif // TILEMAPEXPORT_HPP

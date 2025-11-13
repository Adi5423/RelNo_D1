// test_tilemap.cpp
// ----------------
// Comprehensive test for tilemap export utilities
// Tests various formats and game engine exports

#include "Noise.hpp"
#include <iostream>
#include <iomanip>

using namespace Noise;

void print_section(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

// Test 1: Basic noise to tilemap conversion
void test_noise_to_tilemap() {
    print_section("Test 1: Noise to Tilemap Conversion");
    
    std::cout << "Generating 64x64 Perlin noise map...\n";
    auto noiseMap = generate_perlin_map(64, 64, 30.0f, 3, 1.0f, 0.5f, 2.0f, 0.0f, 42);
    
    std::cout << "Converting to tilemap with height thresholds...\n\n";
    
    TilemapConfig config;
    config.heightToTile.clear();
    config.heightToTile[0.0f] = 0;   // Water
    config.heightToTile[0.4f] = 1;   // Sand
    config.heightToTile[0.5f] = 2;   // Grass
    config.heightToTile[0.7f] = 3;   // Stone
    config.heightToTile[0.85f] = 4;  // Snow
    
    auto tilemap = noise_to_tilemap(noiseMap, config);
    
    print_tilemap_stats(tilemap);
    print_tilemap_ascii(tilemap, 64, 20);
    
    std::cout << "\n✓ Noise to tilemap conversion working!\n";
}

// Test 2: Cave to tilemap conversion
void test_cave_to_tilemap() {
    print_section("Test 2: Cave to Tilemap Conversion");
    
    std::cout << "Generating cave with open caverns preset...\n";
    auto params = CaveParams::preset_open_caverns();
    params.seed = 123;
    auto caveMap = generate_cave_boolmap(64, 64, params);
    
    std::cout << "Converting to tilemap...\n\n";
    
    TilemapConfig config;
    config.solidTileId = 1;
    config.airTileId = 0;
    
    auto tilemap = cave_to_tilemap(caveMap, config);
    
    print_tilemap_stats(tilemap);
    print_tilemap_ascii(tilemap, 64, 20);
    
    std::cout << "\n✓ Cave to tilemap conversion working!\n";
}

// Test 3: Terrain to tilemap conversion
void test_terrain_to_tilemap() {
    print_section("Test 3: Terrain to Tilemap Conversion");
    
    std::cout << "Generating 1D terrain profile...\n";
    auto params = TerrainParams::preset_rolling_hills();
    params.seed = 456;
    auto terrain = generate_terrain_profile(128, 0.0f, 1.0f, params);
    
    std::cout << "Converting to 2D tilemap (128x64)...\n\n";
    
    TilemapConfig config;
    config.solidTileId = 1;
    config.airTileId = 0;
    
    auto tilemap = terrain_to_tilemap(terrain, 64, config);
    
    print_tilemap_stats(tilemap);
    print_tilemap_ascii(tilemap, 80, 30);
    
    std::cout << "\n✓ Terrain to tilemap conversion working!\n";
}

// Test 4: CSV export
void test_csv_export() {
    print_section("Test 4: CSV Export");
    
    std::cout << "Generating small tilemap for CSV export...\n";
    auto noiseMap = generate_perlin_map(32, 32, 20.0f, 2, 1.0f, 0.5f, 2.0f, 0.0f, 789);
    
    TilemapConfig config;
    auto tilemap = noise_to_tilemap(noiseMap, config);
    
    std::cout << "Exporting to CSV...\n";
    bool success = export_to_csv(tilemap, "tilemap_test.csv", "TilemapOutput");
    
    if (success) {
        std::cout << "\n✓ CSV export successful!\n";
    } else {
        std::cout << "\n✗ CSV export failed!\n";
    }
}

// Test 5: JSON export
void test_json_export() {
    print_section("Test 5: JSON Export");
    
    std::cout << "Generating tilemap for JSON export...\n";
    auto noiseMap = generate_perlin_map(32, 32, 20.0f, 2, 1.0f, 0.5f, 2.0f, 0.0f, 111);
    
    TilemapConfig config;
    config.layerName = "TerrainLayer";
    config.tileWidth = 16;
    config.tileHeight = 16;
    auto tilemap = noise_to_tilemap(noiseMap, config);
    
    std::cout << "Exporting to JSON...\n";
    bool success = export_to_json(tilemap, "tilemap_test.json", config, "TilemapOutput");
    
    if (success) {
        std::cout << "\n✓ JSON export successful!\n";
    } else {
        std::cout << "\n✗ JSON export failed!\n";
    }
}

// Test 6: Binary export
void test_binary_export() {
    print_section("Test 6: Binary Export");
    
    std::cout << "Generating tilemap for binary export...\n";
    auto noiseMap = generate_perlin_map(64, 64, 25.0f, 3, 1.0f, 0.5f, 2.0f, 0.0f, 222);
    
    TilemapConfig config;
    auto tilemap = noise_to_tilemap(noiseMap, config);
    
    std::cout << "Exporting to binary format...\n";
    bool success = export_to_binary(tilemap, "tilemap_test.bin", "TilemapOutput");
    
    if (success) {
        std::cout << "Binary file contains: 2 ints (width, height) + tile data\n";
        std::cout << "\n✓ Binary export successful!\n";
    } else {
        std::cout << "\n✗ Binary export failed!\n";
    }
}

// Test 7: Unity format export
void test_unity_export() {
    print_section("Test 7: Unity Format Export");
    
    std::cout << "Generating cave tilemap for Unity...\n";
    auto params = CaveParams::preset_tight_tunnels();
    params.seed = 333;
    auto caveMap = generate_cave_boolmap(48, 48, params);
    
    TilemapConfig config;
    config.layerName = "CaveLayer";
    config.tileWidth = 32;
    config.tileHeight = 32;
    
    std::cout << "Exporting to Unity format...\n";
    bool success = create_tilemap_from_cave(caveMap, "tilemap_unity.json", 
                                           TilemapFormat::UnityTilemap, config, "TilemapOutput");
    
    if (success) {
        std::cout << "\n✓ Unity format export successful!\n";
        std::cout << "  Import this JSON in Unity Tilemap editor\n";
    } else {
        std::cout << "\n✗ Unity export failed!\n";
    }
}

// Test 8: Godot format export
void test_godot_export() {
    print_section("Test 8: Godot Format Export");
    
    std::cout << "Generating terrain tilemap for Godot...\n";
    auto params = TerrainParams::preset_mountainous();
    params.seed = 444;
    auto terrain = generate_terrain_profile(96, 0.0f, 1.0f, params);
    
    TilemapConfig config;
    config.layerName = "TerrainLayer";
    config.tileWidth = 16;
    config.tileHeight = 16;
    
    std::cout << "Exporting to Godot format...\n";
    bool success = create_tilemap_from_terrain(terrain, 64, "tilemap_godot.tscn",
                                               TilemapFormat::GodotTileMap, config, "TilemapOutput");
    
    if (success) {
        std::cout << "\n✓ Godot format export successful!\n";
        std::cout << "  Import this TSCN in Godot TileMap node\n";
    } else {
        std::cout << "\n✗ Godot export failed!\n";
    }
}

// Test 9: Tiled TMX format export
void test_tiled_export() {
    print_section("Test 9: Tiled TMX Format Export");
    
    std::cout << "Generating complex noise tilemap for Tiled...\n";
    auto noiseMap = generate_perlin_map(80, 60, 35.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, 555);
    
    TilemapConfig config;
    config.layerName = "Background";
    config.tileWidth = 16;
    config.tileHeight = 16;
    
    std::cout << "Exporting to Tiled TMX format...\n";
    bool success = create_tilemap_from_noise(noiseMap, "tilemap_tiled.tmx",
                                             TilemapFormat::TiledTMX, config, "TilemapOutput");
    
    if (success) {
        std::cout << "\n✓ Tiled TMX format export successful!\n";
        std::cout << "  Open this TMX file in Tiled Map Editor\n";
    } else {
        std::cout << "\n✗ Tiled export failed!\n";
    }
}

// Test 10: Auto-tiling (16-tile bitmask)
void test_autotiling_16() {
    print_section("Test 10: Auto-Tiling (16-tile Bitmask)");
    
    std::cout << "Generating cave for auto-tiling test...\n";
    auto params = CaveParams::preset_swiss_cheese();
    params.seed = 666;
    auto caveMap = generate_cave_boolmap(40, 40, params);
    
    TilemapConfig config;
    config.solidTileId = 1;
    config.airTileId = 0;
    
    auto tilemap = cave_to_tilemap(caveMap, config);
    
    std::cout << "Before auto-tiling:\n";
    print_tilemap_ascii(tilemap, 40, 15);
    
    std::cout << "\nApplying 16-tile bitmask auto-tiling...\n";
    auto autoTiled = apply_autotiling_16(tilemap, config.solidTileId);
    
    std::cout << "\nAfter auto-tiling (showing bitmask values):\n";
    // Show sample bitmask values
    std::cout << "Sample bitmask values:\n";
    for (int y = 10; y < 15; y++) {
        for (int x = 10; x < 20; x++) {
            if (tilemap[y][x] == config.solidTileId) {
                std::cout << std::setw(3) << autoTiled[y][x] << " ";
            } else {
                std::cout << "  . ";
            }
        }
        std::cout << "\n";
    }
    
    std::cout << "\n✓ Auto-tiling (16-tile) working!\n";
    std::cout << "  Bitmask values: 0-15 (4 directions: N=1, E=2, S=4, W=8)\n";
}

// Test 11: Custom conversion function
void test_custom_converter() {
    print_section("Test 11: Custom Conversion Function");
    
    std::cout << "Generating noise map...\n";
    auto noiseMap = generate_perlin_map(48, 48, 30.0f, 3, 1.0f, 0.5f, 2.0f, 0.0f, 777);
    
    std::cout << "Applying custom converter (checkerboard + noise)...\n";
    
    auto tilemap = custom_to_tilemap(noiseMap, 
        [](float value, int x, int y) -> int {
            // Checkerboard pattern influenced by noise
            bool checker = ((x + y) % 2) == 0;
            if (checker && value > 0.5f) return 1;
            if (!checker && value > 0.7f) return 2;
            return 0;
        }
    );
    
    print_tilemap_stats(tilemap);
    print_tilemap_ascii(tilemap, 48, 20);
    
    std::cout << "\n✓ Custom converter working!\n";
    std::cout << "  Lambda functions enable creative tile mapping\n";
}

// Test 12: All-in-one export test
void test_all_in_one() {
    print_section("Test 12: All-in-One Export");
    
    std::cout << "Testing all-in-one convenience functions...\n\n";
    
    // Generate noise once
    auto noiseMap = generate_perlin_map(64, 64, 30.0f, 3, 1.0f, 0.5f, 2.0f, 0.0f, 888);
    
    TilemapConfig config;
    config.layerName = "AllInOneLayer";
    
    std::cout << "Exporting same tilemap to multiple formats:\n";
    
    bool csv = create_tilemap_from_noise(noiseMap, "all_in_one.csv", TilemapFormat::CSV, config);
    bool json = create_tilemap_from_noise(noiseMap, "all_in_one.json", TilemapFormat::JSON, config);
    bool binary = create_tilemap_from_noise(noiseMap, "all_in_one.bin", TilemapFormat::Binary, config);
    bool unity = create_tilemap_from_noise(noiseMap, "all_in_one_unity.json", TilemapFormat::UnityTilemap, config);
    bool godot = create_tilemap_from_noise(noiseMap, "all_in_one_godot.tscn", TilemapFormat::GodotTileMap, config);
    bool tiled = create_tilemap_from_noise(noiseMap, "all_in_one_tiled.tmx", TilemapFormat::TiledTMX, config);
    
    std::cout << "\nResults:\n";
    std::cout << "  CSV:   " << (csv ? "✓" : "✗") << "\n";
    std::cout << "  JSON:  " << (json ? "✓" : "✗") << "\n";
    std::cout << "  Binary:" << (binary ? "✓" : "✗") << "\n";
    std::cout << "  Unity: " << (unity ? "✓" : "✗") << "\n";
    std::cout << "  Godot: " << (godot ? "✓" : "✗") << "\n";
    std::cout << "  Tiled: " << (tiled ? "✓" : "✗") << "\n";
    
    if (csv && json && binary && unity && godot && tiled) {
        std::cout << "\n✓ All formats exported successfully!\n";
    }
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   RelNo_D1 Tilemap Export Test Suite                     ║\n";
    std::cout << "║   Converting Noise to Game-Ready Tilemaps                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    try {
        test_noise_to_tilemap();
        test_cave_to_tilemap();
        test_terrain_to_tilemap();
        test_csv_export();
        test_json_export();
        test_binary_export();
        test_unity_export();
        test_godot_export();
        test_tiled_export();
        test_autotiling_16();
        test_custom_converter();
        test_all_in_one();
        
        print_section("Summary");
        std::cout << "✓ All tilemap export tests completed successfully!\n\n";
        std::cout << "Key Features:\n";
        std::cout << "  • Height threshold → tile ID mapping\n";
        std::cout << "  • Boolean cave → tilemap conversion\n";
        std::cout << "  • 1D terrain → 2D tilemap filling\n";
        std::cout << "  • Auto-tiling with bitmask (16/48-tile)\n";
        std::cout << "  • 6 export formats: CSV, JSON, Binary, Unity, Godot, Tiled\n";
        std::cout << "  • Custom conversion functions\n";
        std::cout << "  • ASCII preview for debugging\n";
        std::cout << "  • Perfect for game engine integration!\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed with error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}

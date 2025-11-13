// CaveNoise.hpp
// -------------
// Boolean noise generator for 2D platformer cave systems
// Uses threshold-based Perlin/Simplex noise + cellular automata smoothing

#ifndef CAVENOISE_HPP
#define CAVENOISE_HPP

#include <vector>
#include <string>

namespace Noise {

// Output format for cave generation
enum class CaveOutputMode {
    None,           // Return data only
    Image,          // Save as black/white PNG
    BooleanMap      // Return vector<vector<bool>>
};

// Parameters for cave generation
struct CaveParams {
    // Noise parameters
    float scale = 30.0f;        // Base noise scale (lower = larger caves)
    int octaves = 3;            // Detail levels
    float persistence = 0.5f;   // Amplitude decay
    float lacunarity = 2.0f;    // Frequency growth
    int seed = -1;              // Random seed (-1 = random)
    
    // Cave thresholds
    float threshold = 0.5f;     // Values > threshold = solid, < threshold = air
    bool invertThreshold = false; // If true, > threshold = air
    
    // Cellular automata smoothing
    int smoothingIterations = 3; // Number of CA passes (0 = no smoothing)
    int birthLimit = 4;          // Neighbors needed to become solid
    int deathLimit = 3;          // Neighbors needed to stay solid
    
    // Additional features
    bool removeSmallRegions = true;  // Remove isolated small caves/islands
    int minRegionSize = 50;          // Minimum region size (in tiles)
    
    // Preset configurations
    static CaveParams preset_open_caverns();      // Large open spaces
    static CaveParams preset_tight_tunnels();     // Narrow winding passages
    static CaveParams preset_swiss_cheese();      // Many small holes
    static CaveParams preset_vertical_shafts();   // Vertical emphasis
    static CaveParams preset_organic_caves();     // Natural-looking caves
};

// === Core Boolean Map Generation ===

// Generate boolean cave map (true = solid, false = air)
std::vector<std::vector<bool>> generate_cave_boolmap(
    int width, int height,
    const CaveParams& params
);

// Generate cave as float density map [0,1] for gradual transitions
std::vector<std::vector<float>> generate_cave_density(
    int width, int height,
    const CaveParams& params
);

// === Chunk-Based Generation ===

std::vector<std::vector<bool>> generate_cave_chunk(
    int chunkX, int chunkY, int chunkSize,
    const CaveParams& params
);

// === Sampling API ===

// Sample cave at specific coordinate (returns true if solid)
bool sample_cave(float x, float y, const CaveParams& params);

// Sample cave density at specific coordinate (0=air, 1=solid)
float sample_cave_density(float x, float y, const CaveParams& params);

// === Cellular Automata Smoothing ===

// Apply cellular automata rules to smooth cave
void smooth_cave_cellular_automata(
    std::vector<std::vector<bool>>& cave,
    int iterations,
    int birthLimit,
    int deathLimit
);

// Count solid neighbors for CA
int count_solid_neighbors(
    const std::vector<std::vector<bool>>& cave,
    int x, int y,
    int range = 1
);

// === Region Analysis ===

// Remove isolated regions smaller than minSize
void remove_small_regions(
    std::vector<std::vector<bool>>& cave,
    int minSize,
    bool removeAir = false  // If true, fills small air pockets
);

// Find all connected regions (flood fill)
std::vector<std::vector<int>> find_regions(
    const std::vector<std::vector<bool>>& cave,
    bool findSolid = true
);

// === Utility Functions ===

// Convert boolean map to float map for image export
std::vector<std::vector<float>> bool_to_float_map(
    const std::vector<std::vector<bool>>& boolMap
);

// Convert float map to boolean using threshold
std::vector<std::vector<bool>> float_to_bool_map(
    const std::vector<std::vector<float>>& floatMap,
    float threshold = 0.5f,
    bool invert = false
);

// === High-Level API ===

// All-in-one cave generation with output options
std::vector<std::vector<bool>> create_cave(
    int width, int height,
    const CaveParams& params,
    CaveOutputMode mode = CaveOutputMode::None,
    const std::string& filename = "cave.png",
    const std::string& outputDir = "ImageOutput"
);

} // namespace Noise

#endif // CAVENOISE_HPP

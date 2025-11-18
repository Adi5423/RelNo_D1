// src/main.cpp
// -------------------------------------------------------------
// RelNoTerrain3D
// Uses RelNo_D1 to generate a 2D heightmap and convert it into
// a 3D terrain mesh stored as an OBJ file.
// -------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <filesystem>

// Include your noise library header
#include "Noise.hpp"

namespace fs = std::filesystem;

// Type alias for readability: heightmap[y][x]
using HeightMap = std::vector<std::vector<float>>;

// -------------------------------------------------------------
// 1. Generate a heightmap using RelNo_D1 Perlin noise
// -------------------------------------------------------------
HeightMap generateHeightMapPerlin(
    int width,
    int height,
    float scale,
    int octaves,
    float frequency,
    float persistence,
    float lacunarity,
    float base,
    int seed
)
{
    using namespace Noise;

    // Your create_perlinnoise() wrapper is expected to:
    //  - return a 2D std::vector<std::vector<float>>
    //  - values normalized to [0, 1]
    //
    // If your signature is slightly different, adjust here.
    //
    // Example from your docs:
    // create_perlinnoise(width, height, scale, octaves,
    //                    frequency, persistence, lacunarity,
    //                    base, seed, OutputMode::None, "");
    //
    // Here we call it in "no image" mode so we only get the data.
    HeightMap map = create_perlinnoise(
        width, height,
        scale,
        octaves,
        frequency,
        persistence,
        lacunarity,
        base,
        seed,
        OutputMode::None,       // No PNG, just use the data
        ""                      // Unused filename
    );

    return map;
}

// -------------------------------------------------------------
// 2. Write heightmap as a 3D terrain mesh in Wavefront OBJ format
// -------------------------------------------------------------
//
// We create a regular grid:
//
//   v (x, height, y)
//
// where:
//   x in [0, width-1]
//   y in [0, height-1]
//   height = heightMap[y][x] * heightScale
//
// Then we create faces as two triangles per cell:
//
//   (x, y) ---- (x+1, y)
//     |        / |
//     |      /   |
//   (x, y+1) -- (x+1, y+1)
//
// OBJ is 1-based indexing, so we offset indices by +1.
//
void writeHeightMapToOBJ(
    const HeightMap& heightMap,
    const std::string& objPath,
    float heightScale,
    float cellSize
)
{
    const int height = static_cast<int>(heightMap.size());
    if (height == 0)
        throw std::runtime_error("HeightMap has zero height.");

    const int width = static_cast<int>(heightMap[0].size());
    if (width == 0)
        throw std::runtime_error("HeightMap has zero width.");

    // Ensure all rows have same width
    for (int y = 1; y < height; ++y)
    {
        if (static_cast<int>(heightMap[y].size()) != width)
            throw std::runtime_error("HeightMap rows have inconsistent widths.");
    }

    std::ofstream out(objPath);
    if (!out)
        throw std::runtime_error("Failed to open OBJ file for writing: " + objPath);

    // Optional: center the terrain around the origin in XZ plane.
    // This is purely aesthetic for Blender/engines.
    const float halfWidth = (width - 1) * cellSize * 0.5f;
    const float halfHeight = (height - 1) * cellSize * 0.5f;

    // -----------------------
    // Write vertices
    // -----------------------
    //
    // OBJ vertex format:
    //   v x y z
    //
    // We'll use:
    //   x -> worldX
    //   y -> height
    //   z -> worldZ
    //
    // grid coordinates:
    //   ix in [0, width-1]
    //   iy in [0, height-1]
    //
    for (int iy = 0; iy < height; ++iy)
    {
        for (int ix = 0; ix < width; ++ix)
        {
            const float noiseValue = heightMap[iy][ix]; // [0, 1]
            const float worldX = ix * cellSize - halfWidth;
            const float worldZ = iy * cellSize - halfHeight;
            const float worldY = noiseValue * heightScale;

            out << "v " << worldX << " "
                << worldY << " "
                << worldZ << "\n";
        }
    }

    // -----------------------
    // Write faces (triangles)
    // -----------------------
    //
    // OBJ face format:
    //   f i1 i2 i3
    //
    // where i1, i2, i3 are 1-based indices into the vertex list.
    //
    // Vertex index in our grid:
    //
    //   idx(ix, iy) = iy * width + ix
    //
    auto vertexIndex = [width](int x, int y) -> int {
        return y * width + x + 1; // +1 -> OBJ 1-based indexing
        };

    for (int iy = 0; iy < height - 1; ++iy)
    {
        for (int ix = 0; ix < width - 1; ++ix)
        {
            // Quad corners:
            //
            //  (ix,   iy)     -> top-left
            //  (ix+1, iy)     -> top-right
            //  (ix,   iy+1)   -> bottom-left
            //  (ix+1, iy+1)   -> bottom-right
            //
            int v00 = vertexIndex(ix, iy);
            int v10 = vertexIndex(ix + 1, iy);
            int v01 = vertexIndex(ix, iy + 1);
            int v11 = vertexIndex(ix + 1, iy + 1);

            // Two triangles:
            //
            //  Triangle 1: v00, v01, v10
            //  Triangle 2: v10, v01, v11
            //
            out << "f " << v00 << " " << v01 << " " << v10 << "\n";
            out << "f " << v10 << " " << v01 << " " << v11 << "\n";
        }
    }

    out.close();
    std::cout << "OBJ mesh written to: " << objPath << "\n";
}

// -------------------------------------------------------------
// 3. Main – glue everything
// -------------------------------------------------------------
int main()
{
    try
    {
        // 3.1. Define terrain / noise parameters
        const int width = 512;    // number of vertices in X
        const int height = 512;    // number of vertices in Z
        const float scale = 40.0f;  // "zoom" factor of noise
        const int octaves = 2;
        const float frequency = 0.7f;
        const float persistence = 0.5f;
        const float lacunarity = 1.7f;
        const float base = 0.0f;
        const int seed = 23;

        // 3.2. Terrain scaling in world units
        const float heightScale = 55.0f;   // max height of terrain
        const float cellSize = 1.3f;    // spacing between grid points

        // 3.3. Output path (relative to project root)
        const fs::path outputDir = fs::path("output");
        const fs::path outputFile = outputDir / "terrain_perlin.obj";

        if (!fs::exists(outputDir))
        {
            fs::create_directories(outputDir);
        }

        std::cout << "Generating heightmap using RelNo_D1 (Perlin)...\n";

        // 3.4. Generate Perlin noise heightmap
        HeightMap heightMap = generateHeightMapPerlin(
            width,
            height,
            scale,
            octaves,
            frequency,
            persistence,
            lacunarity,
            base,
            seed
        );

        std::cout << "Heightmap generated. Exporting to OBJ...\n";

        // 3.5. Convert to 3D mesh and save
        writeHeightMapToOBJ(heightMap, outputFile.string(), heightScale, cellSize);

        std::cout << "Done.\n";
        std::cout << "Open '" << outputFile.string()
            << "' in Blender / any 3D tool to view the terrain.\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

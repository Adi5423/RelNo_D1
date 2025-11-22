#pragma once
#include <vector>
#include <string>
#include <cstddef>
#include "Noise.hpp"

namespace Noise {

    enum class OutputMode; // forward declare (Noise.hpp provides def when included in compilation units)

    // aligned buffer RAII wrapper (opaque here, defined in cpp)
    struct AlignedBuffer {
        float* data = nullptr;
        std::size_t size = 0; // number of floats
        AlignedBuffer() = default;
        AlignedBuffer(std::size_t n);
        ~AlignedBuffer();
        AlignedBuffer(const AlignedBuffer&) = delete;
        AlignedBuffer& operator=(const AlignedBuffer&) = delete;
        AlignedBuffer(AlignedBuffer&& other) noexcept;
        AlignedBuffer& operator=(AlignedBuffer&& other) noexcept;
        float* get() noexcept { return data; }
    };

    // PinkNoise generator class (lightweight)
    class PinkNoise {
    public:
        explicit PinkNoise(int seed = -1);
        ~PinkNoise() = default;

        // low-level method: build single layer white noise into target (contiguously)
        // width*height sized target
        void generate_white_layer(float* target, int width, int height, int octaveSeed) const;

        // build integral image (summed-area table) from `src` (size w*h) into `dst` (size (w+1)*(h+1))
        // `dst` layout: (h+1) rows of (w+1) floats; row major
        static void build_integral(const float* src, float* dst, int width, int height);

        // compute box-averages using integral image and write into `out` (contiguous w*h)
        // box defined by integer blockSize (block width/height)
        // top-left anchored boxes — consistent with previous implementation (blocks starting at multiples)
        static void box_average_from_integral(const float* integral, float* out, int width, int height, int blockSize);

    private:
        int seed_;
    };

    // High-level generator
    std::vector<std::vector<float>> generate_pink_map(
        int width,
        int height,
        int octaves = 6,
        float alpha = 1.0f,
        int sampleRate = 44100,
        float amplitude = 1.0f,
        int seed = -1
    );

    void save_pink_image(
        const std::vector<std::vector<float>>& noise,
        const std::string& filename = "pink_noise.png",
        const std::string& outputDir = ""
    );

    std::vector<std::vector<float>> create_pinknoise(
        int width,
        int height,
        int octaves = 6,
        float alpha = 1.0f,
        int sampleRate = 44100,
        float amplitude = 1.0f,
        int seed = -1,
        OutputMode mode = OutputMode::Image,
        const std::string& filename = "pink_noise.png",
        const std::string& outputDir = ""
    );

} // namespace Noise

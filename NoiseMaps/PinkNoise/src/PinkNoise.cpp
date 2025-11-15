// PinkNoise.cpp
#include "PinkNoise.hpp"
#include "Noise.hpp" // for OutputMode definition
#include "stb_image_write.h"

#include <random>
#include <vector>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <thread>
#include <atomic>
#include <cassert>
#include <cstring>
#include <cstdint> // for std::uintptr_t

#if defined(_MSC_VER)
#include <malloc.h> // _aligned_malloc / _aligned_free
#else
#include <stdlib.h> // posix_memalign, free
#endif

#if defined(__AVX2__)
#include <immintrin.h>
#endif

namespace Noise {

    // -----------------------------
    // AlignedBuffer implementation
    // -----------------------------
    AlignedBuffer::AlignedBuffer(std::size_t n) : data(nullptr), size(n) {
        if (n == 0) return;

        std::size_t bytes = n * sizeof(float);

#if defined(_MSC_VER)
        // Windows (MSVC): use _aligned_malloc / _aligned_free
        data = static_cast<float*>(_aligned_malloc(bytes, 64));
        if (!data) {
            throw std::bad_alloc();
        }
#else
        // Portable manual alignment for all other compilers (MinGW, Linux, macOS, etc.)
        const std::size_t alignment = 64;

        // We allocate extra space to:
        //  - guarantee we can align to `alignment`
        //  - store the original pointer just before the aligned block
        std::size_t total = bytes + alignment - 1 + sizeof(void*);
        void* raw = std::malloc(total);
        if (!raw) {
            throw std::bad_alloc();
        }

        // Find an aligned address inside the allocated block
        std::uintptr_t start = reinterpret_cast<std::uintptr_t>(raw) + sizeof(void*);
        std::uintptr_t aligned = (start + alignment - 1) & ~(alignment - 1);
        void* alignedPtr = reinterpret_cast<void*>(aligned);

        // Store the original pointer immediately before the aligned block
        reinterpret_cast<void**>(alignedPtr)[-1] = raw;

        data = static_cast<float*>(alignedPtr);
#endif

        // zero initialize the usable bytes (not the padding)
        std::memset(data, 0, bytes);
    }

    AlignedBuffer::~AlignedBuffer() {
#if defined(_MSC_VER)
        if (data) {
            _aligned_free(data);
        }
#else
        if (data) {
            // Recover the original pointer we stashed just before `data`
            void* raw = reinterpret_cast<void**>(data)[-1];
            std::free(raw);
        }
#endif

        data = nullptr;
        size = 0;
    }

    // Move constructor
    AlignedBuffer::AlignedBuffer(AlignedBuffer&& other) noexcept {
        data = other.data;
        size = other.size;
        other.data = nullptr;
        other.size = 0;
    }

    // Move assignment
    AlignedBuffer& AlignedBuffer::operator=(AlignedBuffer&& other) noexcept {
        if (this != &other) {
            // Free existing buffer
#if defined(_MSC_VER)
            if (data) {
                _aligned_free(data);
            }
#else
            if (data) {
                void* raw = reinterpret_cast<void**>(data)[-1];
                std::free(raw);
            }
#endif
            // Steal ownership
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }


    // -----------------------------
    // PinkNoise methods
    // -----------------------------
    PinkNoise::PinkNoise(int seed) : seed_(seed) {}

    // Generate white noise into target (contiguous width*height). RNG seeded by octaveSeed.
    void PinkNoise::generate_white_layer(float* target, int width, int height, int octaveSeed) const {
        std::mt19937 rng;
        if (octaveSeed >= 0) rng.seed(static_cast<unsigned int>(octaveSeed));
        else if (seed_ >= 0) rng.seed(static_cast<unsigned int>(seed_));
        else rng.seed(std::random_device{}());

        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        int N = width * height;
        for (int i = 0; i < N; ++i) target[i] = dist(rng);
    }

    // Build integral image: dst has dims (height+1) x (width+1). dst is contiguous and must be (width+1)*(height+1) floats.
    // We keep row0 and col0 as zeros to simplify box sum queries.
    void PinkNoise::build_integral(const float* src, float* dst, int width, int height) {
        int iw = width + 1;
        int ih = height + 1;
        // zero first row
        for (int x = 0; x < iw; ++x) dst[x] = 0.0f;

        for (int y = 1; y < ih; ++y) {
            float rowSum = 0.0f;
            dst[y * iw + 0] = 0.0f; // first column
            const float* srcRow = src + (y - 1) * width;
            float* dstRow = dst + y * iw;
            for (int x = 1; x < iw; ++x) {
                rowSum += srcRow[x - 1];
                // integral = previous row integral + rowSum
                dstRow[x] = dstRow[x - iw] + rowSum;
            }
        }
    }

    // Box average using integral image. Writes mean into out (w*h). blockSize >=1.
    // Here we use top-left anchored blocks: block at (bx,b y) covers [bx, bx+blockSize-1] x [by, by+blockSize-1]
    // For compatibility with existing behavior we keep same anchoring: blocks start at multiples of blockSize
    void PinkNoise::box_average_from_integral(const float* integral, float* out, int width, int height, int blockSize) {
        int iw = width + 1;
        // We'll compute for each block region and fill block with same average to keep legacy visual style
        // But we implement per-pixel average using the block containing that pixel: compute block start bx = (x/blockSize)*blockSize
        // This keeps same pattern as previous block-averaging
        for (int y = 0; y < height; ++y) {
            int by = (y / blockSize) * blockSize;
            int ey = std::min(by + blockSize, height);
            for (int x = 0; x < width; ++x) {
                int bx = (x / blockSize) * blockSize;
                int ex = std::min(bx + blockSize, width);
                // integral coordinates are +1 offset
                int x1 = bx;
                int y1 = by;
                int x2 = ex; // exclusive
                int y2 = ey; // exclusive
                // sum = I(y2,x2) - I(y1,x2) - I(y2,x1) + I(y1,x1)
                float s = integral[y2 * iw + x2] - integral[y1 * iw + x2] - integral[y2 * iw + x1] + integral[y1 * iw + x1];
                int count = (y2 - y1) * (x2 - x1);
                out[y * width + x] = (count > 0) ? (s / static_cast<float>(count)) : 0.0f;
            }
        }
    }

    // -----------------------------
    // High-level generator
    // -----------------------------
    std::vector<std::vector<float>> generate_pink_map(
        int width,
        int height,
        int octaves,
        float alpha,
        int sampleRate,
        float amplitude,
        int seed
    ) {
        if (width <= 0 || height <= 0) throw std::invalid_argument("width/height must be > 0");
        if (octaves < 1) throw std::invalid_argument("octaves must be >= 1");
        if (alpha < 0.0f) alpha = 0.0f;
        if (amplitude <= 0.0f) amplitude = 1.0f;
        if (sampleRate < 1) sampleRate = 44100;

        // accumulator (contiguous aligned)
        AlignedBuffer accBuf(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
        float* acc = accBuf.get();
        std::fill(acc, acc + (width * height), 0.0f);

        // integral image temp buffer size (width+1)*(height+1)
        AlignedBuffer integralBuf(static_cast<std::size_t>(width + 1) * static_cast<std::size_t>(height + 1));
        float* integral = integralBuf.get();

        // white layer buffer
        AlignedBuffer layerBuf(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
        float* layer = layerBuf.get();

        PinkNoise pn(seed);

        double totalWeight = 0.0;

        // base spacing derived from sampleRate to emulate frequency spacing
        float baseSpacing = std::max(1.0f, std::sqrt(static_cast<float>(sampleRate) / 44100.0f));

        // threading config
        unsigned int hw = std::max(1u, std::thread::hardware_concurrency());
        unsigned int numThreads = std::min<unsigned int>(hw, 8); // conservative cap

        for (int o = 0; o < octaves; ++o) {
            int blockSize = static_cast<int>(std::max(1.0f, baseSpacing * std::pow(2.0f, static_cast<float>(o))));
            int octaveSeed = (seed >= 0) ? (seed + o) : (-1);

            // 1) generate white layer
            pn.generate_white_layer(layer, width, height, octaveSeed);

            // 2) build integral image (single-threaded; O(width*height))
            // integral buffer has (height+1) rows of (width+1) floats
            // set to 0 at start (constructor zeros buffer)
            PinkNoise::build_integral(layer, integral, width, height);

            // 3) compute box-average using integral and write into temp 'layer' buffer (reuse layerBuf)
            // We'll write the averages into a separate aligned buffer 'avgBuf' to avoid race
            AlignedBuffer avgBuf(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
            float* avg = avgBuf.get();

            // We parallelize block-averaging by rows: each thread computes its row range
            std::vector<std::thread> workers;
            std::atomic<int> nextRow{ 0 };

            // Replaced old worker lambda compare to last push 
            auto worker = [&]() {
                int y;
                int iw = width + 1;

                while ((y = nextRow.fetch_add(1, std::memory_order_relaxed)) < height) {

                    int by = (y / blockSize) * blockSize;
                    int ey = std::min(by + blockSize, height);

                    for (int x = 0; x < width; ++x) {

                        int bx = (x / blockSize) * blockSize;
                        int ex = std::min(bx + blockSize, width);

                        // +1 offset for integral image
                        int x1 = bx;
                        int y1 = by;
                        int x2 = ex;
                        int y2 = ey;

                        // summed area table:
                        // I(y2,x2) - I(y1,x2) - I(y2,x1) + I(y1,x1)
                        float s =
                            integral[y2 * iw + x2] -
                            integral[y1 * iw + x2] -
                            integral[y2 * iw + x1] +
                            integral[y1 * iw + x1];

                        int count = (y2 - y1) * (x2 - x1);
                        avg[y * width + x] = (count > 0) ? (s / count) : 0.0f;
                    }
                }
                };


            // Replaced thread creation as compare to last push
            for (unsigned int t = 0; t < numThreads; ++t)
                workers.emplace_back(worker); // no thread id needed (zero-arg lambda)
            for (auto& th : workers) th.join();
                workers.clear();

            // 4) accumulate with weight: acc += avg * weight
            float weight = 1.0f / std::pow(static_cast<float>(blockSize), alpha);
            totalWeight += weight;

            // Vectorized accumulate if AVX2 available
#if defined(__AVX2__)
            const int N = width * height;
            int i = 0;
            const int step = 8; // 8 floats per __m256
            __m256 wv = _mm256_set1_ps(weight);
            for (; i + step <= N; i += step) {
                __m256 a = _mm256_load_ps(acc + i);
                __m256 b = _mm256_load_ps(avg + i);
                __m256 prod = _mm256_mul_ps(b, wv);
                __m256 sum = _mm256_add_ps(a, prod);
                _mm256_store_ps(acc + i, sum);
            }
            // tail
            for (; i < N; ++i) acc[i] += avg[i] * weight;
#else
            int N = width * height;
            for (int i = 0; i < N; ++i) acc[i] += avg[i] * weight;
#endif
            // avgBuf frees on scope exit
        }

        // Normalize accumulator by totalWeight and apply amplitude. Vectorize where possible
        int Npix = width * height;
#if defined(__AVX2__)
        int i = 0;
        __m256 invW = _mm256_set1_ps(static_cast<float>(1.0 / totalWeight));
        __m256 ampv = _mm256_set1_ps(amplitude);
        for (; i + 8 <= Npix; i += 8) {
            __m256 v = _mm256_load_ps(acc + i);
            v = _mm256_mul_ps(v, invW);
            v = _mm256_mul_ps(v, ampv);
            // clamp 0..1
            __m256 zero = _mm256_setzero_ps();
            __m256 one = _mm256_set1_ps(1.0f);
            v = _mm256_max_ps(zero, _mm256_min_ps(v, one));
            _mm256_store_ps(acc + i, v);
        }
        for (; i < Npix; ++i) {
            float val = acc[i] / static_cast<float>(totalWeight);
            val = val * amplitude;
            if (val < 0.0f) val = 0.0f;
            if (val > 1.0f) val = 1.0f;
            acc[i] = val;
        }
#else
        for (int i = 0; i < Npix; ++i) {
            float val = acc[i] / static_cast<float>(totalWeight);
            val = val * amplitude;
            if (val < 0.0f) val = 0.0f;
            if (val > 1.0f) val = 1.0f;
            acc[i] = val;
        }
#endif

        // Convert contiguous acc buffer to std::vector<std::vector<float>> for public API
        std::vector<std::vector<float>> out(height, std::vector<float>(width));
        for (int y = 0; y < height; ++y) {
            std::memcpy(out[y].data(), acc + (std::size_t)y * width, sizeof(float) * static_cast<std::size_t>(width));
        }
        return out;
    }

    // Save image uses previous utility style: single-channel
    void save_pink_image(const std::vector<std::vector<float>>& noise, const std::string& filename, const std::string& outputDir) {
        if (noise.empty() || noise[0].empty()) throw std::invalid_argument("Cannot save empty pink map.");
        int height = static_cast<int>(noise.size());
        int width = static_cast<int>(noise[0].size());
        std::vector<unsigned char> img(width * height);
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                img[y * width + x] = static_cast<unsigned char>(std::clamp(noise[y][x], 0.0f, 1.0f) * 255.0f);
        std::filesystem::path outDir = outputDir.empty() ? (std::filesystem::current_path().parent_path() / "ImageOutput") : outputDir;
        std::filesystem::create_directories(outDir);
        std::filesystem::path file = outDir / filename;
        std::string ext = file.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        int res = 0;
        if (ext == ".jpg" || ext == ".jpeg") res = stbi_write_jpg(file.string().c_str(), width, height, 1, img.data(), 95);
        else res = stbi_write_png(file.string().c_str(), width, height, 1, img.data(), width);
        if (res == 0) throw std::runtime_error("Failed to write pink noise image: " + file.string());
        std::cout << "[OK] Pink noise saved at: " << file.string() << "\n";
    }

    std::vector<std::vector<float>> create_pinknoise(
        int width,
        int height,
        int octaves,
        float alpha,
        int sampleRate,
        float amplitude,
        int seed,
        OutputMode mode,
        const std::string& filename,
        const std::string& outputDir
    ) {
        auto map = generate_pink_map(width, height, octaves, alpha, sampleRate, amplitude, seed);
        if (mode == OutputMode::Image) save_pink_image(map, filename, outputDir);
        return map;
    }

} // namespace Noise

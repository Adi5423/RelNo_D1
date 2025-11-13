#include "Noise.hpp"

int main() {
    using namespace Noise;

    //White noise test image
    create_whitenoise(
		256, 256,   // width, height 
		21,         // seed
        OutputMode::Image, 
        "whiteNoise.png"
    );

    //Perlin noise test image 
    create_perlinnoise(512, 512, 50.0f, 6, 1.0f, 0.5f, 2.0f, 0.0f, 42, OutputMode::Image, "perlinNoise.png");
    create_perlinnoise(
		512, 512,   // width, height
		75.0f,      // scale
		3,          // octaves
		1.0f,       // frequency
		0.5f,       // persistence
		1.5f,       // lacunarity
		0.2f,       // base
		89,         // seed
        OutputMode::Image, 
        "perlin_noise.jpg"
    );

    //Simplex noise test image
    create_simplexnoise(512, 512, 60.0f, 4, 0.5f, 2.0f, 0.0f, 33, OutputMode::Image, "simplexNoise.png");
    create_simplexnoise(
		512, 512,   // width, height 
        60.0f,      // scale
		2,          // octaves
		1.0f,       // persistence
		1.0f,       // lacunarity
		0.0f,       // base
		52,         // seed
        OutputMode::Image, 
        "simplex_Noise.png"
    );
    
    // Pink noise test image
    create_pinknoise(
        512, 512,
        6,          // octaves
        1.0f,       // alpha (pink)
        44100,      // sample rate
        1.0f,       // amplitude
        123,        // seed
        OutputMode::Image,
        "pinkNoise.png"
    );

    // Pink noise test image
    create_pinknoise(
        512, 512,
        3,          // octaves
        0.8f,       // alpha (pink)
        23400,      // sample rate
        0.5f,       // amplitude
        314,        // seed
        OutputMode::Image,
        "pink_noise.png"
    );
    
    return 0;
}

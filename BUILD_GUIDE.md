# GameWindow - Build Guide

## 🎯 Overview

A professional OpenGL 4.6 window application with integrated RelNo_D1 noise generation library.

**Status:** ✅ Production Ready - Zero Warnings Build

---

## ⚡ Quick Start

```powershell
# Clean build
Remove-Item -Path build -Recurse -Force -ErrorAction SilentlyContinue
cmake -S . -B build
cmake --build build

# Run
.\build\GameWindow.exe
```

**Expected Output:**
```
OpenGL version: 4.6.0 - Build 31.0.101.4502
Generating Perlin noise using RelNo_D1...
Noise generated! Sample value = 0.433333
```

---

## 📦 Project Structure

```
GameWindow/
├── CMakeLists.txt          # Professional build configuration
├── README.md               # Project overview
├── BUILD_GUIDE.md          # This file
├── src/
│   └── mainWindow.cpp      # Main application with RelNo_D1 integration
├── vendor/                 # Third-party libraries (submodules)
│   ├── glfw/               # Window + input handling
│   ├── glad/               # OpenGL function loader
│   ├── glm/                # Math library
│   ├── stb/                # Image utilities
│   └── relno_d1/           # Noise generation library
└── build/                  # Generated build files
    └── GameWindow.exe      # Final executable
```

---

## 🔧 Build Configuration Highlights

### **Zero Warning Build**
- ✅ All vendor library warnings suppressed (`-w` flag)
- ✅ Strict warnings enabled for your code only (`-Wall -Wextra`)
- ✅ Clean, professional output

### **Colored Output**
- ✅ Enabled by default for GCC/Clang
- ✅ Better readability during development

### **Optimized Vendor Builds**
- ✅ GLFW: Docs, tests, examples disabled
- ✅ RelNo_D1: Examples disabled
- ✅ Faster compilation times

### **Modern C++ Standards**
- ✅ C++20 enabled
- ✅ Release build by default

---

## 🎨 What's Included

### **Graphics Stack**
- **OpenGL 4.6** - Modern graphics API
- **GLFW 3.x** - Cross-platform windowing
- **GLAD** - OpenGL function loader
- **GLM** - OpenGL Mathematics library

### **Noise Generation (RelNo_D1)**
- **WhiteNoise** - Random noise
- **PerlinNoise** - Smooth, natural-looking noise
- **SimplexNoise** - Improved Perlin noise
- **PinkNoise** - 1/f noise

---

## 🚀 Usage Example

Current implementation (in `src/mainWindow.cpp`):

```cpp
// Generate 256x256 Perlin noise map
auto noiseMap = create_perlinnoise(
    256, 256,      // Resolution
    40.0f,         // Scale
    4,             // Octaves
    1.0f,          // Frequency
    0.5f,          // Persistence
    2.0f,          // Lacunarity
    0.0f,          // Base offset
    21,            // Seed
    OutputMode::None,
    ""
);
```

---

## 📝 Development Notes

### **Adding Your Code**
1. Edit `src/mainWindow.cpp` or add new `.cpp` files
2. If adding files, update `CMakeLists.txt`:
   ```cmake
   add_executable(GameWindow
       src/mainWindow.cpp
       src/yourNewFile.cpp  # Add here
   )
   ```

### **Warnings**
- Your code: Strict warnings enabled (catch bugs early)
- Vendor libs: All warnings suppressed (can't fix them anyway)

### **Clean Rebuild**
```powershell
Remove-Item -Path build -Recurse -Force
cmake -S . -B build
cmake --build build
```

---

## 🎓 Next Steps

### **Rendering the Noise**
Upload `noiseMap` to OpenGL texture and render it:
- Create texture from noise data
- Set up VAO/VBO for fullscreen quad
- Write simple fragment shader

### **Project Ideas**
- ☁️ **Procedural Clouds** - Use Perlin noise for cloud textures
- 🏔️ **Terrain Generation** - Create heightmaps (see OBJ example)
- 🌊 **Water Simulation** - Animated noise for water surfaces
- 🎨 **Procedural Textures** - Generate unique textures

---

## ✅ Verification Checklist

After building, verify:
- [x] No warnings during compilation
- [x] Clean build output (`[35/35] Linking CXX executable GameWindow.exe`)
- [x] Window opens successfully
- [x] Console shows OpenGL version
- [x] Noise generation message appears
- [x] Sample value is displayed

---

## 🐛 Troubleshooting

### Build fails with "submodule not found"
```powershell
git submodule update --init --recursive
```

### Missing DLL errors
The project is statically linked - no DLLs needed!

### Window doesn't open
Check graphics drivers support OpenGL 4.6

---

## 📄 License

See `LICENSE.txt` for details.

**Ready to build amazing graphics applications!** 🎉

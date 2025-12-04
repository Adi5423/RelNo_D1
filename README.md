## This is just a test using RelNo_D1 to convert the perlin map generated to a real 3D terrain to use.

This will build a .obj terrain file using perlin noise map via RelNo_D1

```bash
mkdir build
cd build
```

Initialise and load all the submodules of package (here RelNo_D1) 
```bash
git submodule update --init --recursive
```

Building and Linking the files
```bash
cmake .. "-DCMAKE_INSTALL_PREFIX=../install"
cmake --build . --config Release
```


After you see successfull "Linking CXX executable RelNoTerrain3D.exe" 
Run the exe file 
```bash
.\RelNoTerrain3D.exe
```
After running the exe you will get a ouptut folder with the .obj file inside generated into the build fodler itself.

Which can be easily imported to 3d Softawre to see a terrain.

Ex: 
Blender 3D -> file -> import -> obj -> {select this .obj file from RelNo_D1/build/output/terrain_perlin.obj} .



for headers only package structure :
```
cmake --install .
```

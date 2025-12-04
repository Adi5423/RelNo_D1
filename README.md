This will build a .obj terrain file using perlin noise map via RelNo_D1

```bash
mkdir build
cd build
```
```bash
git submodule update --init --recursive
```
```
cmake .. "-DCMAKE_INSTALL_PREFIX=../install"
cmake --build . --config Release
```
After you see successfull "Linking CXX executable RelNoTerrain3D.exe" 
Run the exe file 
```bash
.\RelNoTerrain3D.exe
```
After running the exe you will get a ouptut folder with the .obj file inside generated into the build fodler itself.

for headers only package structure :
```
cmake --install .
```

# Camera Controller - Quick Reference

## 🎮 Controls

### **Activation**
- **Hold RIGHT MOUSE BUTTON** - Activate camera control mode
  - Cursor becomes hidden and locked
  - Mouse movement controls camera view
  - Release to return to normal cursor

### **Movement** (Right Mouse Button must be held)
- **W** - Move forward
- **S** - Move backward
- **A** - Move left (strafe)
- **D** - Move right (strafe)
- **Q** - Move up (world space)
- **E** - Move down (world space)

### **Speed Control**
- **SHIFT** (Left or Right) - Sprint mode (2x speed)
  - Hold while moving for faster movement
  - Works with all movement keys

### **Camera View**
- **Mouse Movement** - Look around (when right button held)
  - Horizontal movement: Yaw (left/right)
  - Vertical movement: Pitch (up/down)
  - Pitch is clamped to ±89° to prevent gimbal lock

### **Zoom**
- **Mouse Scroll Wheel** - Adjust field of view (1° - 45°)

### **Exit**
- **ESC** - Close the window

---

## 🔧 Technical Details

### Camera Settings (Configurable in Camera.hpp)
```cpp
movementSpeed = 2.5f;        // Base movement speed
sprintMultiplier = 2.0f;     // Sprint speed multiplier
mouseSensitivity = 0.1f;     // Mouse look sensitivity
zoom = 45.0f;                // Default FOV in degrees
```

### Default Camera Position
```cpp
Position: (0, 0, 5)
Looking at: -Z axis (forward)
Up: +Y axis
```

---

## 💡 Usage Tips

1. **First-Time Use**
   - Right-click in the window to activate camera
   - Move mouse to look around
   - Use WASD to navigate

2. **Smooth Movement**
   - Movement is frame-rate independent (uses deltaTime)
   - Sprint with Shift for quick repositioning
   - Q/E for precise vertical adjustments

3. **Best Practices**
   - Release right mouse button when not actively controlling
   - Use scroll wheel to adjust FOV for different perspectives
   - Combine movement keys (e.g., W+D) for diagonal movement

---

## 🎯 Integration Example

To use the camera in your rendering code:

```cpp
// In your render loop:
glm::mat4 view = camera.getViewMatrix();
glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);

// Pass to your shaders:
glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
```

---

## 🐛 Troubleshooting

**Camera not responding?**
- Make sure you're holding the right mouse button
- Check that the window has focus

**Movement too fast/slow?**
- Adjust `movementSpeed` in Camera.hpp
- Use Shift for temporary speed boost

**Mouse too sensitive?**
- Lower `mouseSensitivity` in Camera.hpp (try 0.05f)

**Camera flipping at extreme angles?**
- This is prevented by pitch clamping (±89°)
- If it happens, there's a bug - please report!

---

## 📝 Next Steps

### Add 3D Objects
The camera is ready! Now you can:
1. Create vertex buffers for 3D geometry
2. Write vertex/fragment shaders
3. Use `camera.getViewMatrix()` and `camera.getProjectionMatrix()`
4. Render your scene!

### Example Scene Ideas
- ☁️ Render the Perlin noise as a 3D cloud volume
- 🏔️ Create a terrain from the noise heightmap
- 📦 Place simple cubes to test camera movement
- 🌌 Add a skybox for immersion

---

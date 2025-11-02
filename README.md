# 🌙 3D Maze Runner - Night Edition

A thrilling 3D maze game set in the darkness of night. Navigate through a procedurally generated maze, collect food items, and use your jumping ability to overcome obstacles!

**Available in 3 languages: JavaScript/HTML, Python, and C++!**

## 🎮 Game Features

- **3D Graphics**: High-quality 3D rendering with advanced visual effects
- **Procedural Generation**: Every game generates a unique random maze using recursive backtracking algorithm
- **Night Theme**: Dark atmospheric environment with moon, stars, and ambient lighting
- **Wall Jumping**: Jump over walls to find shortcuts and reach hidden areas
- **Food Collection**: Collect all food items scattered throughout the maze to win
- **Smooth Controls**: Intuitive WASD movement with mouse/arrow key camera controls
- **Multiple Versions**: Choose from JavaScript (browser), Python, or C++ implementations

## 🎯 How to Play

### Controls

- **W** - Move forward
- **S** - Move backward
- **A** - Move left
- **D** - Move right
- **Space** - Jump (can jump over walls!)
- **Arrow Keys / Mouse** - Look around / Rotate camera view

### Objective

- Navigate through the dark maze
- Collect all food items (🍎) scattered throughout
- Use your jumping ability strategically to overcome walls and find shortcuts
- Complete the maze by collecting all items

### Gameplay Tips

1. **Jump Over Walls**: When you see a wall blocking your path, press Space to jump high enough to pass over it
2. **Explore Thoroughly**: The maze is large and food items are hidden in various locations
3. **Use Camera Rotation**: Look around with mouse or arrow keys to better understand your surroundings
4. **Stay Alert**: The night setting makes visibility challenging - use the lighting to navigate

## 🚀 Getting Started

### Version 1: JavaScript/HTML (Recommended - Best Graphics)

**Prerequisites:**
- Modern web browser (Chrome, Firefox, Edge, Safari)
- Internet connection (for loading Three.js library from CDN)

**Installation:**
1. Download or clone this repository
2. Open `index.html` in your web browser
3. Click "Start Game" on the instructions screen
4. Enjoy the game with maximum graphics quality!

**Features:**
- ✅ Post-processing effects (Bloom, Tone Mapping)
- ✅ Advanced particle systems
- ✅ Procedural textures
- ✅ Enhanced lighting with multiple light sources
- ✅ Smooth animations and effects
- ✅ No installation required - runs in browser!

---

### Version 2: Python

**Prerequisites:**
- Python 3.7 or higher
- Pygame and PyOpenGL libraries

**Installation:**
```bash
pip install -r requirements.txt
```

**Run:**
```bash
python maze_runner_python.py
```

**Features:**
- ✅ Cross-platform compatibility
- ✅ OpenGL 3D graphics
- ✅ Same gameplay mechanics
- ✅ Easy to modify and extend

---

### Version 3: C++

**Prerequisites:**
- C++ compiler (GCC, Clang, or MSVC)
- SFML library (for window management)
- OpenGL libraries

**Installation:**

**Linux:**
```bash
sudo apt-get install libsfml-dev libgl1-mesa-dev libglu1-mesa-dev
make
./maze_runner
```

**Windows (MinGW/MSYS2):**
```bash
pacman -S mingw-w64-x86_64-sfml mingw-w64-x86_64-opengl
g++ -std=c++17 -o maze_runner maze_runner.cpp -lsfml-window -lsfml-system -lopengl32 -lglu32
maze_runner.exe
```

**macOS:**
```bash
brew install sfml
make
./maze_runner
```

**Features:**
- ✅ High performance
- ✅ Native compilation
- ✅ Professional game development ready

## 🛠️ Technical Details

### Technologies Used

#### JavaScript/HTML Version:
- **HTML5**: Structure and layout
- **CSS3**: Styling and UI design
- **JavaScript (ES6+)**: Game logic
- **Three.js**: 3D graphics rendering engine
- **Post-Processing**: Bloom, Tone Mapping effects

#### Python Version:
- **Python 3.7+**: Game logic
- **Pygame**: Window management and input
- **PyOpenGL**: 3D graphics rendering
- **NumPy**: Mathematical operations

#### C++ Version:
- **C++17**: Game logic and performance
- **SFML**: Window management and input handling
- **OpenGL**: 3D graphics rendering
- **GLU**: Utility library for 3D shapes

### Maze Generation Algorithm

The game uses a **Recursive Backtracking** algorithm to generate random mazes:
- Ensures all areas of the maze are accessible
- Creates a unique maze layout every time you play
- Generates a 25x25 cell maze for challenging gameplay

### Game Mechanics

- **Physics**: Gravity and jumping mechanics
- **Collision Detection**: Wall collision with jump-over capability
- **Food System**: Random placement of collectible items
- **Camera System**: Third-person camera that follows the player

## 📁 File Structure

```
Maze Runner/
│
├── index.html              # JavaScript/HTML version (Best graphics)
├── maze_runner_python.py   # Python version
├── maze_runner.cpp         # C++ version
├── Makefile                # Build file for C++ version
├── requirements.txt        # Python dependencies
└── README.md              # This documentation file
```

## 🎨 Game Design

### Visual Style

#### JavaScript Version (Maximum Graphics):
- **Dark Theme**: Night-time atmosphere with dark colors
- **Enhanced Moon**: Procedurally generated moon texture with craters and glow
- **Advanced Lighting**: Multiple light sources (moonlight, fill lights, rim lights)
- **Particle Systems**: Atmospheric particles and collection effects
- **Post-Processing**: Bloom effects, tone mapping, and color grading
- **Procedural Textures**: Generated textures for walls, ground, and moon
- **Animated Elements**: Floating food items, rotating stars, particle animations
- **8,000 Stars**: Twinkling starfield with varying colors and sizes
- **Shadow System**: High-quality soft shadows (4096x4096 shadow maps)

#### Python/C++ Versions:
- **Dark Theme**: Night-time atmosphere
- **Moon Lighting**: Directional light simulating moonlight
- **Stars**: Procedurally placed starfield
- **Glowing Elements**: Player and food items with emissive materials

### Sound Design

*Note: Sound effects can be added in future versions*

## 🔮 Future Enhancements

Potential features for future versions:
- Sound effects and background music
- Difficulty levels (maze size options)
- Timer/challenge mode
- Multiple maze themes
- Leaderboard system
- Mobile touch controls

## 🐛 Troubleshooting

### JavaScript Version

**Game won't start:**
- Ensure you have JavaScript enabled in your browser
- Check your internet connection (for CDN resources)
- Try refreshing the page

**Controls not working:**
- Click on the game canvas to focus it
- Make sure no other applications are intercepting keyboard input

**Performance issues:**
- Close other browser tabs to free up resources
- Update your browser to the latest version
- Check your graphics drivers
- Reduce browser zoom level if needed

### Python Version

**Import errors:**
- Ensure all dependencies are installed: `pip install -r requirements.txt`
- Check Python version: `python --version` (should be 3.7+)

**Display issues:**
- Update graphics drivers
- Try running with different OpenGL settings

### C++ Version

**Compilation errors:**
- Ensure SFML and OpenGL development libraries are installed
- Check compiler version supports C++17
- Verify library paths are correct

**Runtime errors:**
- Check that required libraries are in system path
- Verify OpenGL context creation

## 📝 License

This project is open source and available for educational purposes.

## 👨‍💻 Development

This project demonstrates the same game concept implemented in three different languages:

- **JavaScript/HTML**: Maximum graphics quality with advanced effects, best for showcasing visual capabilities
- **Python**: Cross-platform, easy to modify, great for learning and prototyping
- **C++**: High performance, native execution, professional game development ready

Each version implements the same core gameplay mechanics:
- Random maze generation using recursive backtracking
- Player movement and wall-jumping
- Food collection system
- Night theme with atmospheric lighting

Choose the version that best suits your needs!

## 📊 Version Comparison

| Feature | JavaScript | Python | C++ |
|---------|-----------|--------|-----|
| Graphics Quality | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| Performance | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Ease of Use | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ |
| Cross-Platform | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| Setup Required | None | Python + libs | Compiler + libs |
| Post-Processing | Yes | No | No |
| Particle Systems | Advanced | Basic | Basic |

## 🎉 Enjoy!

Have fun exploring the maze and collecting all the food items. Remember - you can jump over walls, so be creative with your pathfinding!

Try all three versions to see how the same game concept can be implemented differently across languages!

---

*Made with ❤️ using Three.js, Pygame/PyOpenGL, and SFML/OpenGL*


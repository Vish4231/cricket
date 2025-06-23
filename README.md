# Cricket Manager Simulation Game

A professional-grade Cricket Management Simulation Game built with C++17, SDL2, and OpenGL. Experience the thrill of managing cricket teams in a Football Manager-style interface with deep tactical gameplay, realistic match simulations, and immersive IPL-style auctions.

## 🎯 Features

### 🏏 Core Gameplay
- **Professional GUI**: Modern tabbed interface with dark theme and custom styling
- **Player Management**: Comprehensive player profiles with 100+ attributes
- **Team Management**: Squad building, tactics, and formation management
- **Match Simulation**: Real-time ball-by-ball cricket simulation
- **Auction System**: IPL-style bidding with AI-controlled franchises

### 🎮 Match Engine
- **Live Match Simulation**: 2.5D/3D cricket field with player animations
- **Realistic Cricket Rules**: LBW, run outs, no balls, pitch conditions
- **Commentary System**: Text and audio commentary with multiple commentators
- **Match Highlights**: Replays and statistical analysis
- **Weather Effects**: Dynamic weather impacting gameplay

### 🏆 Career Mode
- **Multiple Formats**: Test, ODI, and T20 cricket
- **Tournaments**: IPL, World Cup, County Championship
- **Player Development**: Youth academy and scouting system
- **Financial Management**: Budgets, sponsorships, and contracts
- **Team Chemistry**: Player morale and team dynamics

### 🎨 Visual & Audio
- **3D Graphics**: OpenGL rendering with shader support
- **Player Animations**: Rigged models with batting, bowling, and fielding animations
- **Sound Effects**: Crowd noise, bat hits, appeals, and commentary
- **Modern UI**: ImGui-based interface with custom themes

## 🛠️ Technical Stack

- **Language**: C++17
- **Graphics**: SDL2 + OpenGL 3.3
- **GUI**: ImGui
- **Audio**: SDL2_mixer
- **Build System**: CMake
- **Platform**: macOS (with Windows/Linux support planned)

## 📋 Requirements

### System Requirements
- **OS**: macOS 10.15 or later
- **CPU**: Intel i5 or Apple M1 equivalent
- **RAM**: 8GB minimum, 16GB recommended
- **GPU**: OpenGL 3.3 compatible graphics card
- **Storage**: 2GB available space

### Development Requirements
- **Compiler**: Clang 12+ or GCC 9+
- **CMake**: 3.16+
- **SDL2**: 2.0.20+
- **OpenGL**: 3.3+
- **GLEW**: 2.1+

## 🚀 Installation

### Prerequisites

1. **Install Homebrew** (if not already installed):
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. **Install Dependencies**:
```bash
brew install cmake sdl2 sdl2_image sdl2_ttf sdl2_mixer glew glm
```

### Building from Source

1. **Clone the Repository**:
```bash
git clone https://github.com/yourusername/cricket-manager.git
cd cricket-manager
```

2. **Create Build Directory**:
```bash
mkdir build
cd build
```

3. **Configure with CMake**:
```bash
cmake ..
```

4. **Build the Project**:
```bash
make -j$(nproc)
```

5. **Run the Game**:
```bash
./CricketManager
```

## 🎮 How to Play

### Getting Started
1. **Launch the Game**: Start Cricket Manager from the terminal
2. **Main Menu**: Choose between Career Mode, Quick Match, or Auction
3. **Team Selection**: Pick your favorite team or create a new one
4. **Squad Management**: Review and manage your player roster
5. **Tactics**: Set batting order, bowling order, and fielding positions
6. **Match Day**: Experience live cricket simulation

### Key Controls
- **Mouse**: Navigate GUI and interact with elements
- **Escape**: Pause match or return to previous menu
- **Space**: Advance match simulation
- **Tab**: Switch between GUI tabs
- **F11**: Toggle fullscreen mode

### Game Modes

#### Career Mode
- Manage a team across multiple seasons
- Participate in various tournaments
- Develop youth players
- Handle finances and contracts
- Build team chemistry and morale

#### Quick Match
- Jump straight into match simulation
- Choose teams, venue, and format
- Experience tactical gameplay
- Watch live commentary and highlights

#### Auction Mode
- Participate in IPL-style player auctions
- Bid against AI-controlled teams
- Manage team budget
- Build your dream squad

## 🏗️ Project Structure

```
CricketManager/
├── include/                 # Header files
│   ├── Game.h              # Main game class
│   ├── Player.h            # Player management
│   ├── Team.h              # Team management
│   ├── MatchEngine.h       # Cricket simulation engine
│   ├── AuctionManager.h    # Auction system
│   ├── GUIManager.h        # User interface
│   ├── CommentaryManager.h # Commentary system
│   ├── AnimationHandler.h  # 3D animations
│   └── Venue.h             # Stadium management
├── src/                    # Source files
│   ├── main.cpp            # Entry point
│   ├── Game.cpp            # Main game implementation
│   ├── Player.cpp          # Player logic
│   ├── Team.cpp            # Team logic
│   ├── MatchEngine.cpp     # Match simulation
│   ├── AuctionManager.cpp  # Auction logic
│   ├── GUIManager.cpp      # GUI implementation
│   ├── CommentaryManager.cpp # Commentary system
│   ├── AnimationHandler.cpp # Animation system
│   └── Venue.cpp           # Venue management
├── assets/                 # Game assets
│   ├── models/             # 3D models
│   ├── textures/           # Textures
│   ├── sounds/             # Audio files
│   ├── fonts/              # Fonts
│   └── data/               # Game data
├── shaders/                # GLSL shaders
├── third_party/            # Third-party libraries
├── CMakeLists.txt          # Build configuration
├── Info.plist              # macOS bundle info
└── README.md               # This file
```

## 🔧 Configuration

### Graphics Settings
- **Resolution**: 1280x720 (default), supports up to 4K
- **Fullscreen**: Toggle with F11
- **VSync**: Enabled by default
- **Anti-aliasing**: MSAA 4x
- **Shadow Quality**: High/Medium/Low

### Audio Settings
- **Master Volume**: 0-100%
- **Commentary Volume**: 0-100%
- **Sound Effects**: 0-100%
- **Crowd Volume**: 0-100%

### Game Settings
- **Match Speed**: Slow/Normal/Fast
- **Commentary Style**: Excited/Calm/Analytical
- **AI Difficulty**: Easy/Medium/Hard
- **Auto-save**: Every 5 minutes

## 🐛 Troubleshooting

### Common Issues

**Game won't start:**
- Ensure all dependencies are installed
- Check OpenGL compatibility
- Verify SDL2 installation

**Poor performance:**
- Reduce graphics settings
- Close other applications
- Update graphics drivers

**Audio issues:**
- Check system audio settings
- Verify SDL2_mixer installation
- Restart the game

**Build errors:**
- Update CMake to latest version
- Ensure all dependencies are properly linked
- Check compiler compatibility

### Debug Mode
Build with debug information:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Setup
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

### Code Style
- Follow C++17 standards
- Use meaningful variable names
- Add comments for complex logic
- Maintain consistent formatting

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **SDL2 Team**: For the excellent multimedia library
- **ImGui**: For the immediate mode GUI library
- **OpenGL**: For the graphics API
- **Cricket Community**: For inspiration and feedback

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/cricket-manager/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/cricket-manager/discussions)
- **Email**: support@cricketmanager.com

## 🗺️ Roadmap

### Version 1.1 (Q2 2024)
- [ ] Multiplayer support
- [ ] More player animations
- [ ] Enhanced commentary
- [ ] Tournament editor

### Version 1.2 (Q3 2024)
- [ ] Mobile port
- [ ] Cloud saves
- [ ] Mod support
- [ ] Advanced statistics

### Version 2.0 (Q4 2024)
- [ ] VR support
- [ ] AI improvements
- [ ] Real-time updates
- [ ] Esports integration

---

**Cricket Manager** - Where Strategy Meets Cricket! 🏏⚡ 
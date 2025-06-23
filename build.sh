#!/bin/bash

# Cricket Manager Build Script for macOS
# This script automates the build process for the Cricket Manager game

set -e  # Exit on any error

echo "🏏 Cricket Manager Build Script"
echo "================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    print_error "This script is designed for macOS only"
    exit 1
fi

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    print_error "Homebrew is not installed. Please install it first:"
    echo "  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    exit 1
fi

print_status "Checking and installing dependencies..."

# Install/update required packages
DEPENDENCIES=(
    "cmake"
    "sdl2"
    "sdl2_image"
    "sdl2_ttf"
    "sdl2_mixer"
    "glm"
    "curl"
    "nlohmann-json"
)

for dep in "${DEPENDENCIES[@]}"; do
    if brew list "$dep" &>/dev/null; then
        print_status "$dep is already installed"
    else
        print_status "Installing $dep..."
        brew install "$dep"
    fi
done

# Check if Xcode Command Line Tools are installed
if ! xcode-select -p &> /dev/null; then
    print_error "Xcode Command Line Tools are not installed"
    print_status "Installing Xcode Command Line Tools..."
    xcode-select --install
    print_warning "Please complete the Xcode Command Line Tools installation and run this script again"
    exit 1
fi

print_status "Creating build directory..."
mkdir -p build
cd build

print_status "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

print_status "Building project..."
make -j$(sysctl -n hw.ncpu)

if [ $? -eq 0 ]; then
    print_success "Build completed successfully!"
    
    # Check if executable was created
    if [ -f "CricketManager" ]; then
        print_success "Executable created: build/CricketManager"
        
        # Test run (optional)
        read -p "Would you like to test run the application? (y/n): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            print_status "Running Cricket Manager..."
            ./CricketManager
        fi
    else
        print_error "Executable not found after build"
        exit 1
    fi
else
    print_error "Build failed!"
    exit 1
fi

print_success "Build script completed!"
print_status "You can now run the game with: ./build/CricketManager" 
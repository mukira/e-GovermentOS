# e-GovernmentOS Build Script Documentation

## Overview

The new `build.sh` script provides an optimized build system for e-GovernmentOS with:
- **Incremental builds** - Only rebuilds changed files (via ninja)
- **Official build system** - Uses the Python-based e-GovernmentOS build system
- **Auto-configuration** - Detects Chromium source and build settings
- **Proper customizations** - Applies all patches and e-GovernmentOS customizations
- **Agent building** - Builds the e-GovernmentOS agent extension

## Quick Start

### First Time Build

```bash
# Set Chromium source location (one-time setup)
export CHROMIUM_SRC=$HOME/chromium/src

# Run full build - builds ALL components:
#   1. e-GovernmentOS-server (MCP server binary)
#   2. e-GovernmentOS-agent (Chrome extension)  
#   3. e-GovernmentOS browser (Chromium with patches)
./build.sh --full
```

This single command builds the complete e-GovernmentOS stack!

### Daily Development (Incremental)

```bash
# Quick incremental build (only rebuilds changed files)
# Builds all three components with smart incremental logic
./build.sh
```

## Usage

### Basic Commands

```bash
# Incremental build (default, fastest)
./build.sh

# Full rebuild with all patches
./build.sh --full

# Build only the agent extension
./build.sh --agent-only

# Debug build
./build.sh --build-type debug

# Show help
./build.sh --help
```

### Configuration

#### Via Environment Variables

```bash
# Set Chromium source directory
export CHROMIUM_SRC=$HOME/chromium/src

# Set build type (debug or release)
export BUILD_TYPE=release

# Set architecture (arm64, x64, universal)
export ARCHITECTURE=arm64
```

#### Via Command-Line Arguments

```bash
# Override Chromium source
./build.sh --chromium-src /path/to/chromium/src

# Build specific architecture  
./build.sh --arch x64

# Debug build
./build.sh --build-type debug --full
```

## Build Modes

### Incremental Build (Default)

```bash
./build.sh
```

**What it does:**
- Builds agent extension (if needed)
- Checks if e-GovernmentOS is already configured
- If configured: Only runs compile phase (ninja)
- If not configured: Runs setup + prep + compile

**Use when:**
- Making changes to code
- Normal development workflow
- You want the fastest rebuild

**Build time:** 5-30 minutes (depending on changes)

### Full Build

```bash
./build.sh --full
```

**What it does:**
- Cleans build directory
- Initializes git repositories
- Applies all patches
- Copies resources and customizations
- Runs full compilation

**Use when:**
- First time building
- After pulling new patches
- Build is broken and needs clean slate
- Switching architectures

**Build time:** 1-3 hours (full compilation)

### Agent Only

```bash
./build.sh --agent-only
```

**What it does:**
- Only builds the e-GovernmentOS agent extension
- Skips Chromium build entirely

**Use when:**
- Working only on agent features
- Testing extension changes
- Need quick iteration

**Build time:** 1-5 minutes

## Requirements

### System Requirements

- **Disk Space:** ~100GB for Chromium source
- **RAM:** 16GB+ recommended
- **OS:** macOS, Linux, or Windows

### Software Requirements

1. **Python 3** - Required for build system
2. **depot_tools** - Must be in PATH (provides ninja, gn, autoninja)
3. **Chromium source** - Downloaded via depot_tools
4. **Node.js + yarn/npm** - For building agent extension

### Setup depot_tools

```bash
# Clone depot_tools
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git

# Add to PATH (add this to your ~/.bashrc or ~/.zshrc)
export PATH="$PATH:/path/to/depot_tools"
```

### Get Chromium Source

Follow the official guide: https://www.chromium.org/developers/how-tos/get-the-code/

```bash
# This takes 2-3 hours
mkdir ~/chromium && cd ~/chromium
fetch --nohooks chromium
cd src
gclient runhooks

# Set environment variable
export CHROMIUM_SRC=$HOME/chromium/src
```

## How It Works

### Build Phases

The script uses the official e-GovernmentOS build system with these phases:

1. **Setup Phase** (`--setup`)
   - Cleans previous builds
   - Initializes git repositories
   - Sets up Sparkle framework (macOS)

2. **Prep Phase** (`--prep`)
   - Applies Chromium patches
   - Copies e-GovernmentOS resources
   - Replaces strings and branding
   - Applies series patches

3. **Build Phase** (`--build`)
   - Configures build with gn
   - Compiles with ninja (incremental)

### Incremental Build Logic

The script intelligently decides what to build:

```
Has args.gn?
├─ YES → Only run compile (ninja picks up changes automatically)
└─ NO  → Run setup + prep + compile (first time)
```

Ninja automatically tracks:
- Source file changes
- Header dependencies
- Build flag changes
- Only rebuilds affected targets

## Troubleshooting

### Error: Chromium source directory not found

**Solution:** Set `CHROMIUM_SRC` environment variable:
```bash
export CHROMIUM_SRC=$HOME/chromium/src
./build.sh
```

### Error: ninja/autoninja not found

**Solution:** Add depot_tools to PATH:
```bash
export PATH="$PATH:/path/to/depot_tools"
```

### Error: Agent submodule not initialized

**Solution:** Initialize submodules:
```bash
git submodule update --init --recursive
```

### Build is broken after pulling changes

**Solution:** Run full rebuild:
```bash
./build.sh --full
```

### Want to clean everything and start fresh

**Solution:** 
```bash
# Clean Chromium build
cd $CHROMIUM_SRC
rm -rf out/

# Run full build
cd /path/to/e-GovernmentOS
./build.sh --full
```

## Output Locations

After a successful build:

```
e-GovernmentOS/
├── packages/browseros-agent/dist/     # Agent extension
└── (Chromium source)/out/Release/     # e-GovernmentOS binary
    ├── e-GovernmentOS.app/                 # macOS
    ├── chrome.exe                     # Windows
    └── chrome                         # Linux
```

## Running e-GovernmentOS

### macOS
```bash
$CHROMIUM_SRC/out/Release/e-GovernmentOS.app/Contents/MacOS/e-GovernmentOS
```

### Linux
```bash
$CHROMIUM_SRC/out/Release/chrome
```

### Windows
```bash
$CHROMIUM_SRC/out/Release/chrome.exe
```

### Load Agent Extension

1. Open e-GovernmentOS
2. Go to `chrome://extensions/`
3. Enable "Developer mode"
4. Click "Load unpacked"
5. Select `packages/browseros-agent/dist/`

## Comparison with Old Scripts

### Old build.sh
- Ran build_worker.sh in background with nohup
- No integration with patches or customizations
- Referenced non-existent directories

### Old build_worker.sh
- Hardcoded paths for single machine
- Referenced e-GovernmentOS-agent and e-GovernmentOS-server (don't exist)
- Only ran ninja (no patches, no setup)
- No validation or error handling

### New build.sh ✨
- Uses official Python build system
- Applies all patches and customizations
- Auto-detects configuration
- Handles incremental builds intelligently
- Builds actual agent from submodule
- Comprehensive error handling
- Cross-platform support
- Configurable via env vars or CLI

## Tips for Fast Builds

1. **Use incremental builds** - Run `./build.sh` without `--full`
2. **Build agent separately** - Use `--agent-only` when only changing extension
3. **Use ccache** - Set up ccache for faster recompilation
4. **More cores** - Ninja auto-detects CPU cores, uses all available
5. **SSD recommended** - Chromium builds are I/O intensive

## Advanced Usage

### Build Specific Module

```bash
cd packages/browseros
python3 -m build.browseros build --modules compile --chromium-src $CHROMIUM_SRC
```

### List Available Modules

```bash
cd packages/browseros  
python3 -m build.browseros build --list
```

### Use Config File

```bash
cd packages/browseros
python3 -m build.browseros build --config build/config/release.macos.yaml
```

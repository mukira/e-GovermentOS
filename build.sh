#!/bin/bash
################################################################################
# e-GovernmentOS Optimized Incremental Build Script
# 
# This script builds e-GovernmentOS with all customizations using the official
# Python build system. It supports incremental builds via ninja.
#
# Usage:
#   ./build.sh                    # Incremental build (default)
#   ./build.sh --full             # Full rebuild with setup and patches
#   ./build.sh --help             # Show help
#
# Environment Variables (optional):
#   CHROMIUM_SRC    - Path to Chromium source directory
#   BUILD_TYPE      - Build type: debug or release (default: release)
#   ARCHITECTURE    - Target architecture: arm64, x64, universal (auto-detect)
#
################################################################################

set -e  # Exit on error

#===============================================================================
# Configuration
#===============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_START_TIME=$(date +%s)

# Default configuration
DEFAULT_BUILD_TYPE="${BUILD_TYPE:-release}"

# Auto-detect architecture based on hardware
DETECTED_ARCH=$(uname -m)
if [[ "$DETECTED_ARCH" == "x86_64" ]]; then
    DEFAULT_ARCH="${ARCHITECTURE:-x64}"  # Intel Mac
elif [[ "$DETECTED_ARCH" == "arm64" ]]; then
    DEFAULT_ARCH="${ARCHITECTURE:-arm64}"  # Apple Silicon
else
    DEFAULT_ARCH="${ARCHITECTURE:-x64}"  # Fallback
fi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

#===============================================================================
# Helper Functions
#===============================================================================

log_info() {
    echo -e "${BLUE}ℹ${NC} $1"
}

log_success() {
    echo -e "${GREEN}✓${NC} $1"
}

log_error() {
    echo -e "${RED}✗${NC} $1" >&2
}

log_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

show_help() {
    cat << EOF
e-GovernmentOS Optimized Incremental Build Script

USAGE:
    ./build.sh [OPTIONS]

OPTIONS:
    --full              Full rebuild (setup + patches + build)
    --incremental       Incremental build only (default)
    --agent-only        Build only the agent extension
    --build-type TYPE   Set build type: debug or release (default: release)
    --arch ARCH         Set architecture: arm64, x64, universal (default: arm64)
    --chromium-src PATH Override Chromium source directory
    --help              Show this help message

ENVIRONMENT VARIABLES:
    CHROMIUM_SRC        Path to Chromium source directory
    BUILD_TYPE          Build type (debug or release)
    ARCHITECTURE        Target architecture

EXAMPLES:
    # Incremental build (fastest, for development)
    ./build.sh

    # Full rebuild with all patches
    ./build.sh --full

    # Debug build
    ./build.sh --build-type debug

    # Build only the agent extension
    ./build.sh --agent-only

EOF
}

#===============================================================================
# Argument Parsing
#===============================================================================

FULL_BUILD=false
INCREMENTAL_BUILD=true
AGENT_ONLY=false
CLI_BUILD_TYPE=""
CLI_ARCH=""
CLI_CHROMIUM_SRC=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --full)
            FULL_BUILD=true
            INCREMENTAL_BUILD=false
            shift
            ;;
        --incremental)
            INCREMENTAL_BUILD=true
            FULL_BUILD=false
            shift
            ;;
        --agent-only)
            AGENT_ONLY=true
            shift
            ;;
        --build-type)
            CLI_BUILD_TYPE="$2"
            shift 2
            ;;
        --arch)
            CLI_ARCH="$2"
            shift 2
            ;;
        --chromium-src)
            CLI_CHROMIUM_SRC="$2"
            shift 2
            ;;
        --help|-h)
            show_help
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Apply CLI overrides
BUILD_TYPE="${CLI_BUILD_TYPE:-$DEFAULT_BUILD_TYPE}"
ARCH="${CLI_ARCH:-$DEFAULT_ARCH}"

#===============================================================================
# Environment Validation
#===============================================================================

echo "════════════════════════════════════════════════════════════════════════"
echo "  e-GovernmentOS Optimized Build System"
echo "════════════════════════════════════════════════════════════════════════"

log_info "Validating build environment..."

# Check Python 3
if ! command -v python3 &> /dev/null; then
    log_error "Python 3 is required but not found"
    exit 1
fi
log_success "Python 3 found: $(python3 --version)"

# Detect Chromium source directory
if [[ -n "$CLI_CHROMIUM_SRC" ]]; then
    CHROMIUM_SRC="$CLI_CHROMIUM_SRC"
elif [[ -n "$CHROMIUM_SRC" ]]; then
    CHROMIUM_SRC="$CHROMIUM_SRC"
else
    # Try common locations
    POSSIBLE_LOCATIONS=(
        "$HOME/chromium/src"
        "$HOME/src/chromium/src"
        "/Users/Mukira/chromium/src"
    )
    
    for loc in "${POSSIBLE_LOCATIONS[@]}"; do
        if [[ -d "$loc" ]]; then
            CHROMIUM_SRC="$loc"
            log_warning "Auto-detected Chromium source: $CHROMIUM_SRC"
            break
        fi
    done
    
    if [[ -z "$CHROMIUM_SRC" ]]; then
        log_error "Chromium source directory not found"
        log_error "Please set CHROMIUM_SRC environment variable or use --chromium-src"
        log_error "Example: export CHROMIUM_SRC=$HOME/chromium/src"
        exit 1
    fi
fi

if [[ ! -d "$CHROMIUM_SRC" ]]; then
    log_error "Chromium source directory not found: $CHROMIUM_SRC"
    exit 1
fi
log_success "Chromium source: $CHROMIUM_SRC"

# Check depot_tools (ninja, gn should be in PATH)
if ! command -v ninja &> /dev/null && ! command -v autoninja &> /dev/null; then
    # Try to find ninja in Chromium source
    if [[ -n "$CHROMIUM_SRC" ]] && [[ -f "$CHROMIUM_SRC/third_party/ninja/ninja" ]]; then
        log_warning "depot_tools not in PATH, using Chromium's ninja"
        export PATH="$CHROMIUM_SRC/third_party/ninja:$PATH"
        
        # Also add depot_tools if it exists in common locations
        DEPOT_TOOLS_LOCATIONS=(
            "$CHROMIUM_SRC/../depot_tools"
            "$HOME/depot_tools"
        )
        for dt_path in "${DEPOT_TOOLS_LOCATIONS[@]}"; do
            if [[ -d "$dt_path" ]]; then
                export PATH="$dt_path:$PATH"
                log_info "Added depot_tools to PATH: $dt_path"
                break
            fi
        done
    else
        log_error "ninja/autoninja not found in PATH"
        log_error "Please ensure depot_tools is in your PATH"
        log_error "Or run: git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git"
        exit 1
    fi
fi
log_success "Build tools found (ninja: $(which ninja 2>/dev/null || echo 'via chromium'))"

# Check if we're in the BrowserOS repo
if [[ ! -d "$SCRIPT_DIR/packages/browseros" ]]; then
    log_error "This script must be run from the BrowserOS repository root"
    exit 1
fi
log_success "BrowserOS repository: $SCRIPT_DIR"

#===============================================================================
# Build Configuration
#===============================================================================

log_info "Build configuration:"
echo "  • Build type:   $BUILD_TYPE"
echo "  • Architecture: $ARCH"
echo "  • Chromium src: $CHROMIUM_SRC"
echo "  • Mode:         $([ "$FULL_BUILD" = true ] && echo "FULL REBUILD" || echo "INCREMENTAL")"

#===============================================================================
# Build BrowserOS-server
#===============================================================================

build_server() {
    log_info "Building e-GovernmentOS-server..."
    
    SERVER_DIR="$SCRIPT_DIR/BrowserOS-server"
    
    # Check if server directory exists
    if [[ ! -d "$SERVER_DIR" ]]; then
        log_warning "e-GovernmentOS-server not found at: $SERVER_DIR"
        log_info "Cloning BrowserOS-server repository..."
        git clone https://github.com/browseros-ai/BrowserOS-server.git "$SERVER_DIR"
    fi
    
    cd "$SERVER_DIR"
    
    # Check for bun
    if ! command -v bun &> /dev/null; then
        log_error "Bun is required for e-GovernmentOS-server"
        log_error "Install from: https://bun.sh"
        return 1
    fi
    
    # Install dependencies if needed
    if [[ ! -d "node_modules" ]] || [[ "package.json" -nt "node_modules" ]]; then
        log_info "Installing server dependencies..."
        bun install
    fi
    
    # Create .env.dev if it doesn't exist
    if [[ ! -f ".env.dev" ]]; then
        log_info "Creating server configuration (.env.dev)..."
        cat > .env.dev << 'EOF'
# e-GovernmentOS Server Configuration
HTTP_MCP_PORT=9223
AGENT_PORT=3000
EXTENSION_PORT=9224
MAX_SESSIONS=5
SESSION_IDLE_TIMEOUT_MS=90000
LOG_LEVEL=info
NODE_ENV=development
EOF
    fi
    
    # For development, we run from source instead of building a binary
    # This avoids bundler issues with WASM and native modules
    log_info "Server configured to run from source (better for development)"
    log_success "Server setup complete: $SERVER_DIR"
    log_info "The server will run from source using 'bun start'"
    
    cd "$SCRIPT_DIR"
}

#===============================================================================
# Build Agent Extension
#===============================================================================

build_agent() {
    log_info "Building e-GovernmentOS Agent extension..."
    
    AGENT_DIR="$SCRIPT_DIR/packages/browseros-agent"
    
    # Check if agent submodule is initialized
    if [[ ! -d "$AGENT_DIR" ]] || [[ ! -f "$AGENT_DIR/package.json" ]]; then
        log_warning "Agent submodule not initialized, initializing..."
        cd "$SCRIPT_DIR"
        git submodule update --init --recursive packages/browseros-agent
    fi
    
    # Check if agent directory exists after init
    if [[ ! -d "$AGENT_DIR" ]]; then
        log_error "Agent directory not found: $AGENT_DIR"
        log_error "Please run: git submodule update --init --recursive"
        return 1
    fi
    
    cd "$AGENT_DIR"
    
    # Install dependencies if needed
    if [[ ! -d "node_modules" ]] || [[ "package.json" -nt "node_modules" ]]; then
        log_info "Installing agent dependencies..."
        if command -v yarn &> /dev/null; then
            yarn install
        elif command -v npm &> /dev/null; then
            npm install
        else
            log_error "Neither yarn nor npm found. Please install Node.js and yarn/npm"
            return 1
        fi
    fi
    
    # Build agent
    log_info "Compiling agent extension..."
    if command -v yarn &> /dev/null; then
        yarn build
    else
        npm run build
    fi
    
    if [[ -d "dist" ]]; then
        log_success "Agent built successfully: $AGENT_DIR/dist"
    else
        log_error "Agent build failed - dist directory not created"
        return 1
    fi
    
    cd "$SCRIPT_DIR"
}

#===============================================================================
# Build BrowserOS (Chromium)
#===============================================================================

build_browseros() {
    log_info "Building e-GovernmentOS Chromium..."
    
    cd "$SCRIPT_DIR/packages/browseros"
    
    # Check if build is already configured
    # Capitalize build type (debug -> Debug, release -> Release)
    BUILD_TYPE_CAP="$(tr '[:lower:]' '[:upper:]' <<< "${BUILD_TYPE:0:1}")${BUILD_TYPE:1}"
    OUT_DIR="$CHROMIUM_SRC/out/$BUILD_TYPE_CAP"
    ARGS_FILE="$OUT_DIR/args.gn"
    
    if [[ "$FULL_BUILD" = true ]]; then
        log_info "Running FULL BUILD (setup + prep + build)..."
        
        # Full build pipeline
        python3 -m build.browseros build \
            --setup \
            --prep \
            --build \
            --build-type "$BUILD_TYPE" \
            --arch "$ARCH" \
            --chromium-src "$CHROMIUM_SRC"
    else
        # Incremental build
        if [[ -f "$ARGS_FILE" ]]; then
            log_info "Configuration exists, running INCREMENTAL BUILD..."
            log_info "Using existing configuration: $ARGS_FILE"
            
            # Only run compile phase
            python3 -m build.browseros build \
                --build \
                --build-type "$BUILD_TYPE" \
                --arch "$ARCH" \
                --chromium-src "$CHROMIUM_SRC"
        else
            log_warning "No existing configuration found, running initial setup..."
            log_info "This will take longer (setting up + applying patches + building)..."
            
            # First time build - need setup and prep
            python3 -m build.browseros build \
                --setup \
                --prep \
                --build \
                --build-type "$BUILD_TYPE" \
                --arch "$ARCH" \
                --chromium-src "$CHROMIUM_SRC"
        fi
    fi
    
    cd "$SCRIPT_DIR"
    
    # Verify build output
    if [[ -d "$OUT_DIR/BrowserOS.app" ]] || [[ -f "$OUT_DIR/chrome" ]]; then
        log_success "e-GovernmentOS built successfully!"
        log_info "Output directory: $OUT_DIR"
    else
        log_error "Build completed but output not found in: $OUT_DIR"
        return 1
    fi
}

#===============================================================================
# Main Build Process
#===============================================================================

main() {
    echo ""
    log_info "Starting build process..."
    echo ""
    
    # Build based on mode
    if [[ "$AGENT_ONLY" = true ]]; then
        # Only build agent
        build_agent
    else
        # Build all components: server, agent, and browser
        
        # 1. Build server (fastest, does not depend on others)
        log_info "Building BrowserOS-server..."
        build_server || {
            log_warning "Server build failed, continuing..."
        }
        
        # 2. Build agent extension
        log_info "Building BrowserOS-agent extension..."
        build_agent || {
            log_warning "Agent build failed, continuing..."
        }
        
        # 3. Build browser (slowest, can take minutes to hours)
        log_info "Building BrowserOS browser..."
        build_browseros || {
            log_error "BrowserOS build failed"
            exit 1
        }
    fi
    
    # Calculate build time
    BUILD_END_TIME=$(date +%s)
    BUILD_DURATION=$((BUILD_END_TIME - BUILD_START_TIME))
    MINS=$((BUILD_DURATION / 60))
    SECS=$((BUILD_DURATION % 60))
    
    echo ""
    echo "════════════════════════════════════════════════════════════════════════"
    log_success "Build completed successfully in ${MINS}m ${SECS}s"
    echo "════════════════════════════════════════════════════════════════════════"
    echo ""
    
    # Show output locations
    if [[ "$AGENT_ONLY" != true ]]; then
        # Capitalize build type for output path
        BUILD_TYPE_CAP="$(tr '[:lower:]' '[:upper:]' <<< "${BUILD_TYPE:0:1}")${BUILD_TYPE:1}"
        
        log_info "Build outputs:"
        echo "  • Server:  $SCRIPT_DIR/BrowserOS-server (runs from source)"
        echo "  • Agent:   $SCRIPT_DIR/packages/browseros-agent/dist"
        echo "  • Browser: $CHROMIUM_SRC/out/$BUILD_TYPE_CAP"
        echo ""
        log_info "To run the complete BrowserOS stack:"
        echo "  ./launch-browseros.sh"
        echo ""
        log_info "Or run components individually:"
        echo "  • Server:  cd BrowserOS-server && bun start"
        
        if [[ -d "$CHROMIUM_SRC/out/$BUILD_TYPE_CAP/BrowserOS.app" ]]; then
            echo "  • Browser: $CHROMIUM_SRC/out/$BUILD_TYPE_CAP/BrowserOS.app/Contents/MacOS/BrowserOS"
        elif [[ -f "$CHROMIUM_SRC/out/$BUILD_TYPE_CAP/chrome" ]]; then
            echo "  • Browser: $CHROMIUM_SRC/out/$BUILD_TYPE_CAP/chrome"
        fi
    else
        log_info "Agent output:"
        echo "  • $SCRIPT_DIR/packages/browseros-agent/dist"
    fi
}

# Run main
main

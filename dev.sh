#!/bin/bash
################################################################################
# BrowserOS Development Mode with Hot Reload
#
# Watches for file changes and automatically rebuilds/reloads components
# - Agent extension: Auto-rebuild on file changes
# - Browser resources: Auto-copy to output directory
# - Server: Auto-restart on code changes
#
# Usage:
#   ./dev.sh           # Start all watchers
#   ./dev.sh --agent   # Watch agent only
#   ./dev.sh --server  # Watch server only
################################################################################

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
AGENT_DIR="$SCRIPT_DIR/packages/browseros-agent"
SERVER_DIR="$SCRIPT_DIR/BrowserOS-server"
BROWSER_OUT="/Users/Mukira/chromium/src/out/Release"

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}ℹ${NC} $1"
}

log_success() {
    echo -e "${GREEN}✓${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

# Function to watch and rebuild agent
watch_agent() {
    log_info "Starting agent hot-reload watcher..."
    cd "$AGENT_DIR"
    
    # Use webpack watch mode for auto-rebuild
    yarn watch &
    AGENT_WATCHER_PID=$!
    
    log_success "Agent watcher started (PID: $AGENT_WATCHER_PID)"
    log_info "Any changes to agent code will auto-rebuild"
    log_warning "Reload extension in chrome://extensions after changes"
    
    echo $AGENT_WATCHER_PID > "$SCRIPT_DIR/.agent-watcher.pid"
}

# Function to watch and restart server
watch_server() {
    log_info "Starting server hot-reload watcher..."
    cd "$SERVER_DIR"
    
    # Use bun with --watch flag for auto-restart
    bun --watch start &
    SERVER_WATCHER_PID=$!
    
    log_success "Server watcher started (PID: $SERVER_WATCHER_PID)"
    log_info "Server will auto-restart on code changes"
    
    echo $SERVER_WATCHER_PID > "$SCRIPT_DIR/.server-watcher.pid"
}

# Function to watch browser resources (logos, icons, etc.)
watch_browser_resources() {
    log_info "Starting browser resources watcher..."
    
    # Watch for changes in BrowserOS resource directories
    RESOURCE_DIRS=(
        "$SCRIPT_DIR/packages/browseros/resources"
        "$SCRIPT_DIR/packages/browseros/app"
    )
    
    # Install fswatch if not available
    if ! command -v fswatch &> /dev/null; then
        log_warning "fswatch not installed. Install with: brew install fswatch"
        return 1
    fi
    
    # Watch and copy resources on change
    (
        while true; do
            # Use fswatch to monitor directories
            for dir in "${RESOURCE_DIRS[@]}"; do
                if [[ -d "$dir" ]]; then
                    fswatch -1 "$dir" && {
                        log_info "Resource changed, copying to output..."
                        # Trigger resource copy (part of build process)
                        cd "$SCRIPT_DIR/packages/browseros"
                        python3 -m build.browseros build --prep-only
                        log_success "Resources updated! Restart browser to see changes."
                    }
                fi
            done
        done
    ) &
    
    RESOURCE_WATCHER_PID=$!
    echo $RESOURCE_WATCHER_PID > "$SCRIPT_DIR/.resource-watcher.pid"
    log_success "Resource watcher started (PID: $RESOURCE_WATCHER_PID)"
}

# Cleanup function
cleanup() {
    log_info "Stopping watchers..."
    
    [[ -f "$SCRIPT_DIR/.agent-watcher.pid" ]] && {
        kill $(cat "$SCRIPT_DIR/.agent-watcher.pid") 2>/dev/null || true
        rm "$SCRIPT_DIR/.agent-watcher.pid"
    }
    
    [[ -f "$SCRIPT_DIR/.server-watcher.pid" ]] && {
        kill $(cat "$SCRIPT_DIR/.server-watcher.pid") 2>/dev/null || true
        rm "$SCRIPT_DIR/.server-watcher.pid"
    }
    
    [[ -f "$SCRIPT_DIR/.resource-watcher.pid" ]] && {
        kill $(cat "$SCRIPT_DIR/.resource-watcher.pid") 2>/dev/null || true
        rm "$SCRIPT_DIR/.resource-watcher.pid"
    }
    
    log_success "All watchers stopped"
    exit 0
}

trap cleanup SIGINT SIGTERM

# Parse arguments
WATCH_AGENT=true
WATCH_SERVER=true
WATCH_RESOURCES=false

if [[ "$1" == "--agent" ]]; then
    WATCH_SERVER=false
    WATCH_RESOURCES=false
elif [[ "$1" == "--server" ]]; then
    WATCH_AGENT=false
    WATCH_RESOURCES=false
elif [[ "$1" == "--resources" ]]; then
    WATCH_AGENT=false
    WATCH_SERVER=false
    WATCH_RESOURCES=true
fi

echo "════════════════════════════════════════════════════════════════════════"
echo "  BrowserOS Development Mode - Hot Reload"
echo "════════════════════════════════════════════════════════════════════════"
echo ""

# Start watchers
[[ "$WATCH_AGENT" == true ]] && watch_agent
[[ "$WATCH_SERVER" == true ]] && watch_server
[[ "$WATCH_RESOURCES" == true ]] && watch_browser_resources

echo ""
echo "════════════════════════════════════════════════════════════════════════"
log_success "Development watchers are running!"
echo "════════════════════════════════════════════════════════════════════════"
echo ""
log_info "What's being watched:"
[[ "$WATCH_AGENT" == true ]] && echo "  • Agent:     Auto-rebuilds on changes"
[[ "$WATCH_SERVER" == true ]] && echo "  • Server:    Auto-restarts on changes"
[[ "$WATCH_RESOURCES" == true ]] && echo "  • Resources: Auto-copies on changes"
echo ""
log_info "Press Ctrl+C to stop all watchers"
echo ""

# Keep script running
wait

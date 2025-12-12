#!/bin/bash
################################################################################
# e-GovernmentOS Fresh Profile + Hot Reload Development
#
# Launches e-GovernmentOS with a clean profile AND enables hot-reload for:
# - Agent extension (auto-rebuild on changes)
# - Server (auto-restart on changes)
#
# Perfect for testing the first-time user experience while developing!
#
# Usage:
#   ./dev-fresh.sh
################################################################################

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROFILE_DIR="$HOME/Library/Application Support/e-GovernmentOS"
AGENT_DIR="$SCRIPT_DIR/packages/e-governmentos-agent"
SERVER_DIR="$SCRIPT_DIR/e-GovernmentOS-server"

# Auto-detect the correct e-GovernmentOS build directory
if [[ -d "/Users/Mukira/chromium/src/out/Default_x64/e-GovernmentOS.app" ]]; then
    BROWSER_BINARY="/Users/Mukira/chromium/src/out/Default_x64/e-GovernmentOS.app/Contents/MacOS/e-GovernmentOS"
elif [[ -d "/Users/Mukira/chromium/src/out/Release/e-GovernmentOS.app" ]]; then
    BROWSER_BINARY="/Users/Mukira/chromium/src/out/Release/e-GovernmentOS.app/Contents/MacOS/e-GovernmentOS"
    echo "Please run ./build.sh first"
    exit 1
fi

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
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

log_error() {
    echo -e "${RED}✗${NC} $1"
}

cleanup() {
    log_info "Cleaning up..."
    
    # Kill watchers
    [[ -f "$SCRIPT_DIR/.agent-watcher.pid" ]] && {
        kill $(cat "$SCRIPT_DIR/.agent-watcher.pid") 2>/dev/null || true
        rm "$SCRIPT_DIR/.agent-watcher.pid"
    }
    
    [[ -f "$SCRIPT_DIR/.server-watcher.pid" ]] && {
        kill $(cat "$SCRIPT_DIR/.server-watcher.pid") 2>/dev/null || true
        rm "$SCRIPT_DIR/.server-watcher.pid"
    }
    
    # Stop server
    ./launch-e-governmentos.sh --stop 2>/dev/null || true
    
    log_success "Cleanup complete"
    exit 0
}

trap cleanup SIGINT SIGTERM

echo "════════════════════════════════════════════════════════════════════════"
echo "  e-GovernmentOS Fresh Profile + Hot Reload Development"
echo "════════════════════════════════════════════════════════════════════════"
echo ""

# Step 1: Stop everything
log_info "Stopping any running instances..."
pkill -f "e-GovernmentOS.app" 2>/dev/null || true
./launch-e-governmentos.sh --stop 2>/dev/null || true
sleep 2

# Step 2: Delete existing profile
if [[ -d "$PROFILE_DIR" ]]; then
    log_warning "Deleting existing profile for fresh start..."
    rm -rf "$PROFILE_DIR"
    log_success "Profile deleted"
else
    log_info "No existing profile found"
fi

# Step 3: Start agent watcher
log_info "Starting agent hot-reload watcher..."
cd "$AGENT_DIR"
yarn watch > /dev/null 2>&1 &
AGENT_WATCHER_PID=$!
echo $AGENT_WATCHER_PID > "$SCRIPT_DIR/.agent-watcher.pid"
log_success "Agent watcher started (PID: $AGENT_WATCHER_PID)"
cd "$SCRIPT_DIR"

# Step 4: Start server watcher
log_info "Starting server with auto-restart..."
cd "$SERVER_DIR"
bun --watch start > "$SCRIPT_DIR/e-governmentos-server.log" 2>&1 &
SERVER_WATCHER_PID=$!
echo $SERVER_WATCHER_PID > "$SCRIPT_DIR/.server-watcher.pid"
log_success "Server started (PID: $SERVER_WATCHER_PID)"
cd "$SCRIPT_DIR"

# Wait for server to be ready
log_info "Waiting for server to be ready..."
for i in {1..30}; do
    if lsof -Pi :9223 -sTCP:LISTEN -t >/dev/null 2>&1; then
        log_success "Server is ready on port 9223"
        break
    fi
    sleep 1
done

# Step 5: Launch browser with fresh profile
log_info "Launching e-GovernmentOS with fresh profile..."
log_info "Profile location: $PROFILE_DIR"

"$BROWSER_BINARY" \
    --user-data-dir="$PROFILE_DIR" \
    --load-extension="$AGENT_DIST" \
    --no-first-run \
    --no-default-browser-check &

BROWSER_PID=$!

echo ""
echo "════════════════════════════════════════════════════════════════════════"
log_success "e-GovernmentOS Fresh Development Environment Running!"
echo "════════════════════════════════════════════════════════════════════════"
echo ""
log_info "What's active:"
echo "  • Fresh Profile:     $PROFILE_DIR"
echo "  • Agent Hot-Reload:  Auto-rebuilds on file changes"
echo "  • Server Auto-Restart: Restarts on code changes"
echo "  • Browser:           Running (PID: $BROWSER_PID)"
echo ""
log_info "Development workflow:"
echo "  1. Edit agent code → Auto-rebuilds in ~5s"
echo "  2. Reload extension at chrome://extensions"
echo "  3. Edit server code → Auto-restarts immediately"
echo ""
log_warning "Profile will be deleted on next run!"
log_info "Perfect for testing first-time user experience"
echo ""
log_info "Server logs: tail -f $SCRIPT_DIR/e-governmentos-server.log"
log_info "Press Ctrl+C to stop all components"
echo ""

# Keep script running
wait

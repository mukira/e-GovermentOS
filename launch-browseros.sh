#!/bin/bash
################################################################################
# e-GovernmentOS Unified Launcher
#
# Starts all BrowserOS components together:
# 1. BrowserOS-server (MCP server for browser automation)
# 2. e-GovernmentOS browser with Browser OS Agent extension loaded
#
# Usage:
#   ./launch-browseros.sh              # Start everything
#   ./launch-browseros.sh --help       # Show help
#
################################################################################

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Paths
SERVER_DIR="$SCRIPT_DIR/BrowserOS-server"
AGENT_DIR="$SCRIPT_DIR/BrowserOS-agent/dist"
BROWSER_APP="/Users/Mukira/chromium/src/out/Release/BrowserOS.app"
BROWSER_BINARY="$BROWSER_APP/Contents/MacOS/BrowserOS"

# Server ports
SERVER_LOG="$SCRIPT_DIR/browseros-server.log"
SERVER_PID_FILE="$SCRIPT_DIR/browseros-server.pid"

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
e-GovernmentOS Unified Launcher

Starts e-GovernmentOS browser with Browser OS Agent extension and MCP server.

USAGE:
    ./launch-browseros.sh [OPTIONS]

OPTIONS:
    --server-only       Start only the server (no browser)
    --browser-only      Start only the browser (assumes server running)
    --stop              Stop the server
    --status            Check server status
    --help              Show this help

EXAMPLES:
    # Start everything (recommended)
    ./launch-browseros.sh

    # Start server in background, browser in foreground
    ./launch-browseros.sh --server-only &
    sleep 2
    ./launch-browseros.sh --browser-only

    # Stop the server
    ./launch-browseros.sh --stop

EOF
}

check_dependencies() {
    log_info "Checking dependencies..."
    
    # Check bun
    if ! command -v bun &> /dev/null; then
        log_error "Bun is required for BrowserOS-server"
        log_error "Install from: https://bun.sh"
        exit 1
    fi
    log_success "Bun found: $(bun --version)"
    
    # Check server directory
    if [[ ! -d "$SERVER_DIR" ]]; then
        log_error "BrowserOS-server not found at: $SERVER_DIR"
        log_error "Run: git clone https://github.com/browseros-ai/BrowserOS-server.git"
        exit 1
    fi
    log_success "Server directory found"
    
    # Check agent extension
    if [[ ! -d "$AGENT_DIR" ]]; then
        log_warning "Agent extension not built at: $AGENT_DIR"
        log_info "Building agent extension..."
        cd "$SCRIPT_DIR/packages/browseros-agent"
        if command -v yarn &> /dev/null; then
            yarn install && yarn build
        else
            npm install && npm run build
        fi
        cd "$SCRIPT_DIR"
    fi
    log_success "Agent extension ready"
    
    # Check browser binary
    if [[ ! -f "$BROWSER_BINARY" ]]; then
        log_error "e-GovernmentOS browser not found at: $BROWSER_BINARY"
        log_error "Run: ./build.sh"
        exit 1
    fi
    log_success "e-GovernmentOS browser found"
}

start_server() {
    log_info "Starting BrowserOS-server..."
    
    cd "$SERVER_DIR"
    
    # Install dependencies if needed
    if [[ ! -d "node_modules" ]]; then
        log_info "Installing server dependencies..."
        bun install
    fi
    
    # Start server in background
    nohup bun start > "$SERVER_LOG" 2>&1 &
    SERVER_PID=$!
    echo $SERVER_PID > "$SERVER_PID_FILE"
    
    log_success "Server started (PID: $SERVER_PID)"
    log_info "Waiting for server to be ready..."
    
    # Wait for server to start (check for port 9100 - MCP)
    for i in {1..30}; do
        if lsof -Pi :9100 -sTCP:LISTEN -t >/dev/null 2>&1; then
            log_success "Server is ready on port 9100"
            return 0
        fi
        sleep 1
    done
    
    log_error "Server failed to start within 30 seconds"
    log_error "Check logs at: $SERVER_LOG"
    return 1
}

stop_server() {
    if [[ -f "$SERVER_PID_FILE" ]]; then
        PID=$(cat "$SERVER_PID_FILE")
        if ps -p $PID > /dev/null 2>&1; then
            log_info "Stopping server (PID: $PID)..."
            kill $PID
            rm "$SERVER_PID_FILE"
            log_success "Server stopped"
        else
            log_warning "Server not running (stale PID file)"
            rm "$SERVER_PID_FILE"
        fi
    else
        log_warning "No server PID file found"
    fi
}

check_server_status() {
    if [[ -f "$SERVER_PID_FILE" ]]; then
        PID=$(cat "$SERVER_PID_FILE")
        if ps -p $PID > /dev/null 2>&1; then
            log_success "Server is running (PID: $PID)"
            if lsof -Pi :9100 -sTCP:LISTEN -t >/dev/null 2>&1; then
                log_success "HTTP MCP server listening on port 9100"
            fi
            if lsof -Pi :9200 -sTCP:LISTEN -t >/dev/null 2>&1; then
                log_success "Agent server listening on port 9200"
            fi
            if lsof -Pi :9300 -sTCP:LISTEN -t >/dev/null 2>&1; then
                log_success "Extension WebSocket listening on port 9300"
            fi
            return 0
        else
            log_warning "Server not running (stale PID file)"
            rm "$SERVER_PID_FILE"
            return 1
        fi
    else
        log_warning "Server is not running"
        return 1
    fi
}

start_browser() {
    log_info "Starting e-GovernmentOS with Browser OS Agent extension..."
    
    # Create temp profile for clean start
    PROFILE_DIR="$HOME/Library/Application Support/BrowserOS"
    
    log_info "Profile directory: $PROFILE_DIR"
    log_info "Agent extension: $AGENT_DIR"
    
    # Launch e-GovernmentOS with extension
    # Launch e-GovernmentOS (verbose but filtered)
    # Filter out known benign errors (Lock file, SSL, NewTabPage context)
    "$BROWSER_BINARY" \
        --user-data-dir="$PROFILE_DIR" \
        --load-extension="$AGENT_DIR" \
        --enable-logging --v=1 \
        --allow-insecure-localhost \
        --no-default-browser-check \
        2>&1 | grep -v "Failed to open lock file" \
             | grep -v "Error parsing certificate" \
             | grep -v "NewTabPage loaded into" &
        
    BROWSER_PID=$!
    log_success "e-GovernmentOS launched (PID: $BROWSER_PID)"
    # log_info "Browser logs hidden..."
}

cleanup() {
    log_info "Cleaning up..."
    stop_server
    exit 0
}

#===============================================================================
# Main
#===============================================================================

# Parse arguments
SERVER_ONLY=false
BROWSER_ONLY=false
STOP_SERVER=false
CHECK_STATUS=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --server-only)
            SERVER_ONLY=true
            shift
            ;;
        --browser-only)
            BROWSER_ONLY=true
            shift
            ;;
        --stop)
            STOP_SERVER=true
            shift
            ;;
        --status)
            CHECK_STATUS=true
            shift
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

# Handle stop command
if [[ "$STOP_SERVER" = true ]]; then
    stop_server
    exit 0
fi

# Handle status command
if [[ "$CHECK_STATUS" = true ]]; then
    check_server_status
    exit $?
fi

# Trap Ctrl+C for cleanup
trap cleanup SIGINT SIGTERM

echo "════════════════════════════════════════════════════════════════════════"
echo "  e-GovernmentOS Unified Launcher"
echo "════════════════════════════════════════════════════════════════════════"
echo ""

check_dependencies

if [[ "$BROWSER_ONLY" = true ]]; then
    # Only start browser
    start_browser
    log_info "Browser started. Press Ctrl+C to exit."
    wait
elif [[ "$SERVER_ONLY" = true ]]; then
    # Only start server
    start_server
    log_info "Server started. Press Ctrl+C to stop."
    log_info "Logs: tail -f $SERVER_LOG"
    wait
else
    # Start both
    start_server || exit 1
    sleep 2
    start_browser
    
    echo ""
    echo "════════════════════════════════════════════════════════════════════════"
    log_success "e-GovernmentOS stack is running!"
    echo "════════════════════════════════════════════════════════════════════════"
    echo ""
    log_info "Components:"
    echo "  • Server:  http://localhost:9100 (MCP)"
    echo "  • Agent:   http://localhost:9200"
    echo "  • Browser: Running with extension loaded"
    echo ""
    log_info "Server logs: tail -f $SERVER_LOG"
    log_info "Press Ctrl+C to stop all components"
    echo ""
    
    # Wait for user interrupt
    wait
fi

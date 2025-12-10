# BrowserOS Unified Stack

This document explains how all BrowserOS components work together.

## Components

### 1. **BrowserOS Browser** (Chromium + Patches)
- Location: `/Users/Mukira/chromium/src/out/Release/BrowserOS.app`
- What: Custom Chromium build with BrowserOS branding and features
- Built with: `./build.sh`

### 2. **BrowserOS-agent Extension**
- Location: `packages/browseros-agent/dist/`
- What: Chrome extension providing AI agent UI and browser control
- Built with: `cd packages/browseros-agent && yarn build`

### 3. **BrowserOS-server** (MCP Server)
- Location: `BrowserOS-server/`
- What: Server providing browser automation tools via MCP protocol
- Runs with: `bun start`

## How They Connect

```
User runs ./launch-browseros.sh
         │
         ├─► Starts BrowserOS-server
         │   ├─ HTTP MCP Server (port 9223)
         │   ├─ Agent Server (port 3000)  
         │   └─ WebSocket Manager (port 9224)
         │
         └─► Starts BrowserOS browser
             └─ Loads agent extension
                └─ Connects to server via WebSocket (port 9224)
```

## Data Flow

1. **AI Agent wants to control browser**
   - Sends request to BrowserOS-server (port 9223 or 3000)
   
2. **Server executes browser automation**
   - Uses WebSocket (port 9224) to communicate with agent extension
   - Extension uses Chrome APIs to control the browser
   
3. **Results flow back**
   - Extension sends results to server
   - Server returns results to AI agent

## Quick Start

### Single Command (Recommended)

```bash
./launch-browseros.sh
```

This starts:
- ✅ BrowserOS-server in background
- ✅ BrowserOS browser with extension loaded
- ✅ All connections configured automatically

### Advanced Usage

```bash
# Start only server (background)
./launch-browseros.sh --server-only

# Start only browser (assumes server running)
./launch-browseros.sh --browser-only

# Check server status
./launch-browseros.sh --status

# Stop server
./launch-browseros.sh --stop
```

## Development Workflow

### Building Everything

```bash
# 1. Build browser (first time or after source changes)
./build.sh

# 2. Build agent extension (after extension changes)
cd packages/browseros-agent
yarn build
cd ../..

# 3. Build server binary (optional, for distribution)
cd BrowserOS-server
bun run dev:server:macos
cd ..

# 4. Launch everything
./launch-browseros.sh
```

### Incremental Development

For daily development:

```bash
# Browser code changes
./build.sh  # Incremental, only rebuilds changed files

# Agent extension changes
cd packages/browseros-agent && yarn build && cd ../..

# Server code changes (no build needed, just restart)
./launch-browseros.sh --stop
./launch-browseros.sh
```

## Ports Reference

| Component | Port | Protocol | Purpose |
|-----------|------|----------|---------|
| HTTP MCP Server | 9223 | HTTP/SSE | External MCP clients |
| Agent Server | 3000 | WebSocket | Claude SDK integration |
| Extension WebSocket | 9224 | WebSocket | Browser extension connection |

## Environment Variables

### For BrowserOS-server

Create `BrowserOS-server/.env.dev`:

```bash
# Required for AI agent functionality
ANTHROPIC_API_KEY=your_key_here

# Optional
MAX_SESSIONS=5
SESSION_IDLE_TIMEOUT_MS=90000
LOG_LEVEL=info
```

### For Browser (optional)

```bash
# To use a different Chromium build
export CHROMIUM_SRC=/path/to/chromium/src

# To change build type
export BUILD_TYPE=debug
```

## Troubleshooting

### Server won't start

```bash
# Check if ports are in use
lsof -i :9223
lsof -i :3000
lsof -i :9224

# View server logs
tail -f browseros-server.log

# Check dependencies
cd BrowserOS-server
bun install
```

### Extension not loading

```bash
# Rebuild extension
cd packages/browseros-agent
rm -rf dist node_modules
yarn install
yarn build

# Check extension directory exists
ls -la packages/browseros-agent/dist/
```

### Browser not connecting to server

1. Make sure server is running: `./launch-browseros.sh --status`
2. Check server logs: `tail -f browseros-server.log`
3. Verify extension loaded in browser: `chrome://extensions`
4. Check WebSocket connection in browser console

## Architecture Deep Dive

### Why This Design?

**Separation of Concerns:**
- **Browser**: Focus on rendering and core browser features
- **Extension**: UI and browser API integration
- **Server**: AI agent tools and automation logic

**Benefits:**
- Server can run independently (use with any Chrome/Chromium)
- Extension works with regular Chrome for development
- Browser can be used without AI features
- All components can be developed/tested separately

### Communication Protocol

**Extension ↔ Server:**
- WebSocket for real-time bidirectional communication
- JSON-RPC style messages
- Server sends commands, extension executes and returns results

**AI Agent ↔ Server:**
- HTTP/SSE for MCP clients (external AI tools)
- WebSocket for embedded Claude SDK agent
- Standard MCP protocol for tool calls

## Production Deployment

For distributing BrowserOS:

```bash
# Build production server binaries
cd BrowserOS-server
bun run dist:server

# Build production extension
bun run dist:ext

# Browser already built for release
# (built with --build-type release)
```

Distribute:
- `BrowserOS.app` (or .exe on Windows)
- `BrowserOS-server` binary
- Pre-configured launcher script

Users just run: `./launch-browseros.sh`

# e-GovernmentOS Unified Stack

This document explains how all e-GovernmentOS components work together.

## Components

### 1. **e-GovernmentOS Browser** (Chromium + Patches)
- Location: `/Users/Mukira/chromium/src/out/Release/e-GovernmentOS.app`
- What: Custom Chromium build with e-GovernmentOS branding and features
- Built with: `./build.sh`

### 2. **e-GovernmentOS-agent Extension**
- Location: `packages/e-governmentos-agent/dist/`
- What: Chrome extension providing AI agent UI and browser control
- Built with: `cd packages/e-governmentos-agent && yarn build`

### 3. **e-GovernmentOS-server** (MCP Server)
- Location: `e-GovernmentOS-server/`
- What: Server providing browser automation tools via MCP protocol
- Runs with: `bun start`

## How They Connect

```
User runs ./launch-e-governmentos.sh
         │
         ├─► Starts e-GovernmentOS-server
         │   ├─ HTTP MCP Server (port 9223)
         │   ├─ Agent Server (port 3000)  
         │   └─ WebSocket Manager (port 9224)
         │
         └─► Starts e-GovernmentOS browser
             └─ Loads agent extension
                └─ Connects to server via WebSocket (port 9224)
```

## Data Flow

1. **AI Agent wants to control browser**
   - Sends request to e-GovernmentOS-server (port 9223 or 3000)
   
2. **Server executes browser automation**
   - Uses WebSocket (port 9224) to communicate with agent extension
   - Extension uses Chrome APIs to control the browser
   
3. **Results flow back**
   - Extension sends results to server
   - Server returns results to AI agent

## Quick Start

### Single Command (Recommended)

```bash
./launch-e-governmentos.sh
```

This starts:
- ✅ e-GovernmentOS-server in background
- ✅ e-GovernmentOS browser with extension loaded
- ✅ All connections configured automatically

### Advanced Usage

```bash
# Start only server (background)
./launch-e-governmentos.sh --server-only

# Start only browser (assumes server running)
./launch-e-governmentos.sh --browser-only

# Check server status
./launch-e-governmentos.sh --status

# Stop server
./launch-e-governmentos.sh --stop
```

## Development Workflow

### Building Everything

```bash
# 1. Build browser (first time or after source changes)
./build.sh

# 2. Build agent extension (after extension changes)
cd packages/e-governmentos-agent
yarn build
cd ../..

# 3. Build server binary (optional, for distribution)
cd e-GovernmentOS-server
bun run dev:server:macos
cd ..

# 4. Launch everything
./launch-e-governmentos.sh
```

### Incremental Development

For daily development:

```bash
# Browser code changes
./build.sh  # Incremental, only rebuilds changed files

# Agent extension changes
cd packages/e-governmentos-agent && yarn build && cd ../..

# Server code changes (no build needed, just restart)
./launch-e-governmentos.sh --stop
./launch-e-governmentos.sh
```

## Ports Reference

| Component | Port | Protocol | Purpose |
|-----------|------|----------|---------|
| HTTP MCP Server | 9223 | HTTP/SSE | External MCP clients |
| Agent Server | 3000 | WebSocket | Claude SDK integration |
| Extension WebSocket | 9224 | WebSocket | Browser extension connection |

## Environment Variables

### For e-GovernmentOS-server

Create `e-GovernmentOS-server/.env.dev`:

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
tail -f e-governmentos-server.log

# Check dependencies
cd e-GovernmentOS-server
bun install
```

### Extension not loading

```bash
# Rebuild extension
cd packages/e-governmentos-agent
rm -rf dist node_modules
yarn install
yarn build

# Check extension directory exists
ls -la packages/e-governmentos-agent/dist/
```

### Browser not connecting to server

1. Make sure server is running: `./launch-e-governmentos.sh --status`
2. Check server logs: `tail -f e-governmentos-server.log`
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

For distributing e-GovernmentOS:

```bash
# Build production server binaries
cd e-GovernmentOS-server
bun run dist:server

# Build production extension
bun run dist:ext

# Browser already built for release
# (built with --build-type release)
```

Distribute:
- `e-GovernmentOS.app` (or .exe on Windows)
- `e-GovernmentOS-server` binary
- Pre-configured launcher script

Users just run: `./launch-e-governmentos.sh`

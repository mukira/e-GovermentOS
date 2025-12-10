# Hot-Reload Development Guide

Quick reference for seeing changes immediately during development.

## The Problem

When you change a logo or UI element, you don't want to wait for full rebuilds. You want **instant feedback**.

## The Solution: Component-Specific Workflows

### 🎨 For Agent UI Changes (Logos, Styles, Components)

**What:** Agent extension UI, logos, panels, styles
**Time:** ~5 seconds per change

```bash
# Terminal 1: Start watch mode
cd packages/browseros-agent
yarn watch

# Now: Edit files and see instant recompiles!
# Changes trigger automatic rebuild
```

**To see changes:**
1. Make your change (edit logo, CSS, component)
2. Wait 5 seconds (auto-rebuild)
3. Go to `chrome://extensions`
4. Click reload button on BrowserOS extension
5. Refresh the page you're testing

### 🖥️ For Browser Resources (App Icon, Branding)  

**What:** BrowserOS.app icon, splash screens, branding
**Location:** `packages/browseros/resources/`

```bash
# After changing resources:
cd packages/browseros
python3 -m build.browseros build --prep-only

# Restart browser to see changes
pkill -f "BrowserOS.app"
./launch-browseros.sh --browser-only
```

**Time:** ~30 seconds

### 🔧 For Server Code

**What:** MCP tools, agent backend
**Time:** Instant restart

```bash
# Terminal 1: Run with watch mode
cd BrowserOS-server  
bun --watch start

# Server auto-restarts on any code change
```

## Quick Workflow Examples

### Changing the Agent Logo

```bash
# 1. Start watch mode (one time)
cd packages/browseros-agent
yarn watch &

# 2. Edit the logo
open src/assets/logo.svg  # or .png

# 3. Wait ~5 seconds for rebuild
# 4. Reload extension: chrome://extensions → Click reload
# 5. Done! Logo updated instantly
```

### Changing the BrowserOS App Icon

```bash
# 1. Replace icon file
cp new-icon.icns packages/browseros/app/app.icns

# 2. Copy resources
cd packages/browseros
python3 -m build.browseros build --prep-only

# 3. Restart browser
pkill -f "BrowserOS.app"
open /Users/Mukira/chromium/src/out/Release/BrowserOS.app
```

## Development Mode Script

For ultimate productivity:

```bash
# Start hot-reload for agent + server
./dev.sh

# This starts:
# - Agent webpack watcher (auto-rebuild)
# - Server with --watch (auto-restart)
```

## Tips for Fastest Iteration

### 1. Keep Extension Reloader Open
```
chrome://extensions
```
After agent changes, just click the reload button.

### 2. Use Browser DevTools
```
Right-click extension → Inspect
```
See console errors and test changes live.

### 3. Don't Run Full Builds
```bash
# ❌ Slow (rebuilds everything)
./build.sh

# ✅ Fast (watches and rebuilds only changes)
cd packages/browseros-agent && yarn watch
```

### 4. Separate Terminals

```
Terminal 1: yarn watch          (agent auto-build)
Terminal 2: bun --watch start   (server auto-restart)
Terminal 3: Your testing/commands
```

##Component Rebuild Times

| Component | Method | Time | Hot-Reload? |
|-----------|--------|------|-------------|
| Agent UI/Logo | `yarn watch` | 5s | ✅ Yes |
| Agent Code | `yarn watch` | 5-10s | ✅ Yes |
| Server Code | `bun --watch start` | <1s | ✅ Yes |
| Browser Resources | `--prep-only` | 30s | ⚠️ Manual reload |
| Browser Code | `./build.sh` | 5-30min | ❌ No |

## Cheat Sheet

```bash
# Agent changes (CSS, components, logos)
cd packages/browseros-agent && yarn watch

# Server changes
cd BrowserOS-server && bun --watch start

# Browser resources (icons, branding)
cd packages/browseros && python3 -m build.browseros build --prep-only

# Full rebuild (rarely needed)
./build.sh --full
```

## Where Files Live

**Agent Extension:**
- Source: `packages/browseros-agent/src/`
- Logo: `packages/browseros-agent/src/assets/`
- Built: `packages/browseros-agent/dist/`
- Loaded from: `dist/` directory in browser

**Browser:**
- Resources: `packages/browseros/resources/`
- App icon: `packages/browseros/app/app.icns`
- Built: `/Users/Mukira/chromium/src/out/Release/BrowserOS.app`

**Server:**
- Source: `BrowserOS-server/packages/`
- Runs from: Source (no build needed!)

## Common Issues

### "Changes not showing"
1. Did webpack rebuild? (Check terminal output)
2. Did you reload extension? (chrome://extensions)
3. Did you hard-refresh page? (Cmd+Shift+R)

### "Extension broke after change"
1. Check console: Right-click extension → Inspect
2. Check build: Look for webpack errors
3. Reload: Restart browser if needed

### "Build seems slow"
1. Are you using `yarn watch`? (Not full rebuild)
2. Close large files in IDE
3. Clear webpack cache: `rm -rf node_modules/.cache`

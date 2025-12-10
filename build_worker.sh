#!/bin/bash
echo "════════════════════════════════════════════════════════════════"
echo "  BrowserOS Build Worker Starting"
echo "════════════════════════════════════════════════════════════════"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Add Ninja to PATH
export PATH="/Users/Mukira/chromium/src/third_party/ninja:$PATH"

# 1. Clean caches (matching previous log behavior)
echo "🧹 Cleaning user caches..."
# python3 clean_startup_tabs.py # Optional, if it exists

# 2. Build Components
echo "🏗️  Building Components..."

# 2a. Build Agent
echo "   📦 Building BrowserOS Agent..."
cd "$SCRIPT_DIR/BrowserOS-agent"
yarn run build
cd "$SCRIPT_DIR"

# 2b. Build Server (Native Binary)
echo "   🖥️  Building BrowserOS Server..."
cd "$SCRIPT_DIR/BrowserOS-server"
# Using the dev:server:macos script we verified earlier
bun run dev:server:macos
cd "$SCRIPT_DIR"

# 2c. Embed Server
echo "   🔗 Embedding Server into App Bundle..."
SERVER_BIN_DEST="$SCRIPT_DIR/out/Release/BrowserOS.app/Contents/Resources/BrowserOSServer/default/resources/bin/browseros_server"
cp "$SCRIPT_DIR/BrowserOS-server/dist/server/browseros-server" "$SERVER_BIN_DEST"
chmod +x "$SERVER_BIN_DEST"

# 2d. Rebuild Chromium (Incremental)
echo "   🌐 Updating Chromium Binary (if needed)..."
ninja -C out/Release chrome

# 3. Launch
echo "🚀 Launching E-Nation OS as New User..."
TEMP_PROFILE="/tmp/browseros_user_$(date +%s)"
echo "   Profile: $TEMP_PROFILE"

# Launch the binary directly to capture output in nohup log
AGENT_PATH="$SCRIPT_DIR/BrowserOS-agent/dist"
echo "   Loading Extension from: $AGENT_PATH"

"./out/Release/BrowserOS.app/Contents/MacOS/BrowserOS" \
  --user-data-dir="$TEMP_PROFILE" \
  --no-first-run \
  --no-default-browser-check \
  --load-extension="$AGENT_PATH" \
  --enable-logging --v=1


echo "✅ Done!"

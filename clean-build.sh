#!/bin/bash
# Clean Build - Kills all existing build processes and starts fresh
# Use this when builds get stuck

set -e

echo "Cleaning up stuck build processes..."

# Kill all ninja and build processes
pkill -9 ninja 2>/dev/null || true
pkill -9 siso 2>/dev/null || true
pkill -9 autoninja 2>/dev/null || true  
pkill -f "build.sh" 2>/dev/null || true
pkill -f "python.*autoninja" 2>/dev/null || true

sleep 2

# Remove lock files
rm -f /Users/Mukira/chromium/src/out/Default_x64/.ninja_lock 2>/dev/null || true
rm -f /Users/Mukira/chromium/src/out/Release/.ninja_lock 2>/dev/null || true

echo "✓ Cleaned up processes and locks"
echo ""
echo "Now run: ./build.sh"

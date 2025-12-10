#!/bin/bash
# Quick build and launch script for BrowserOS
# Builds all components and launches the unified stack

set -e

echo "🚀 Building and launching BrowserOS..."
echo ""

# Build everything
./build.sh

echo ""
echo "✅ Build complete! Launching BrowserOS stack..."
echo ""

# Launch the unified stack
./launch-browseros.sh

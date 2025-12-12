#!/bin/bash
# Quick build and launch script for e-GovernmentOS
# Builds all components and launches the unified stack

set -e

echo "🚀 Building and launching e-GovernmentOS..."
echo ""

# Build everything
./build.sh

echo ""
echo "✅ Build complete! Launching e-GovernmentOS stack..."
echo ""

# Launch the unified stack
./launch-e-governmentos.sh

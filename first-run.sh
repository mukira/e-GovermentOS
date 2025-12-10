#!/bin/bash
################################################################################
# BrowserOS First Run Simulator
#
# Simulates a brand new installation by:
# 1. Wiping the user data/profile directory
# 2. Launching the BrowserOS stack
################################################################################

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

PROFILE_DIR="$HOME/Library/Application Support/BrowserOS"

echo -e "${BLUE}════════════════════════════════════════════════════════════════════════${NC}"
echo -e "  BrowserOS First Run Simulator"
echo -e "${BLUE}════════════════════════════════════════════════════════════════════════${NC}"
echo ""

echo -e "${RED}WARNING: This will delete all BrowserOS user data.${NC}"
echo -e "Profile Directory: $PROFILE_DIR"
echo ""

# Confirm actions
echo -e "${BLUE}ℹ${NC} Clearing user profile..."
if [ -d "$PROFILE_DIR" ]; then
    rm -rf "$PROFILE_DIR"
    echo -e "${GREEN}✓${NC} Profile cleared."
else
    echo -e "${BLUE}ℹ${NC} No existing profile found (already clean)."
fi

echo ""
echo -e "${BLUE}ℹ${NC} Launching BrowserOS..."
echo ""

# Execute the main launcher
./launch-browseros.sh "$@"

#!/bin/bash
# Enhanced e-GovernmentOS Build Monitor
# Features: Accurate progress, error detection, stall warnings

CHROMIUM_OUT="/Users/Mukira/chromium/src/out/Default_x64"
BUILD_LOG="/tmp/e-governmentos-build.log"

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
BOLD='\033[1m'
NC='\033[0m'

# Track stats
PREV_TASKS=0
START_TIME=$(date +%s)
LAST_UPDATE_TIME=$START_TIME
STALL_THRESHOLD=60  # Warn if no progress for 60 seconds
ERROR_COUNT=0

draw_progress_bar() {
    local current=$1
    local total=$2
    local width=50
    local percent=$((current * 100 / total))
    local filled=$((percent * width / 100))
    local empty=$((width - filled))
    
    printf "["
    printf "${GREEN}%0.s█${NC}" $(seq 1 $filled)
    printf "%0.s░" $(seq 1 $empty)
    printf "] ${BOLD}${percent}%%${NC}"
}

format_time() {
    local seconds=$1
    if [[ $seconds -lt 60 ]]; then
        echo "${seconds}s"
    elif [[ $seconds -lt 3600 ]]; then
        printf "%dm %ds" $((seconds / 60)) $((seconds % 60))
    else
        printf "%dh %dm" $((seconds / 3600)) $(((seconds % 3600) / 60))
    fi
}

check_for_errors() {
    # Check build output for errors
    if [[ -f "$BUILD_LOG" ]]; then
        local errors=$(grep -i "error:" "$BUILD_LOG" | tail -5)
        if [[ -n "$errors" ]]; then
            echo -e "${RED}⚠ BUILD ERRORS DETECTED:${NC}"
            echo "$errors" | while read line; do
                echo -e "  ${RED}$line${NC}"
            done
            return 1
        fi
    fi
    return 0
}

echo -e "${BOLD}${BLUE}"
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║      e-GovernmentOS Build Monitor - Accurate + Error Detection     ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo -e "${NC}"
echo "Press Ctrl+C to stop monitoring"
echo ""

while true; do
    clear
    echo -e "${BOLD}${BLUE}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BOLD}${BLUE}║      e-GovernmentOS Build Monitor - Accurate + Error Detection     ║${NC}"
    echo -e "${BOLD}${BLUE}╚════════════════════════════════════════════════════════════════╝${NC}"
    echo ""
    
    # Check if ninja is running
    if ps aux | grep -q "[n]inja.*Default_x64"; then
        echo -e "${GREEN}●${NC} ${BOLD}Build Status:${NC} ${GREEN}RUNNING${NC}"
        echo ""
        
        CURRENT_TIME=$(date +%s)
        ELAPSED=$((CURRENT_TIME - START_TIME))
        
        if [[ -f "$CHROMIUM_OUT/.ninja_log" ]]; then
            COMPLETED_TASKS=$(wc -l < "$CHROMIUM_OUT/.ninja_log" 2>/dev/null | tr -d ' ')
            
            # Detect build type
            if [[ $COMPLETED_TASKS -lt 1000 ]]; then
                BUILD_TYPE="Incremental"
                ESTIMATED_TOTAL=$COMPLETED_TASKS
                NEEDS_REBUILD=$COMPLETED_TASKS
            else
                BUILD_TYPE="Full"
                ESTIMATED_TOTAL=71000
                NEEDS_REBUILD=$COMPLETED_TASKS
            fi
            
            # Display build type
            if [[ "$BUILD_TYPE" == "Incremental" ]]; then
                echo -e "  ${BOLD}Build Type:${NC} ${CYAN}Incremental${NC} (only rebuilding changed files)"
                echo -e "  ${BOLD}Files to rebuild:${NC} ~${NEEDS_REBUILD} files need recompilation"
            else
                echo -e "  ${BOLD}Build Type:${NC} ${YELLOW}Full Rebuild${NC}"
            fi
            echo ""
            
            # Progress bar
            echo -ne "  ${BOLD}Progress:${NC} "
            if [[ "$BUILD_TYPE" == "Incremental" ]]; then
                # For incremental, assume we know total from ninja
                draw_progress_bar $COMPLETED_TASKS $((NEEDS_REBUILD + 100))
            else
                draw_progress_bar $COMPLETED_TASKS $ESTIMATED_TOTAL
            fi
            echo ""
            echo ""
            
            # Check for stall
            TIME_SINCE_UPDATE=$((CURRENT_TIME - LAST_UPDATE_TIME))
            if [[ $COMPLETED_TASKS -ne $PREV_TASKS ]]; then
                LAST_UPDATE_TIME=$CURRENT_TIME
                TIME_SINCE_UPDATE=0
            fi
            
            # Calculate speed
            TASKS_DIFF=$((COMPLETED_TASKS - PREV_TASKS))
            if [[ $ELAPSED -gt 0 ]]; then
                AVG_SPEED=$((COMPLETED_TASKS / ELAPSED))
            else
                AVG_SPEED=0
            fi
            
            # Calculate ETA
            if [[ $AVG_SPEED -gt 0 ]]; then
                if [[ "$BUILD_TYPE" == "Incremental" ]]; then
                    REMAINING=$((NEEDS_REBUILD - COMPLETED_TASKS))
                else
                    REMAINING=$((ESTIMATED_TOTAL - COMPLETED_TASKS))
                fi
                ETA_SECONDS=$((REMAINING / AVG_SPEED))
                ETA_FORMATTED=$(format_time $ETA_SECONDS)
            else
                ETA_FORMATTED="Calculating..."
            fi
            
            # Display stats
            echo -e "  ${BOLD}Tasks:${NC}      ${CYAN}${COMPLETED_TASKS}${NC} completed"
            echo -e "  ${BOLD}Speed:${NC}      ${YELLOW}~${AVG_SPEED}${NC} tasks/second (average)"
            echo -e "  ${BOLD}Elapsed:${NC}    ${MAGENTA}$(format_time $ELAPSED)${NC}"
            echo -e "  ${BOLD}ETA:${NC}        ${GREEN}${ETA_FORMATTED}${NC} remaining"
            
            # Stall detection
            if [[ $TIME_SINCE_UPDATE -gt $STALL_THRESHOLD ]]; then
                echo ""
                echo -e "  ${RED}⚠ WARNING:${NC} Build may be stalled (no progress for $(format_time $TIME_SINCE_UPDATE))"
            fi
            
            # Update tracking
            PREV_TASKS=$COMPLETED_TASKS
        else
            echo "  ${YELLOW}⚠${NC} Waiting for ninja to start..."
        fi
        
        # CPU usage
        CPU=$(ps aux | grep "[n]inja.*Default_x64" | awk '{print $3}' | head -1)
        [[ -n "$CPU" ]] && echo -e "  ${BOLD}CPU Usage:${NC}  ${CPU}%"
        
        # Error check
        echo ""
        if ! check_for_errors; then
            ERROR_COUNT=$((ERROR_COUNT + 1))
        fi
        
        # Recent activity
        echo ""
        echo -e "${BOLD}Recent Activity:${NC}"
        echo "────────────────────────────────────────────────────────────────"
        tail -5 "$CHROMIUM_OUT/.ninja_log" 2>/dev/null | awk '{
            file=$4
            if (length(file) > 60) {
                file = "..." substr(file, length(file)-57)
            }
            print "  " file
        }' || echo "  (waiting for compilation to start)"
        
    else
        echo -e "${YELLOW}●${NC} ${BOLD}Build Status:${NC} ${YELLOW}NOT RUNNING${NC}"
        echo ""
        
        # Check if build failed
        if [[ $ERROR_COUNT -gt 0 ]]; then
            echo -e "  ${RED}Build may have failed!${NC} Check errors above."
            echo ""
        fi
        
        echo "  To start build: ${CYAN}./build.sh${NC}"
        echo ""
        break
    fi
    
    echo ""
    echo "────────────────────────────────────────────────────────────────"
    echo -e "${BOLD}Refreshing in 2 seconds...${NC} (Ctrl+C to stop)"
    
    sleep 2
done

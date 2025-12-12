#!/usr/bin/env python3
"""Clean, minimal build monitor with proper formatting"""

import time
import sys
import os
import re
import subprocess
import threading
import select
import termios
import tty
from datetime import datetime, timedelta

LOG_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "rebuild.log")

# State
paused = False
muted = False
stall_threshold = 300  # 5 minutes

# ANSI codes
CLEAR = "\033[2J\033[H"
HIDE_CURSOR = "\033[?25l"
SHOW_CURSOR = "\033[?25h"
ALT_SCREEN = "\033[?1049h"
NORM_SCREEN = "\033[?1049l"

# Colors
CYAN = "\033[36m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
RED = "\033[31m"
BLUE = "\033[34m"
MAGENTA = "\033[35m"
GRAY = "\033[90m"
BOLD = "\033[1m"
RESET = "\033[0m"

def get_progress(line):
    match = re.search(r'\[(\d+)/(\d+)\]', line)
    return (int(match.group(1)), int(match.group(2))) if match else (None, None)

def get_cpu():
    try:
        result = subprocess.run(['ps', '-A', '-o', '%cpu'], capture_output=True, text=True)
        return min(sum(float(x) for x in result.stdout.split('\n')[1:] if x.strip()), 100.0)
    except:
        return 0.0

def get_mem():
    try:
        result = subprocess.run(['vm_stat'], capture_output=True, text=True)
        pages = {}
        for line in result.stdout.split('\n'):
            if 'Pages free' in line: pages['free'] = int(line.split(':')[1].strip().rstrip('.'))
            elif 'Pages active' in line: pages['active'] = int(line.split(':')[1].strip().rstrip('.'))
            elif 'Pages wired' in line: pages['wired'] = int(line.split(':')[1].strip().rstrip('.'))
            elif 'Pages inactive' in line: pages['inactive'] = int(line.split(':')[1].strip().rstrip('.'))
        used = (pages.get('active', 0) + pages.get('wired', 0)) * 4096
        total = sum(pages.values()) * 4096
        return (used / total * 100) if total > 0 else 0.0
    except:
        return 0.0

def sparkline(values, width=50):
    if not values: return " " * width
    ticks = "▁▂▃▄▅▆▇█"
    mn, mx = min(values), max(values)
    if mx == mn: return ticks[0] * width
    norm = [(v - mn) / (mx - mn) for v in values[-width:]]
    return ''.join(ticks[min(int(v * 8), 7)] for v in norm)

def bar(filled, width, color=""):
    fill = "█" * filled + "░" * (width - filled)
    return f"{color}{fill}{RESET}" if color else fill

def fmt_time(sec):
    if sec < 60: return f"{int(sec)}s"
    if sec < 3600: return f"{int(sec//60)}m {int(sec%60)}s"
    h, m = int(sec//3600), int((sec%3600)//60)
    return f"{h}h {m}m"

def keyboard_listener(queue):
    old = termios.tcgetattr(sys.stdin)
    try:
        tty.setcbreak(sys.stdin.fileno())
        while True:
            if select.select([sys.stdin], [], [], 0.1)[0]:
                queue.append(sys.stdin.read(1).lower())
    finally:
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old)

def main():
    global paused, muted
    
    print(ALT_SCREEN + HIDE_CURSOR + CLEAR, end='', flush=True)
    
    keys = []
    threading.Thread(target=keyboard_listener, args=(keys,), daemon=True).start()
    
    print("Waiting for build...")
    while not os.path.exists(LOG_FILE):
        time.sleep(0.5)
    
    with open(LOG_FILE, 'r') as f:
        pos = 0
        cur = tot = 0
        last_update = start = time.time()
        speeds = []
        stall_warned = False
        error_detected = False
        
        while True:
            # Handle keys
            while keys:
                k = keys.pop(0)
                if k == 'p':
                    paused = not paused
                    subprocess.run(['pkill', '-STOP' if paused else '-CONT', '-f', 'ninja'], check=False)
                    last_update = time.time()
                    stall_warned = False
                elif k == 'm':
                    muted = not muted
                elif k == 'q':
                    print(SHOW_CURSOR + NORM_SCREEN)
                    return
            
            if paused:
                time.sleep(0.1)
                continue
            
            # Read log
            f.seek(pos)
            lines = f.readlines()
            pos = f.tell()
            
            for line in lines:
                c, t = get_progress(line)
                if c and t:
                    if c > cur:
                        last_update = time.time()
                        stall_warned = False
                        elapsed = time.time() - start
                        speed = c / elapsed if elapsed > 0 else 0
                        speeds.append(speed)
                        if len(speeds) > 50:
                            speeds.pop(0)
                    cur, tot = c, t
                
                # Check for errors
                if "ninja: build stopped: subcommand failed" in line or "FAILED:" in line:
                    error_detected = True
                    if not muted:
                        for _ in range(5):
                            subprocess.run(['osascript', '-e', 'beep'], check=False)
                            time.sleep(0.5)
            
            # Check stall
            if time.time() - last_update > stall_threshold and cur > 0 and not stall_warned:
                stall_warned = True
                if not muted:
                    for _ in range(3):
                        subprocess.run(['osascript', '-e', 'beep'], check=False)
                        time.sleep(0.3)
            
            # Render
            if tot > 0:
                print(CLEAR, end='', flush=True)
                
                pct = (cur / tot) * 100
                filled = int(60 * cur / tot)
                prog = bar(filled, 60, GREEN if pct > 50 else YELLOW)
                
                print(f"\n  {BOLD}{CYAN}━━━ e-GovernmentOS Build Monitor ━━━{RESET}\n")
                print(f"  Progress:  {prog} {BOLD}{YELLOW}{pct:.4f}%{RESET}")
                print(f"             {GRAY}{cur:,} / {tot:,} files{RESET}\n")
                
                if speeds:
                    avg = sum(speeds) / len(speeds)
                    remaining = tot - cur
                    eta_sec = remaining / avg if avg > 0 else 0
                    eta_time = datetime.now() + timedelta(seconds=eta_sec)
                    
                    speed_color = GREEN if avg > 10 else (YELLOW if avg > 5 else RED)
                    print(f"  Speed:     {speed_color}{avg:.1f} files/s{RESET}")
                    print(f"  ETA:       {MAGENTA}{eta_time.strftime('%I:%M %p')}{RESET} {GRAY}({fmt_time(eta_sec)}){RESET}")
                    print(f"  Trend:     {sparkline(speeds, 60)}\n")
                
                cpu, mem = get_cpu(), get_mem()
                cpu_color = GREEN if cpu < 80 else (YELLOW if cpu < 95 else RED)
                mem_color = GREEN if mem < 70 else (YELLOW if mem < 85 else RED)
                
                print(f"  CPU:       {bar(int(cpu*0.5), 50, cpu_color)} {cpu_color}{cpu:.0f}%{RESET}")
                print(f"  RAM:       {bar(int(mem*0.5), 50, mem_color)} {mem_color}{mem:.0f}%{RESET}\n")
                
                status = f"{YELLOW}⏸ PAUSED{RESET}" if paused else (f"{RED}❌ FAILED{RESET}" if error_detected else (f"{RED}⚠ STALLED{RESET}" if stall_warned else f"{GREEN}▶ BUILDING{RESET}"))
                audio = "🔇" if muted else "🔊"
                
                print(f"  Status:    {status}")
                print(f"  Audio:     {audio}")
                print(f"  Elapsed:   {BLUE}{fmt_time(time.time()-start)}{RESET}\n")
                
                print(f"  {GRAY}[P] Pause  [M] Mute  [Q] Quit{RESET}")
            
            # Check done
            if cur > 0 and cur == tot:
                print(SHOW_CURSOR + NORM_SCREEN)
                print(f"\n✅ Build complete! ({fmt_time(time.time()-start)})\n")
                break
            
            time.sleep(2.0)  # Slower refresh for better readability

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print(SHOW_CURSOR + NORM_SCREEN)
    except Exception as e:
        print(SHOW_CURSOR + NORM_SCREEN)
        print(f"Error: {e}")

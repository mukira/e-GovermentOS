#!/usr/bin/env python3
import sys
import re
import time
import os
import collections
from datetime import datetime

# Try to import rich
try:
    from rich.live import Live
    from rich.panel import Panel
    from rich.console import Console, Group
    from rich.table import Table
    from rich.text import Text
    from rich.align import Align
    from rich import box
    from rich.spinner import Spinner
    from rich.layout import Layout
    from rich.style import Style
except ImportError:
    print("Error: The 'rich' library is required.")
    print("Please run: pip install rich")
    sys.exit(1)

STALL_THRESHOLD = 300  # 5 minutes without progress

def format_time(seconds):
    if seconds < 0: return "00:00"
    m, s = divmod(int(seconds), 60)
    h, m = divmod(m, 60)
    if h > 0:
        return f"{h}h {m:02d}m {s:02d}s"
    return f"{m:02d}m {s:02d}s"

class BuildMonitor:
    def __init__(self, filepath):
        self.filepath = filepath
        self.console = Console()
        
        # State
        self.current = 0
        self.total = 100 
        self.max_total = 0 
        self.start_time = time.time()
        self.initial_run_time = self.start_time
        self.current_start_val = 0
        self.last_change_time = time.time()
        self.logs = collections.deque(maxlen=15)
        self.progress_history = collections.deque() # For Sliding Window Speed
        self.is_stalled = False
        
        # Smart Metrics
        self.speed = 0.0          # Current instant speed
        self.avg_speed = 0.0      # EMA Smoothed speed
        self.phase = "Initializing"
        self.phase_map = {
            "v8/": "Compiling V8 JS Engine",
            "blink/": "Compiling Blink Renderer",
            "chrome/": "Building Chrome Core",
            "net/": "Building Network Stack",
            "ui/": "Building UI Components",
            "mojom": "Generating IPC Bindings",
            "LINK": "Linking Executables"
        }
        
        self.spinner = Spinner("dots", text="Monitoring...", style="cyan")
        self.status = "Waiting..."

    def get_status_style(self):
        if self.is_stalled:
            return "bold yellow on red"
        return "bold green on blue"

    def detect_phase(self, line):
        for key, description in self.phase_map.items():
            if key in line:
                self.phase = description
                return

    def update_metrics(self, cpu_load=0.0):
        now = time.time()
        time_since_last = now - self.last_change_time
        
        # --- Sliding Window Speed Calculation ---
        # 1. Add current point (if changed or periodically)
        self.progress_history.append((now, self.current))
        
        # 2. Prune old points (> 60s ago)
        while len(self.progress_history) > 2 and (now - self.progress_history[0][0] > 60):
            self.progress_history.popleft()
            
        # 3. Calculate Speed over the window
        if len(self.progress_history) > 1:
            start_time, start_count = self.progress_history[0]
            end_time, end_count = self.progress_history[-1]
            time_delta = end_time - start_time
            count_delta = end_count - start_count
            
            if time_delta > 1:
                self.speed = count_delta / time_delta
            else:
                self.speed = 0.0
        else:
             self.speed = 0.0

        # Stall Check (Smart)
        if self.current > 0 and time_since_last > STALL_THRESHOLD:
            if cpu_load < 1.0: # Stricter CPU check
                self.is_stalled = True
                self.status = "STALLED"
            else:
                self.is_stalled = False
                self.status = f"Busy (CPU: {cpu_load:.1f})..."
        else:
            self.is_stalled = False
            # Detect graph check
            if self.speed > 50:
                self.status = "Verifying Graph..."
                self.phase = "Graph Verification"
            else:
                self.status = "Building..."
                
    def get_dashboard(self, cpu_status="Unknown"):
        layout = Layout()
        layout.split(
            Layout(name="header", size=3),
            Layout(name="main", ratio=1),
            Layout(name="footer", size=10)
        )
        
        layout["main"].split_row(
            Layout(name="metrics", ratio=1),
            Layout(name="progress", ratio=2)
        )

        now = time.time()
        elapsed = now - self.start_time
        
        # --- HEADER ---
        header_style = Style(color="white", bgcolor="blue", bold=True)
        status_style = Style(color="green", bgcolor="blue", bold=True)
        
        # Pulse effect for liveness
        pulse = "●" if int(now * 2) % 2 == 0 else "○"
        
        title_text = f" {pulse} e-GovernmentOS Build (PID {os.getpid()}) "
        status_text = f" {self.status} "

        if self.is_stalled:
            title_text = " BUILD STALLED - CHECK PROCESS "
            status_text = " STALLED "
            header_style = Style(color="white", bgcolor="red", bold=True)
            status_style = Style(color="yellow", bgcolor="red", blink=True, bold=True)

        grid = Table.grid(expand=True)
        grid.add_column(justify="left")
        grid.add_column(justify="center", ratio=1)
        grid.add_column(justify="right")
        
        # Render spinner
        spinner_render = self.spinner.render(now)
        
        grid.add_row(
            f" 🖥️  Load: {cpu_status}",
            Text(title_text, style="bold white"),
            spinner_render
        )
        layout["header"].update(Panel(grid, style=header_style))

        # --- METRICS ---
        m_grid = Table.grid(expand=True, padding=(0,1))
        m_grid.add_column(ratio=1)
        
        if self.speed > 0:
            rem_sec = (self.total - self.current) / self.speed
            eta = format_time(rem_sec)
            finish_time = datetime.fromtimestamp(now + rem_sec).strftime("%H:%M")
        else:
            eta = "--:--"
            finish_time = "--:--"
            
        def make_stat(label, value, color):
            return Group(
                Text(label, style="dim white"),
                Text(value, style=f"bold {color} reverse")
            )

        m_grid.add_row(make_stat("🚀 SPEED", f"{self.speed:.1f}/s", "magenta"))
        m_grid.add_row(Text(" "))
        m_grid.add_row(make_stat("⏱️  ELAPSED", format_time(elapsed), "cyan"))
        m_grid.add_row(Text(" "))
        m_grid.add_row(make_stat("⌛ ETA", f"{eta} (at {finish_time})", "yellow"))
        m_grid.add_row(Text(" "))
        m_grid.add_row(make_stat("🏭 PHASE", self.phase, "blue"))
        
        layout["metrics"].update(Panel(m_grid, title="Smart Metrics", border_style="blue"))

        # --- PROGRESS ---
        # RAW TRUTH MODE - Requested by User
        # No more "Grand Total" guessing. Show exactly what is in the logs.
        
        remaining = max(0, self.total - self.current)
        completed_pct = (self.current / self.total * 100) if self.total > 0 else 0
        
        if self.total > self.max_total: self.max_total = self.total
        
        # Visual Bar
        bar_width = 60
        filled = int(bar_width * (completed_pct / 100))
        rem_bar = bar_width - filled
        bar_str = "█" * filled + "░" * rem_bar
        
        color = "red" if self.is_stalled else "bright_green"
        if self.speed > 50: color = "blue"

        # Stats Table
        
        # Calculate ETA for the main block for better visibility
        if self.speed > 0:
            rem_sec = remaining / self.speed
            eta_str = format_time(rem_sec)
        else:
            eta_str = "--:--"

        stat_group = Group(
            Align.center(Text(f"\n{self.current:,} / {self.total:,}", style="bold white")),
            Align.center(Text(f"{completed_pct:.4f}%", style=f"bold {color} underline", justify="center")),
            Align.center(Text(f"Remaining: {remaining:,}", style="bold yellow blink")),
            Align.center(Text(f"ETA: {eta_str}", style="bold cyan")),
            Text("\n"),
            Align.center(Text(bar_str, style=color)),
            Text("\n")
        )
        layout["progress"].update(Panel(stat_group, title="Real-Time Building", border_style=color))

        # --- LOGS ---
        log_text = Text()
        for i, line in enumerate(list(self.logs)):
            style = "dim white"
            if "CXX" in line: style = "green"
            if "ACTION" in line: style = "cyan"
            if "STAMP" in line: style = "blue"
            if i == len(self.logs) - 1: style = f"bold {style}"
            
            log_text.append(f"{line}\n", style=style)
            
        layout["footer"].update(Panel(log_text, title="Latest Activity", border_style="white"))
        
        return layout

    def run(self):
        if not os.path.exists(self.filepath):
            self.console.print(f"[yellow]Waiting for {self.filepath}...[/]")
            while not os.path.exists(self.filepath):
                time.sleep(1)

        with open(self.filepath, 'r') as f:
            with Live(self.get_dashboard(), refresh_per_second=10, screen=True) as live:
                while True:
                    lines_read = 0
                    while True:
                        line = f.readline()
                        if not line: break
                        lines_read += 1
                        
                        clean_line = line.strip()
                        match = re.search(r'\[(\d+)/(\d+)\]', line)
                        if match:
                            prev = self.current
                            self.current = int(match.group(1))
                            self.total = int(match.group(2))
                            now = time.time()
                            if self.current > prev:
                                self.last_change_time = now
                        
                        content = re.sub(r'\[\d+/\d+\]\s*', '', clean_line)
                        if content:
                            self.logs.append(content[:120])
                            self.detect_phase(content) # Detect phase from log line
                        
                        if lines_read > 5000: break

                    # Reset start time after initial load to prevent fake high speed
                    # The first pass reads the whole file instantly, giving massive speed (e.g. 300/s)
                    if self.start_time == self.initial_run_time:
                         self.start_time = time.time()
                         self.current_start_val = self.current
                         self.progress_history.clear()

                    # Stats
                    cpu_status = "Unknown"
                    current_load = 0.0
                    try:
                        current_load = os.getloadavg()[0]
                        cpu_status = f"{current_load:.2f}"
                    except: pass

                    self.update_metrics(current_load)
                    live.update(self.get_dashboard(cpu_status))
                    time.sleep(0.05)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 monitor_build_progress.py <logfile>")
        sys.exit(1)
    
    monitor = BuildMonitor(sys.argv[1])
    monitor.run()

#!/usr/bin/env python3
"""
Simple HTTP server for GeoIntel extension
Run this to serve the extension locally and avoid file:// CORS issues
"""

import http.server
import socketserver
import os
import webbrowser

PORT = 8000
DIRECTORY = "."

class MyHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=DIRECTORY, **kwargs)
    
    def end_headers(self):
        # Add CORS headers
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', '*')
        # Add No-Cache headers
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate, max-age=0')
        self.send_header('Pragma', 'no-cache')
        self.send_header('Expires', '0')
        super().end_headers()

class ReusableTCPServer(socketserver.TCPServer):
    allow_reuse_address = True

if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    with ReusableTCPServer(("", PORT), MyHTTPRequestHandler) as httpd:
        print(f"\n✅ GeoIntel Extension Server Running!")
        print(f"📍 Open in browser: http://localhost:{PORT}/index.html")
        print(f"\n🛑 Press Ctrl+C to stop\n")
        
        # Auto-open browser
        webbrowser.open(f'http://localhost:{PORT}/index.html')
        
        httpd.serve_forever()

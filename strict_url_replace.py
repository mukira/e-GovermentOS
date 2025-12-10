import os

file_path = os.path.expanduser("~/chromium/src/chrome/browser/chrome_browser_main.cc")

with open(file_path, "r") as f:
    lines = f.readlines()

new_lines = []
for line in lines:
    # Target the specific ugly link line
    if 'chrome-extension://djhdjhlnljbjgejbndockeedocneiaei/onboarding.html' in line:
        # Replace with the requested clean link, exact casing
        new_lines.append('      browser_creator_->AddFirstRunTabs({GURL("chrome://BrowserOS/onboarding.html")});\n')
    else:
        # Keep everything else (including bit.ly, other logic) exactly as is
        new_lines.append(line)

with open(file_path, "w") as f:
    f.writelines(new_lines)

print("Replaced ugly extension URL with chrome://BrowserOS/onboarding.html")

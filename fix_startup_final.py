import os

file_path = os.path.expanduser("~/chromium/src/chrome/browser/chrome_browser_main.cc")

with open(file_path, "r") as f:
    lines = f.readlines()

new_lines = []
for line in lines:
    if 'GURL("chrome://e-governmentos-first-run")' in line:
        continue # Skip this line
    new_lines.append(line)

with open(file_path, "w") as f:
    f.writelines(new_lines)

print("Removed invalid startup URL from chrome_browser_main.cc")

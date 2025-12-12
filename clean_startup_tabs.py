import os

file_path = os.path.expanduser("~/chromium/src/chrome/browser/chrome_browser_main.cc")

with open(file_path, "r") as f:
    lines = f.readlines()

new_lines = []
for line in lines:
    if 'chrome-extension://djhdjhlnljbjgejbndockeedocneiaei/onboarding.html' in line:
        continue # Remove hardcoded extension tab
    if 'bit.ly/e-GovernmentOS-setup' in line:
        continue # Remove bit.ly tab
    new_lines.append(line)

with open(file_path, "w") as f:
    f.writelines(new_lines)

print("Cleaned up startup tabs in chrome_browser_main.cc")

import os

file_path = os.path.expanduser("~/chromium/src/chrome/browser/chrome_browser_main.cc")

with open(file_path, "r") as f:
    lines = f.readlines()

new_lines = []
inserted = False
for line in lines:
    if 'AddFirstRunTabs' in line and not inserted:
        # Restore the line we deleted, but corrected if needed.
        # Original was: browser_creator_->AddFirstRunTabs({GURL("chrome://e-governmentos-first-run")});
        # We'll add it back.
        if 'chrome://e-governmentos-first-run' not in line:
             new_lines.append('      browser_creator_->AddFirstRunTabs({GURL("chrome://e-governmentos-first-run")});\n')
        inserted = True
    new_lines.append(line)

with open(file_path, "w") as f:
    f.writelines(new_lines)

print("Restored chrome://e-governmentos-first-run to startup")

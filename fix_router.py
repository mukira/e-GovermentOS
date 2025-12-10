import os

file_path = os.path.expanduser("~/chromium/src/chrome/browser/resources/settings/router.ts")

with open(file_path, "r") as f:
    lines = f.readlines()

new_lines = []
inserted = False
for line in lines:
    new_lines.append(line)
    if "export interface SettingsRoutes {" in line and not inserted:
        new_lines.append("  NXTSCAPE: Route;\n")
        new_lines.append("  BROWSEROS: Route;\n")
        new_lines.append("  BROWSEROS_PREFS: Route;\n")
        inserted = True

with open(file_path, "w") as f:
    f.writelines(new_lines)

print("Fixed router.ts")

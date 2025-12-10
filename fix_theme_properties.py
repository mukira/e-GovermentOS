import os

path = os.path.expanduser("~/chromium/src/chrome/browser/themes/theme_properties.cc")

with open(path, "r") as f:
    content = f.read()

# Replace const SkColor with [[maybe_unused]] const SkColor
new_content = content.replace("const SkColor k", "[[maybe_unused]] const SkColor k")

with open(path, "w") as f:
    f.write(new_content)

print("Fixed theme_properties.cc")

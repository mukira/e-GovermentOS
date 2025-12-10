import os

file_path = os.path.expanduser("~/chromium/src/chrome/browser/ai/agent_runtime.h")

with open(file_path, "r") as f:
    content = f.read()

new_content = content.replace('#include "base/callback.h"', '#include "base/functional/callback.h"')

with open(file_path, "w") as f:
    f.write(new_content)

print(f"Fixed {file_path}")

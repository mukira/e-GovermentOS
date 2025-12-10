import os

file_path = os.path.expanduser("~/chromium/src/chrome/browser/ui/browser_command_controller.cc")

with open(file_path, "r") as f:
    content = f.read()

# We need to add a second argument `base::DoNothing()` to OpenURL calls.
# The calls look like:
# browser_->OpenURL(content::OpenURLParams(
#     GURL("..."), content::Referrer(),
#     WindowOpenDisposition::CURRENT_TAB, ui::PAGE_TRANSITION_TYPED, false));

# We can replace the closing `));` with `), base::DoNothing());` for the specific blocks.
# But we need to be careful not to break other OpenURL calls if they exist.
# The specific ones are for "ecitizen.go.ke" and "nation.co.ke".

# Let's replace the whole block signature.

current_citizen = """      browser_->OpenURL(content::OpenURLParams(
          GURL("https://ecitizen.go.ke/"), content::Referrer(),
          WindowOpenDisposition::CURRENT_TAB, ui::PAGE_TRANSITION_TYPED, false));"""

new_citizen = """      browser_->OpenURL(content::OpenURLParams(
          GURL("https://ecitizen.go.ke/"), content::Referrer(),
          WindowOpenDisposition::CURRENT_TAB, ui::PAGE_TRANSITION_TYPED, false), base::DoNothing());"""

current_news = """      browser_->OpenURL(content::OpenURLParams(
          GURL("https://www.nation.co.ke/"), content::Referrer(),
          WindowOpenDisposition::CURRENT_TAB, ui::PAGE_TRANSITION_TYPED, false));"""

new_news = """      browser_->OpenURL(content::OpenURLParams(
          GURL("https://www.nation.co.ke/"), content::Referrer(),
          WindowOpenDisposition::CURRENT_TAB, ui::PAGE_TRANSITION_TYPED, false), base::DoNothing());"""

if current_citizen in content:
    content = content.replace(current_citizen, new_citizen)
else:
    print("Could not find citizen block")

if current_news in content:
    content = content.replace(current_news, new_news)
else:
    print("Could not find news block")
    
# Also ensure base/functional/callback_helpers.h is included for base::DoNothing
if '#include "base/functional/callback_helpers.h"' not in content:
    content = content.replace('#include "chrome/browser/ui/browser_command_controller.h"', 
                            '#include "chrome/browser/ui/browser_command_controller.h"\n#include "base/functional/callback_helpers.h"')

with open(file_path, "w") as f:
    f.write(content)

print("Fixed browser_command_controller.cc")

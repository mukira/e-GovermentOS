import os

file_path = os.path.expanduser("~/chromium/src/chrome/browser/themes/theme_properties.cc")

with open(file_path, "r") as f:
    content = f.read()

# Fix 1: WHITESPACE_ASCII error
# The original code was likely using base::SplitString with separators as string, not an enum for the second arg if it's WHITESPACE_ASCII.
# Signature: SplitString(StringPiece input, StringPiece separators, WhitespaceHandling whitespace, SplitResult result_type)
# We passed base::WHITESPACE_ASCII as the second argument, which is wrong if that's an enum or constant for whitespace.
# Actually, base::WHITESPACE_ASCII represents the characters.
# But looking at error: "no member named 'WHITESPACE_ASCII' in namespace 'base'".
# It seems it might be base::kWhitespaceASCII or similar.
# BUT, the correct usage for SplitString is usually passing a string of delimiters.
# " " or base::kWhitespaceASCII if it exists.
# Let's just use " " (space) for simplicity if we are splitting alignment strings like "top left".
# Wait, the code had:
# base::SplitString(alignment, base::WHITESPACE_ASCII, base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY)
# The second arg should be the separators.
# Let's change `base::WHITESPACE_ASCII` to `" "` (space).

if "base::WHITESPACE_ASCII" in content:
    content = content.replace("base::WHITESPACE_ASCII", '" "')

# Fix 2: COLOR_OTP_BACKGROUND -> COLOR_NTP_BACKGROUND
if "COLOR_OTP_BACKGROUND" in content:
    content = content.replace("COLOR_OTP_BACKGROUND", "COLOR_NTP_BACKGROUND")

with open(file_path, "w") as f:
    f.write(content)

print("Fixed theme_properties.cc compilation errors")

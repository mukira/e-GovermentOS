import os

file_path = os.path.expanduser("~/chromium/src/chrome/browser/extensions/external_provider_impl.cc")

with open(file_path, "r") as f:
    lines = f.readlines()

# We need to move the declaration of e-governmentos_loader to before its first use.
# First use is around line 890 (in previous cat)
# Definition is near end.

# Strategy:
# 1. Find declaration line
# 2. Remove it and the block setting it up
# 3. Insert it before the first usage loop or at start of function AddExternalProviders

declaration_block = []
in_decl = False
decl_start_index = -1
decl_end_index = -1

# Identify the declaration block
for i, line in enumerate(lines):
    if "auto e-governmentos_loader = base::MakeRefCounted<e-GovernmentOSExternalLoader>(profile);" in line:
        decl_start_index = i - 3 # Include comments above
        in_decl = True
    
    if in_decl and "}" in line and i > decl_start_index + 10: # Rough heuristic for end of block
             # Actually the block typically ends before the last usage logic...
             # Let's look for constraints.
             pass

# Simpler strategy:
# Just read the whole content, locate the specific declaration block string, remove it, and insert it at the top of AddExternalProviders.

with open(file_path, "r") as f:
    content = f.read()

# The block to move up
decl_string = """  // Add e-GovernmentOS external extension loader
  // This loader fetches extension configuration from a remote URL
  // Enabled by default for all profiles
  auto e-governmentos_loader = base::MakeRefCounted<e-GovernmentOSExternalLoader>(profile);
  
  // Allow custom config URL via command line
  if (base::CommandLine::ForCurrentProcess()->HasSwitch("e-governmentos-extensions-url")) {
    std::string config_url = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII("e-governmentos-extensions-url");
    GURL url(config_url);
    if (url.is_valid()) {
      e-governmentos_loader->SetConfigUrl(url);
    }
  }"""

# Remove it from where it is (searching from bottom is safer or just replace)
# It appears near the end.
if decl_string in content:
    content = content.replace(decl_string, "")
else:
    print("Could not find exact declaration block to move. Manual check needed.")
    # Fallback: maybe comments slightly different?
    # Let's look for just the line:
    line_decl = "auto e-governmentos_loader = base::MakeRefCounted<e-GovernmentOSExternalLoader>(profile);"
    if line_decl in content:
        # We found the line, but not the block.
        # Let's just prepend definitions at the top of the function
        pass

# Insert at top of AddExternalProviders
# Find "void ExternalProviderImpl::CreateExternalProviders"
insertion_point = "void ExternalProviderImpl::CreateExternalProviders("
index = content.find(insertion_point)
if index != -1:
    # Find opening brace
    brace_index = content.find("{", index)
    if brace_index != -1:
        # Insert after brace
        content = content[:brace_index+1] + "\n" + decl_string + "\n" + content[brace_index+1:]

with open(file_path, "w") as f:
    f.write(content)

print("Moved e-governmentos_loader declaration to top.")

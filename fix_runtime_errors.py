import os

SRC_DIR = os.path.expanduser("~/chromium/src/chrome/browser/ai")

def fix_agent_runtime():
    header_path = os.path.join(SRC_DIR, "agent_runtime.h")
    cc_path = os.path.join(SRC_DIR, "agent_runtime.cc")

    # 1. Fix Header: Add copy constructor for AuditLogEntry
    with open(header_path, "r") as f:
        h_lines = f.readlines()
    
    new_h_lines = []
    for line in h_lines:
        if "struct AuditLogEntry {" in line:
            new_h_lines.append(line)
            # Check if copy ctor exists (it wasn't there in cat output)
            # We already added default ctor/dtor in previous fix script, let's look for them
        elif "AuditLogEntry();" in line:
            new_h_lines.append(line)
            new_h_lines.append("  AuditLogEntry(const AuditLogEntry& other);\n")
        else:
            new_h_lines.append(line)
            
    with open(header_path, "w") as f:
        f.writelines(new_h_lines)
    print("Fixed agent_runtime.h")

    # 2. Fix Implementation: Remove try/catch, fix Time, implement copy ctor
    with open(cc_path, "r") as f:
        cc_content = f.read()

    # Add includes
    if '#include "base/strings/string_number_conversions.h"' not in cc_content:
        cc_content = cc_content.replace('#include "chrome/browser/ai/agent_runtime.h"', 
                                      '#include "chrome/browser/ai/agent_runtime.h"\n#include "base/strings/string_number_conversions.h"')

    # Fix try/catch
    # Simple regex replacement or string replacement might be risky for multi-line but let's try
    # The try block wraps ExecuteAgent logic.
    # We will just remove "try {" and "} catch (...) { ... }"
    # Logic:
    # try {
    #   agent->Run();
    #   LogAction(agent->GetId(), "EXECUTE", "SUCCESS");
    # } catch (...) {
    #   LogAction(agent->GetId(), "EXECUTE", "FAILED");
    #   LOG(ERROR) << "Agent execution failed for: " << agent->GetId();
    # }
    
    # We'll replace it with:
    # agent->Run();
    # LogAction(agent->GetId(), "EXECUTE", "SUCCESS");
    
    if "try {" in cc_content:
        # Find the try block start
        start_try = cc_content.find("try {")
        if start_try != -1:
            # We assume the structure is simple as seen in previous cat
            # Let's replace the whole function body for ExecuteAgent if strictly needed, 
            # or just simple replace since we know the content.
            # actually, simpler:
            cc_content = cc_content.replace("  try {", "")
            cc_content = cc_content.replace("    agent->Run();", "  agent->Run();")
            cc_content = cc_content.replace("    LogAction(agent->GetId(), \"EXECUTE\", \"SUCCESS\");", "  LogAction(agent->GetId(), \"EXECUTE\", \"SUCCESS\");")
            cc_content = cc_content.replace("  } catch (...) {", "")
            cc_content = cc_content.replace("    LogAction(agent->GetId(), \"EXECUTE\", \"FAILED\");", "")
            cc_content = cc_content.replace("    LOG(ERROR) << \"Agent execution failed for: \" << agent->GetId();", "")
            cc_content = cc_content.replace("  }", "") 
            # The last brace might match function end... wait.
            # The catch block closing brace.
            # This string replace is too fragile.
            
            # Better approach: Rewrite the function ExecuteAgent
            # We know what it looked like from cat output
            pass

    # Rewrite ExecuteAgent completely to be safe
    new_execute_agent = """void AgentRuntime::ExecuteAgent(std::unique_ptr<Agent> agent) {
  LOG(INFO) << "Executing agent: " << agent->GetId();
  agent->Run();
  LogAction(agent->GetId(), "EXECUTE", "SUCCESS");
}"""
    # Regex to replace existing ExecuteAgent impl?
    # Or just write the file fresh? We have the content.
    # Let's simple-replace the known bad block.
    bad_block = """  try {
    agent->Run();
    LogAction(agent->GetId(), "EXECUTE", "SUCCESS");
  } catch (...) {
    LogAction(agent->GetId(), "EXECUTE", "FAILED");
    LOG(ERROR) << "Agent execution failed for: " << agent->GetId();
  }"""
    
    good_block = """  agent->Run();
  LogAction(agent->GetId(), "EXECUTE", "SUCCESS");"""
    
    cc_content = cc_content.replace(bad_block, good_block)


    # Fix Time::ToString
    cc_content = cc_content.replace("base::Time::Now().ToString()", "base::NumberToString(base::Time::Now().ToDoubleT())")

    # Add AuditLogEntry copy ctor implementation
    if "AuditLogEntry::AuditLogEntry(const AuditLogEntry& other) = default;" not in cc_content:
        cc_content += "\nAuditLogEntry::AuditLogEntry(const AuditLogEntry& other) = default;\n"

    with open(cc_path, "w") as f:
        f.write(cc_content)
    print("Fixed agent_runtime.cc")

if __name__ == "__main__":
    fix_agent_runtime()

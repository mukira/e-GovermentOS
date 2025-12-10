import os

SRC_DIR = os.path.expanduser("~/chromium/src/chrome/browser/ai")

def fix_agent_executor():
    path = os.path.join(SRC_DIR, "agent_executor.cc")
    with open(path, "r") as f:
        content = f.read()
    
    # Fix TimeDelta
    if "base::TimeDelta::FromMilliseconds" in content:
        content = content.replace("base::TimeDelta::FromMilliseconds", "base::Milliseconds")
        with open(path, "w") as f:
            f.write(content)
        print("Fixed agent_executor.cc")

def fix_agent_runtime_header():
    path = os.path.join(SRC_DIR, "agent_runtime.h")
    with open(path, "r") as f:
        lines = f.readlines()
    
    new_lines = []
    for line in lines:
        if "struct AgentAction {" in line:
            new_lines.append(line)
            new_lines.append("  AgentAction();\n")
            new_lines.append("  AgentAction(const AgentAction& other);\n") 
            new_lines.append("  ~AgentAction();\n")
        elif "struct AuditLogEntry {" in line:
            new_lines.append(line)
            new_lines.append("  AuditLogEntry();\n")
            new_lines.append("  ~AuditLogEntry();\n")
        else:
            new_lines.append(line)
            
    with open(path, "w") as f:
        f.writelines(new_lines)
    print("Fixed agent_runtime.h")

def fix_agent_runtime_cc():
    path = os.path.join(SRC_DIR, "agent_runtime.cc")
    with open(path, "r") as f:
        content = f.read()
    
    # Add implementations if not present
    implementations = """

AgentAction::AgentAction() = default;
AgentAction::AgentAction(const AgentAction& other) = default;
AgentAction::~AgentAction() = default;

AuditLogEntry::AuditLogEntry() = default;
AuditLogEntry::~AuditLogEntry() = default;
"""
    
    if "AgentAction::AgentAction()" not in content:
        # Insert before the namespace closing or after includes
        # Best to insert after includes
        parts = content.split("namespace ai {")
        if len(parts) > 1:
            new_content = parts[0] + "namespace ai {" + implementations + parts[1]
            with open(path, "w") as f:
                f.write(new_content)
            print("Fixed agent_runtime.cc")

if __name__ == "__main__":
    fix_agent_executor()
    fix_agent_runtime_header()
    fix_agent_runtime_cc()

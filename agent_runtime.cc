// Copyright 2025 E-Nation OS. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ai/agent_runtime.h"
#include "base/strings/string_number_conversions.h"
#include "base/logging.h"
#include "base/time/time.h"

namespace ai {

AgentAction::AgentAction() = default;
AgentAction::AgentAction(const AgentAction& other) = default;
AgentAction::~AgentAction() = default;

AuditLogEntry::AuditLogEntry() = default;
AuditLogEntry::AuditLogEntry(const AuditLogEntry& other) = default;
AuditLogEntry::~AuditLogEntry() = default;


AgentRuntime::AgentRuntime() {}

AgentRuntime::~AgentRuntime() {}

void AgentRuntime::ExecuteAgent(std::unique_ptr<Agent> agent) {
  LOG(INFO) << "Executing agent: " << agent->GetId();
  LogAction(agent->GetId(), "EXECUTE", "STARTED");
  
  // In a real implementation, this would spin up a sandbox
  // For now, we just run it directly but wrapped

  agent->Run();
  LogAction(agent->GetId(), "EXECUTE", "COMPLETED");
}

void AgentRuntime::ApproveAction(const AgentAction& action, base::OnceCallback<void(bool)> callback) {
  LOG(INFO) << "Requesting approval for action: " << action.type << " on " << action.target;
  
  // Mock approval logic - in reality this would show a UI dialog
  bool approved = true; // Auto-approve for now in this mock
  
  if (action.requires_approval) {
    // TODO: Trigger UI prompt
    LOG(WARNING) << "Action requires user approval";
  }

  LogAction("SYSTEM", "APPROVE_ACTION", approved ? "APPROVED" : "DENIED");
  std::move(callback).Run(approved);
}

std::vector<AuditLogEntry> AgentRuntime::GetAuditLog() const {
  return audit_log_;
}

void AgentRuntime::LogAction(const std::string& agent_id, const std::string& action, const std::string& status) {
  AuditLogEntry entry;
  entry.timestamp = base::NumberToString(base::Time::Now().ToTimeT());
  entry.agent_id = agent_id;
  entry.action = action;
  entry.status = status;
  
  audit_log_.push_back(entry);
}

} // namespace ai

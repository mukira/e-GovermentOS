// Copyright 2025 E-Nation OS. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_AI_AGENT_RUNTIME_H_
#define CHROME_BROWSER_AI_AGENT_RUNTIME_H_

#include <string>
#include <vector>
#include <memory>
#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"

namespace ai {

struct AgentConfig {
  std::string name;
  std::string model_id;
  bool requires_internet;
  bool requires_filesystem;
};

struct AgentAction {
  AgentAction();
  AgentAction(const AgentAction& other);
  ~AgentAction();
  std::string type;
  std::string target;
  std::string payload;
  bool requires_approval;
};

struct AuditLogEntry {
  AuditLogEntry();
  AuditLogEntry(const AuditLogEntry& other);
  ~AuditLogEntry();
  std::string timestamp;
  std::string agent_id;
  std::string action;
  std::string status;
};

class Agent {
 public:
  virtual ~Agent() = default;
  virtual std::string GetId() const = 0;
  virtual void Run() = 0;
};

class AgentRuntime {
 public:
  AgentRuntime();
  virtual ~AgentRuntime();

  // Executes an agent in a sandboxed environment
  void ExecuteAgent(std::unique_ptr<Agent> agent);

  // Approves a high-risk action requested by an agent
  void ApproveAction(const AgentAction& action, base::OnceCallback<void(bool)> callback);

  // Retrieves the audit log
  std::vector<AuditLogEntry> GetAuditLog() const;

 private:
  void LogAction(const std::string& agent_id, const std::string& action, const std::string& status);
  
  std::vector<AuditLogEntry> audit_log_;
  base::WeakPtrFactory<AgentRuntime> weak_ptr_factory_{this};
};

} // namespace ai

#endif // CHROME_BROWSER_AI_AGENT_RUNTIME_H_

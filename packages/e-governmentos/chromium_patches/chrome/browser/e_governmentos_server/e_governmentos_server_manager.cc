diff --git a/chrome/browser/e_governmentos_server/e_governmentos_server_manager.cc b/chrome/browser/e_governmentos_server/e_governmentos_server_manager.cc
new file mode 100644
index 0000000000000..0a3a2d12d08fc
--- /dev/null
+++ b/chrome/browser/e_governmentos_server/e_governmentos_server_manager.cc
@@ -0,0 +1,1076 @@
+// Copyright 2024 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#include "chrome/browser/e_governmentos_server/e_governmentos_server_manager.h"
+
+#include "base/command_line.h"
+#include "base/files/file_path.h"
+#include "base/files/file_util.h"
+#include "base/json/json_writer.h"
+#include "base/logging.h"
+#include "base/path_service.h"
+#include "base/process/kill.h"
+#include "base/process/launch.h"
+#include "base/rand_util.h"
+#include "base/strings/string_number_conversions.h"
+#include "base/system/sys_info.h"
+#include "base/task/thread_pool.h"
+#include "base/threading/thread_restrictions.h"
+#include "build/build_config.h"
+#include "chrome/browser/browser_process.h"
+
+#if BUILDFLAG(IS_POSIX)
+#include <signal.h>
+#endif
+
+#include "chrome/browser/e_governmentos_server/e_governmentos_server_prefs.h"
+#include "chrome/browser/net/system_network_context_manager.h"
+#include "chrome/browser/profiles/profile.h"
+#include "chrome/browser/profiles/profile_manager.h"
+#include "chrome/common/chrome_paths.h"
+#include "components/metrics/e_governmentos_metrics/e_governmentos_metrics_service.h"
+#include "components/metrics/e_governmentos_metrics/e_governmentos_metrics_service_factory.h"
+#include "components/prefs/pref_change_registrar.h"
+#include "components/prefs/pref_service.h"
+#include "components/version_info/version_info.h"
+#include "content/public/browser/devtools_agent_host.h"
+#include "content/public/browser/devtools_socket_factory.h"
+#include "content/public/browser/storage_partition.h"
+#include "net/base/net_errors.h"
+#include "net/base/port_util.h"
+#include "net/log/net_log_source.h"
+#include "net/socket/tcp_server_socket.h"
+#include "net/traffic_annotation/network_traffic_annotation.h"
+#include "services/network/public/cpp/resource_request.h"
+#include "services/network/public/cpp/simple_url_loader.h"
+#include "services/network/public/mojom/url_loader_factory.mojom.h"
+#include "url/gurl.h"
+
+namespace {
+
+const int kBackLog = 10;
+
+// Helper function to check for command-line port override.
+// Returns the port value if valid override is found, 0 otherwise.
+int GetPortOverrideFromCommandLine(base::CommandLine* command_line,
+                                    const char* switch_name,
+                                    const char* port_name) {
+  if (!command_line->HasSwitch(switch_name)) {
+    return 0;
+  }
+
+  std::string port_str = command_line->GetSwitchValueASCII(switch_name);
+  int port = 0;
+
+  if (!base::StringToInt(port_str, &port) || !net::IsPortValid(port) ||
+      port <= 0) {
+    LOG(WARNING) << "e_governmentos: Invalid " << port_name
+                 << " specified on command line: " << port_str
+                 << " (must be 1-65535)";
+    return 0;
+  }
+
+  // Warn about problematic ports but respect explicit user intent
+  if (net::IsWellKnownPort(port)) {
+    LOG(WARNING) << "e_governmentos: " << port_name << " " << port
+                 << " is well-known (0-1023) and may require elevated "
+                    "privileges";
+  }
+  if (!net::IsPortAllowedForScheme(port, "http")) {
+    LOG(WARNING) << "e_governmentos: " << port_name << " " << port
+                 << " is restricted by Chromium (may interfere with system "
+                    "services)";
+  }
+
+  LOG(INFO) << "e_governmentos: " << port_name
+            << " overridden via command line: " << port;
+  return port;
+}
+
+// Launches the e-GovernmentOS server process on a background thread.
+// This function performs blocking I/O operations (PathExists, LaunchProcess).
+base::Process LaunchProcessOnBackgroundThread(
+    const base::FilePath& exe_path,
+    const base::FilePath& resources_dir,
+    const base::FilePath& execution_dir,
+    uint16_t cdp_port,
+    uint16_t mcp_port,
+    uint16_t agent_port,
+    uint16_t extension_port) {
+  // Check if executable exists (blocking I/O)
+  if (!base::PathExists(exe_path)) {
+    LOG(ERROR) << "e_governmentos: e-GovernmentOS server executable not found at: "
+               << exe_path;
+    return base::Process();
+  }
+
+  if (execution_dir.empty()) {
+    LOG(ERROR) << "e_governmentos: Execution directory path is empty";
+    return base::Process();
+  }
+
+  // Ensure execution directory exists (blocking I/O)
+  if (!base::CreateDirectory(execution_dir)) {
+    LOG(ERROR) << "e_governmentos: Failed to create execution directory at: "
+               << execution_dir;
+    return base::Process();
+  }
+
+  // Build command line
+  base::CommandLine cmd(exe_path);
+  cmd.AppendSwitchASCII("cdp-port", base::NumberToString(cdp_port));
+  cmd.AppendSwitchASCII("http-mcp-port", base::NumberToString(mcp_port));
+  cmd.AppendSwitchASCII("agent-port", base::NumberToString(agent_port));
+  cmd.AppendSwitchASCII("extension-port", base::NumberToString(extension_port));
+  cmd.AppendSwitchPath("resources-dir", resources_dir);
+  cmd.AppendSwitchPath("execution-dir", execution_dir);
+
+  // Set up launch options
+  base::LaunchOptions options;
+#if BUILDFLAG(IS_WIN)
+  options.start_hidden = true;
+#endif
+
+  // Launch the process (blocking I/O)
+  return base::LaunchProcess(cmd, options);
+}
+
+// Factory for creating TCP server sockets for CDP
+class CDPServerSocketFactory : public content::DevToolsSocketFactory {
+ public:
+  explicit CDPServerSocketFactory(uint16_t port) : port_(port) {}
+
+  CDPServerSocketFactory(const CDPServerSocketFactory&) = delete;
+  CDPServerSocketFactory& operator=(const CDPServerSocketFactory&) = delete;
+
+ private:
+  std::unique_ptr<net::ServerSocket> CreateLocalHostServerSocket(int port) {
+    std::unique_ptr<net::ServerSocket> socket(
+        new net::TCPServerSocket(nullptr, net::NetLogSource()));
+    if (socket->ListenWithAddressAndPort("127.0.0.1", port, kBackLog) ==
+        net::OK) {
+      return socket;
+    }
+    if (socket->ListenWithAddressAndPort("::1", port, kBackLog) == net::OK) {
+      return socket;
+    }
+    return nullptr;
+  }
+
+  // content::DevToolsSocketFactory implementation
+  std::unique_ptr<net::ServerSocket> CreateForHttpServer() override {
+    return CreateLocalHostServerSocket(port_);
+  }
+
+  std::unique_ptr<net::ServerSocket> CreateForTethering(
+      std::string* name) override {
+    return nullptr;  // Tethering not needed for e-GovernmentOS
+  }
+
+  uint16_t port_;
+};
+
+}  // namespace
+
+namespace e_governmentos {
+
+// static
+e-GovernmentOSServerManager* e-GovernmentOSServerManager::GetInstance() {
+  static base::NoDestructor<e-GovernmentOSServerManager> instance;
+  return instance.get();
+}
+
+e-GovernmentOSServerManager::e-GovernmentOSServerManager() = default;
+
+e-GovernmentOSServerManager::~e-GovernmentOSServerManager() {
+  Shutdown();
+}
+
+bool e-GovernmentOSServerManager::AcquireLock() {
+  // Allow blocking for lock file operations (short-duration I/O)
+  base::ScopedAllowBlocking allow_blocking;
+
+  base::FilePath exec_dir = Gete-GovernmentOSExecutionDir();
+  if (exec_dir.empty()) {
+    LOG(ERROR) << "e_governmentos: Failed to resolve execution directory for lock";
+    return false;
+  }
+
+  base::FilePath lock_path = exec_dir.Append(FILE_PATH_LITERAL("server.lock"));
+
+  lock_file_ = base::File(lock_path,
+                          base::File::FLAG_OPEN_ALWAYS |
+                          base::File::FLAG_READ |
+                          base::File::FLAG_WRITE);
+
+  if (!lock_file_.IsValid()) {
+    LOG(ERROR) << "e_governmentos: Failed to open lock file: " << lock_path;
+    return false;
+  }
+
+  base::File::Error lock_error =
+      lock_file_.Lock(base::File::LockMode::kExclusive);
+  if (lock_error != base::File::FILE_OK) {
+    LOG(INFO) << "e_governmentos: Server already running in another Chrome process "
+              << "(lock file: " << lock_path << ")";
+    lock_file_.Close();
+    return false;
+  }
+
+  LOG(INFO) << "e_governmentos: Acquired exclusive lock on " << lock_path;
+  return true;
+}
+
+void e-GovernmentOSServerManager::InitializePortsAndPrefs() {
+  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
+  PrefService* prefs = g_browser_process->local_state();
+
+  // STEP 1: Read from prefs or use defaults
+  if (!prefs) {
+    cdp_port_ = e_governmentos_server::kDefaultCDPPort;
+    mcp_port_ = e_governmentos_server::kDefaultMCPPort;
+    agent_port_ = e_governmentos_server::kDefaultAgentPort;
+    extension_port_ = e_governmentos_server::kDefaultExtensionPort;
+    mcp_enabled_ = true;
+  } else {
+    cdp_port_ = prefs->GetInteger(e_governmentos_server::kCDPServerPort);
+    if (cdp_port_ <= 0) {
+      cdp_port_ = e_governmentos_server::kDefaultCDPPort;
+    }
+
+    mcp_port_ = prefs->GetInteger(e_governmentos_server::kMCPServerPort);
+    if (mcp_port_ <= 0) {
+      mcp_port_ = e_governmentos_server::kDefaultMCPPort;
+    }
+
+    agent_port_ = prefs->GetInteger(e_governmentos_server::kAgentServerPort);
+    if (agent_port_ <= 0) {
+      agent_port_ = e_governmentos_server::kDefaultAgentPort;
+    }
+
+    extension_port_ = prefs->GetInteger(e_governmentos_server::kExtensionServerPort);
+    if (extension_port_ <= 0) {
+      extension_port_ = e_governmentos_server::kDefaultExtensionPort;
+    }
+
+    mcp_enabled_ = prefs->GetBoolean(e_governmentos_server::kMCPServerEnabled);
+
+    // Set up pref change observers
+    if (!pref_change_registrar_) {
+      pref_change_registrar_ = std::make_unique<PrefChangeRegistrar>();
+      pref_change_registrar_->Init(prefs);
+      pref_change_registrar_->Add(
+          e_governmentos_server::kMCPServerEnabled,
+          base::BindRepeating(&e-GovernmentOSServerManager::OnMCPEnabledChanged,
+                              base::Unretained(this)));
+      pref_change_registrar_->Add(
+          e_governmentos_server::kRestartServerRequested,
+          base::BindRepeating(&e-GovernmentOSServerManager::OnRestartServerRequestedChanged,
+                              base::Unretained(this)));
+    }
+  }
+  cdp_port_ = FindAvailablePort(cdp_port_);
+  mcp_port_ = FindAvailablePort(mcp_port_);
+  agent_port_ = FindAvailablePort(agent_port_);
+  extension_port_ = FindAvailablePort(extension_port_);
+
+  // STEP 3: Apply command-line overrides (these take highest priority)
+  int cdp_override = GetPortOverrideFromCommandLine(
+      command_line, "e_governmentos-cdp-port", "CDP port");
+  if (cdp_override > 0) {
+    cdp_port_ = cdp_override;
+  }
+
+  int mcp_override = GetPortOverrideFromCommandLine(
+      command_line, "e_governmentos-mcp-port", "MCP port");
+  if (mcp_override > 0) {
+    mcp_port_ = mcp_override;
+    mcp_enabled_ = true;  // Implicit enable when port specified
+  }
+
+  int agent_override = GetPortOverrideFromCommandLine(
+      command_line, "e_governmentos-agent-port", "Agent port");
+  if (agent_override > 0) {
+    agent_port_ = agent_override;
+  }
+
+  int extension_override = GetPortOverrideFromCommandLine(
+      command_line, "e_governmentos-extension-port", "Extension port");
+  if (extension_override > 0) {
+    extension_port_ = extension_override;
+  }
+
+  LOG(INFO) << "e_governmentos: Final ports - CDP: " << cdp_port_
+            << ", MCP: " << mcp_port_ << ", Agent: " << agent_port_
+            << ", Extension: " << extension_port_;
+}
+
+void e-GovernmentOSServerManager::SavePortsToPrefs() {
+  PrefService* prefs = g_browser_process->local_state();
+  if (!prefs) {
+    LOG(WARNING) << "e_governmentos: SavePortsToPrefs - no prefs available, skipping save";
+    return;
+  }
+
+  prefs->SetInteger(e_governmentos_server::kCDPServerPort, cdp_port_);
+  prefs->SetInteger(e_governmentos_server::kMCPServerPort, mcp_port_);
+  prefs->SetInteger(e_governmentos_server::kAgentServerPort, agent_port_);
+  prefs->SetInteger(e_governmentos_server::kExtensionServerPort, extension_port_);
+  prefs->SetBoolean(e_governmentos_server::kMCPServerEnabled, mcp_enabled_);
+
+  LOG(INFO) << "e_governmentos: Saving to prefs - CDP: " << cdp_port_
+            << ", MCP: " << mcp_port_ << ", Agent: " << agent_port_
+            << ", Extension: " << extension_port_
+            << ", MCP enabled: " << (mcp_enabled_ ? "true" : "false");
+}
+
+void e-GovernmentOSServerManager::Start() {
+  if (is_running_) {
+    LOG(INFO) << "e_governmentos: e-GovernmentOS server already running";
+    return;
+  }
+
+  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
+  // Initialize and finalize ports, even with e_governmentos-server disabled
+  // we want to update the prefs from CLI
+  InitializePortsAndPrefs();
+  SavePortsToPrefs();
+
+  if (command_line->HasSwitch("disable-e_governmentos-server")) {
+    LOG(INFO) << "e_governmentos: e-GovernmentOS server disabled via command line";
+    return;
+  }
+
+  // Try to acquire system-wide lock
+  if (!AcquireLock()) {
+    return;  // Another Chrome process already owns the server
+  }
+
+  LOG(INFO) << "e_governmentos: Starting e-GovernmentOS server";
+
+
+  // Start servers and process
+  StartCDPServer();
+  Launche-GovernmentOSProcess();
+
+  health_check_timer_.Start(FROM_HERE, base::Seconds(60), this,
+                            &e-GovernmentOSServerManager::CheckServerHealth);
+}
+
+void e-GovernmentOSServerManager::Stop() {
+  if (!is_running_) {
+    return;
+  }
+
+  LOG(INFO) << "e_governmentos: Stopping e-GovernmentOS server";
+  health_check_timer_.Stop();
+  process_check_timer_.Stop();
+
+  Terminatee-GovernmentOSProcess();
+  StopCDPServer();
+
+  // Release lock
+  if (lock_file_.IsValid()) {
+    lock_file_.Unlock();
+    lock_file_.Close();
+    LOG(INFO) << "e_governmentos: Released lock file";
+  }
+}
+
+bool e-GovernmentOSServerManager::IsRunning() const {
+  return is_running_ && process_.IsValid();
+}
+
+void e-GovernmentOSServerManager::Shutdown() {
+  Stop();
+}
+
+void e-GovernmentOSServerManager::StartCDPServer() {
+  LOG(INFO) << "e_governmentos: Starting CDP server on port " << cdp_port_;
+
+  content::DevToolsAgentHost::StartRemoteDebuggingServer(
+      std::make_unique<CDPServerSocketFactory>(cdp_port_),
+      base::FilePath(),
+      base::FilePath());
+
+  LOG(INFO) << "e_governmentos: CDP WebSocket server started at ws://127.0.0.1:"
+            << cdp_port_;
+  LOG(INFO) << "e_governmentos: MCP server port: " << mcp_port_
+            << " (enabled: " << (mcp_enabled_ ? "true" : "false") << ")";
+  LOG(INFO) << "e_governmentos: Agent server port: " << agent_port_;
+  LOG(INFO) << "e_governmentos: Extension server port: " << extension_port_;
+}
+
+void e-GovernmentOSServerManager::StopCDPServer() {
+  if (cdp_port_ == 0) {
+    return;
+  }
+
+  LOG(INFO) << "e_governmentos: Stopping CDP server";
+  content::DevToolsAgentHost::StopRemoteDebuggingServer();
+  cdp_port_ = 0;
+}
+
+void e-GovernmentOSServerManager::Launche-GovernmentOSProcess() {
+  base::FilePath exe_path = Gete-GovernmentOSServerExecutablePath();
+  base::FilePath resources_dir = Gete-GovernmentOSServerResourcesPath();
+  base::FilePath execution_dir = Gete-GovernmentOSExecutionDir();
+  if (execution_dir.empty()) {
+    LOG(ERROR) << "e_governmentos: Failed to resolve execution directory";
+    StopCDPServer();
+    return;
+  }
+
+  LOG(INFO) << "e_governmentos: Launching server - binary: " << exe_path;
+  LOG(INFO) << "e_governmentos: Launching server - resources: " << resources_dir;
+  LOG(INFO) << "e_governmentos: Launching server - execution dir: " << execution_dir;
+
+  // Capture values to pass to background thread
+  uint16_t cdp_port = cdp_port_;
+  uint16_t mcp_port = mcp_port_;
+  uint16_t agent_port = agent_port_;
+  uint16_t extension_port = extension_port_;
+
+  // Post blocking work to background thread, get result back on UI thread
+  base::ThreadPool::PostTaskAndReplyWithResult(
+      FROM_HERE, {base::MayBlock(), base::TaskPriority::USER_BLOCKING},
+      base::BindOnce(&LaunchProcessOnBackgroundThread, exe_path, resources_dir,
+                     execution_dir, cdp_port, mcp_port, agent_port,
+                     extension_port),
+      base::BindOnce(&e-GovernmentOSServerManager::OnProcessLaunched,
+                     weak_factory_.GetWeakPtr()));
+}
+
+void e-GovernmentOSServerManager::OnProcessLaunched(base::Process process) {
+  if (!process.IsValid()) {
+    LOG(ERROR) << "e_governmentos: Failed to launch e-GovernmentOS server";
+    StopCDPServer();
+    is_restarting_ = false;
+    return;
+  }
+
+  process_ = std::move(process);
+  is_running_ = true;
+
+  LOG(INFO) << "e_governmentos: e-GovernmentOS server started";
+  LOG(INFO) << "e_governmentos: CDP port: " << cdp_port_;
+  LOG(INFO) << "e_governmentos: MCP port: " << mcp_port_;
+  LOG(INFO) << "e_governmentos: Agent port: " << agent_port_;
+  LOG(INFO) << "e_governmentos: Extension port: " << extension_port_;
+
+  process_check_timer_.Start(FROM_HERE, base::Seconds(5), this,
+                             &e-GovernmentOSServerManager::CheckProcessStatus);
+
+  // Reset restart flag and pref after successful launch
+  if (is_restarting_) {
+    is_restarting_ = false;
+    PrefService* prefs = g_browser_process->local_state();
+    if (prefs && prefs->GetBoolean(e_governmentos_server::kRestartServerRequested)) {
+      prefs->SetBoolean(e_governmentos_server::kRestartServerRequested, false);
+      LOG(INFO) << "e_governmentos: Restart completed, reset restart_requested pref";
+    }
+  }
+
+  // /init will be sent after first successful periodic health check
+
+  // If MCP is disabled, send control request to disable it
+  if (!mcp_enabled_) {
+    SendMCPControlRequest(false);
+  }
+}
+
+void e-GovernmentOSServerManager::Terminatee-GovernmentOSProcess() {
+  if (!process_.IsValid()) {
+    return;
+  }
+
+  LOG(INFO) << "e_governmentos: Force killing e-GovernmentOS server process (PID: "
+            << process_.Pid() << ")";
+
+  // Reset init flag so it gets sent again after restart
+  init_request_sent_ = false;
+
+  // sync primitives is needed for process termination. 
+  // NOTE: only run on background threads
+  base::ScopedAllowBaseSyncPrimitives allow_sync;
+  base::ScopedAllowBlocking allow_blocking;
+
+#if BUILDFLAG(IS_POSIX)
+  // POSIX: Send SIGKILL for immediate termination (no graceful shutdown)
+  // This matches Windows TerminateProcess behavior
+  base::ProcessId pid = process_.Pid();
+  if (kill(pid, SIGKILL) == 0) {
+    int exit_code = 0;
+    if (process_.WaitForExit(&exit_code)) {
+      LOG(INFO) << "e_governmentos: Process killed successfully with SIGKILL";
+    } else {
+      LOG(WARNING) << "e_governmentos: SIGKILL sent but WaitForExit failed";
+    }
+  } else {
+    PLOG(ERROR) << "e_governmentos: Failed to send SIGKILL to PID " << pid;
+  }
+#else
+  // Windows: TerminateProcess is already immediate force kill
+  bool terminated = process_.Terminate(0, true);
+  if (terminated) {
+    LOG(INFO) << "e_governmentos: Process terminated successfully";
+  } else {
+    LOG(ERROR) << "e_governmentos: Failed to terminate process";
+  }
+#endif
+
+  is_running_ = false;
+}
+
+void e-GovernmentOSServerManager::OnProcessExited(int exit_code) {
+  LOG(INFO) << "e_governmentos: e-GovernmentOS server exited with code: " << exit_code;
+  is_running_ = false;
+
+  // Stop CDP server since e-GovernmentOS process is gone
+  StopCDPServer();
+
+  // Restart if it crashed unexpectedly
+  if (exit_code != 0) {
+    LOG(WARNING) << "e_governmentos: e-GovernmentOS server crashed, restarting...";
+    Start();
+  }
+}
+
+void e-GovernmentOSServerManager::CheckServerHealth() {
+  if (!is_running_) {
+    return;
+  }
+
+  // First check if process is still alive
+  if (!process_.IsValid()) {
+    LOG(WARNING) << "e_governmentos: e-GovernmentOS server process is invalid, restarting...";
+    Restarte-GovernmentOSProcess();
+    return;
+  }
+
+  // Build health check URL
+  GURL health_url("http://127.0.0.1:" + base::NumberToString(mcp_port_) + "/health");
+
+  // Create network traffic annotation
+  net::NetworkTrafficAnnotationTag traffic_annotation =
+      net::DefineNetworkTrafficAnnotation("e_governmentos_health_check", R"(
+        semantics {
+          sender: "e-GovernmentOS Server Manager"
+          description:
+            "Checks if the e-GovernmentOS MCP server is healthy by querying its "
+            "/health endpoint."
+          trigger: "Periodic health check every 60 seconds while server is running."
+          data: "No user data sent, just an HTTP GET request."
+          destination: LOCAL
+        }
+        policy {
+          cookies_allowed: NO
+          setting: "This feature cannot be disabled by settings."
+          policy_exception_justification:
+            "Internal health check for e-GovernmentOS server functionality."
+        })");
+
+  // Create resource request
+  auto resource_request = std::make_unique<network::ResourceRequest>();
+  resource_request->url = health_url;
+  resource_request->method = "GET";
+  resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;
+
+  // Create URL loader with 10 second timeout
+  auto url_loader = network::SimpleURLLoader::Create(
+      std::move(resource_request), traffic_annotation);
+  url_loader->SetTimeoutDuration(base::Seconds(10));
+
+  // Get URL loader factory from default storage partition
+  auto* url_loader_factory =
+      g_browser_process->system_network_context_manager()
+          ->GetURLLoaderFactory();
+
+  // Keep a raw pointer for the callback
+  auto* url_loader_ptr = url_loader.get();
+
+  // Download response
+  url_loader_ptr->DownloadHeadersOnly(
+      url_loader_factory,
+      base::BindOnce(&e-GovernmentOSServerManager::OnHealthCheckComplete,
+                     weak_factory_.GetWeakPtr(), std::move(url_loader)));
+}
+
+void e-GovernmentOSServerManager::CheckProcessStatus() {
+  if (!is_running_ || !process_.IsValid()) {
+    return;
+  }
+
+  // Check if process has exited
+  int exit_code = 0;
+  if (process_.WaitForExitWithTimeout(base::TimeDelta(), &exit_code)) {
+    // Process has exited
+    OnProcessExited(exit_code);
+  }
+}
+
+void e-GovernmentOSServerManager::OnHealthCheckComplete(
+    std::unique_ptr<network::SimpleURLLoader> url_loader,
+    scoped_refptr<net::HttpResponseHeaders> headers) {
+  if (!is_running_) {
+    return;
+  }
+
+  // Check if we got a valid response
+  int response_code = 0;
+  if (headers) {
+    response_code = headers->response_code();
+  }
+
+  if (response_code == 200) {
+    // Health check passed
+    LOG(INFO) << "e_governmentos: Health check passed";
+
+    // Send /init request on first successful health check
+    if (!init_request_sent_) {
+      init_request_sent_ = true;
+      SendInitRequest();
+    }
+    return;
+  }
+
+  // Health check failed
+  int net_error = url_loader->NetError();
+  LOG(WARNING) << "e_governmentos: Health check failed - HTTP " << response_code
+               << ", net error: " << net::ErrorToString(net_error)
+               << ", restarting e-GovernmentOS server process...";
+
+  Restarte-GovernmentOSProcess();
+}
+
+void e-GovernmentOSServerManager::Restarte-GovernmentOSProcess() {
+  LOG(INFO) << "e_governmentos: Restarting e-GovernmentOS server process";
+
+  // Stop the process and monitoring
+  process_check_timer_.Stop();
+  Terminatee-GovernmentOSProcess();
+
+  // Relaunch the process
+  Launche-GovernmentOSProcess();
+}
+
+void e-GovernmentOSServerManager::OnMCPEnabledChanged() {
+  if (!is_running_) {
+    return;
+  }
+
+  PrefService* prefs = g_browser_process->local_state();
+  if (!prefs) {
+    return;
+  }
+
+  bool new_value = prefs->GetBoolean(e_governmentos_server::kMCPServerEnabled);
+
+  if (new_value != mcp_enabled_) {
+    LOG(INFO) << "e_governmentos: MCP enabled preference changed from "
+              << (mcp_enabled_ ? "true" : "false") << " to "
+              << (new_value ? "true" : "false");
+
+    mcp_enabled_ = new_value;
+    SendMCPControlRequest(new_value);
+  }
+}
+
+void e-GovernmentOSServerManager::OnRestartServerRequestedChanged() {
+  PrefService* prefs = g_browser_process->local_state();
+  if (!prefs) {
+    return;
+  }
+
+  bool restart_requested = prefs->GetBoolean(e_governmentos_server::kRestartServerRequested);
+
+  // Only process if pref is set to true
+  if (!restart_requested) {
+    return;
+  }
+
+  // Ignore if already restarting (prevents thrashing from UI spam)
+  if (is_restarting_) {
+    LOG(INFO) << "e_governmentos: Restart already in progress, ignoring duplicate request";
+    return;
+  }
+
+  // Ignore if not running
+  if (!is_running_) {
+    LOG(WARNING) << "e_governmentos: Cannot restart - server is not running";
+    // Reset pref anyway
+    prefs->SetBoolean(e_governmentos_server::kRestartServerRequested, false);
+    return;
+  }
+
+  LOG(INFO) << "e_governmentos: Server restart requested via preference";
+  is_restarting_ = true;
+
+  // Stop timer now (must be on UI thread)
+  process_check_timer_.Stop();
+
+  // Capture UI task runner to post back after background work
+  auto ui_task_runner = base::SequencedTaskRunner::GetCurrentDefault();
+
+  // Kill process on background thread, then relaunch on UI thread
+  base::ThreadPool::PostTask(
+      FROM_HERE, {base::MayBlock(), base::TaskPriority::USER_BLOCKING},
+      base::BindOnce(
+          [](e-GovernmentOSServerManager* manager,
+             scoped_refptr<base::SequencedTaskRunner> ui_runner) {
+            // Kill old process and wait for exit (blocking, safe on background)
+            manager->Terminatee-GovernmentOSProcess();
+
+            // Post back to UI thread to launch new process
+            ui_runner->PostTask(
+                FROM_HERE,
+                base::BindOnce(&e-GovernmentOSServerManager::Launche-GovernmentOSProcess,
+                               base::Unretained(manager)));
+          },
+          base::Unretained(this), ui_task_runner));
+}
+
+void e-GovernmentOSServerManager::SendMCPControlRequest(bool enabled) {
+  if (!is_running_) {
+    return;
+  }
+
+  GURL control_url("http://127.0.0.1:" + base::NumberToString(mcp_port_) +
+                   "/mcp/control");
+
+  net::NetworkTrafficAnnotationTag traffic_annotation =
+      net::DefineNetworkTrafficAnnotation("e_governmentos_mcp_control", R"(
+        semantics {
+          sender: "e-GovernmentOS Server Manager"
+          description:
+            "Sends control command to e-GovernmentOS MCP server to enable/disable "
+            "the MCP protocol at runtime."
+          trigger: "User changes MCP enabled preference."
+          data: "JSON payload with enabled state: {\"enabled\": true/false}"
+          destination: LOCAL
+        }
+        policy {
+          cookies_allowed: NO
+          setting: "This feature cannot be disabled by settings."
+          policy_exception_justification:
+            "Internal control request for e-GovernmentOS server functionality."
+        })");
+
+  auto resource_request = std::make_unique<network::ResourceRequest>();
+  resource_request->url = control_url;
+  resource_request->method = "POST";
+  resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;
+  resource_request->headers.SetHeader("Content-Type", "application/json");
+
+  std::string json_body = enabled ? "{\"enabled\":true}" : "{\"enabled\":false}";
+
+  auto url_loader = network::SimpleURLLoader::Create(
+      std::move(resource_request), traffic_annotation);
+  url_loader->AttachStringForUpload(json_body, "application/json");
+  url_loader->SetTimeoutDuration(base::Seconds(10));
+
+  auto* url_loader_factory =
+      g_browser_process->system_network_context_manager()
+          ->GetURLLoaderFactory();
+
+  auto* url_loader_ptr = url_loader.get();
+
+  url_loader_ptr->DownloadHeadersOnly(
+      url_loader_factory,
+      base::BindOnce(&e-GovernmentOSServerManager::OnMCPControlRequestComplete,
+                     weak_factory_.GetWeakPtr(), enabled,
+                     std::move(url_loader)));
+
+  LOG(INFO) << "e_governmentos: Sent MCP control request: {\"enabled\": "
+            << (enabled ? "true" : "false") << "}";
+}
+
+void e-GovernmentOSServerManager::OnMCPControlRequestComplete(
+    bool requested_state,
+    std::unique_ptr<network::SimpleURLLoader> url_loader,
+    scoped_refptr<net::HttpResponseHeaders> headers) {
+  if (!is_running_) {
+    return;
+  }
+
+  int response_code = 0;
+  if (headers) {
+    response_code = headers->response_code();
+  }
+
+  if (response_code == 200) {
+    LOG(INFO) << "e_governmentos: MCP control request succeeded - MCP server is now "
+              << (requested_state ? "enabled" : "disabled");
+    return;
+  }
+
+  int net_error = url_loader->NetError();
+  LOG(ERROR) << "e_governmentos: MCP control request failed - HTTP " << response_code
+             << ", net error: " << net::ErrorToString(net_error);
+}
+
+void e-GovernmentOSServerManager::SendInitRequest() {
+  if (!is_running_) {
+    return;
+  }
+
+  // Get the default profile to access e-GovernmentOSMetricsService
+  ProfileManager* profile_manager = g_browser_process->profile_manager();
+  if (!profile_manager) {
+    LOG(ERROR) << "e_governmentos: Failed to get ProfileManager for /init request";
+    return;
+  }
+
+  Profile* profile = profile_manager->GetLastUsedProfileIfLoaded();
+  if (!profile || profile->IsOffTheRecord()) {
+    LOG(WARNING) << "e_governmentos: No valid profile available for /init request";
+    return;
+  }
+
+  // Get e-GovernmentOSMetricsService to retrieve install_id
+  e_governmentos_metrics::e-GovernmentOSMetricsService* metrics_service =
+      e_governmentos_metrics::e-GovernmentOSMetricsServiceFactory::GetForBrowserContext(
+          profile);
+  if (!metrics_service) {
+    LOG(ERROR) << "e_governmentos: Failed to get e-GovernmentOSMetricsService for /init "
+                  "request";
+    return;
+  }
+
+  // Build the /init payload
+  base::Value::Dict payload;
+  payload.Set("client_id", metrics_service->GetInstallId());
+  payload.Set("version", version_info::GetVersionNumber());
+  payload.Set("os", base::SysInfo::OperatingSystemName());
+  payload.Set("arch", base::SysInfo::OperatingSystemArchitecture());
+
+  std::string json_payload;
+  if (!base::JSONWriter::Write(payload, &json_payload)) {
+    LOG(ERROR) << "e_governmentos: Failed to serialize /init payload";
+    return;
+  }
+
+  GURL init_url("http://127.0.0.1:" + base::NumberToString(mcp_port_) +
+                "/init");
+
+  net::NetworkTrafficAnnotationTag traffic_annotation =
+      net::DefineNetworkTrafficAnnotation("e_governmentos_server_init", R"(
+        semantics {
+          sender: "e-GovernmentOS Server Manager"
+          description:
+            "Sends initialization metadata to e-GovernmentOS MCP server including "
+            "install ID, browser version, OS, and architecture."
+          trigger: "e-GovernmentOS server process successfully launched."
+          data:
+            "JSON payload with install_id, version, os, and arch. No PII."
+          destination: LOCAL
+        }
+        policy {
+          cookies_allowed: NO
+          setting: "This feature cannot be disabled by settings."
+          policy_exception_justification:
+            "Internal initialization for e-GovernmentOS server functionality."
+        })");
+
+  auto resource_request = std::make_unique<network::ResourceRequest>();
+  resource_request->url = init_url;
+  resource_request->method = "POST";
+  resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;
+  resource_request->headers.SetHeader("Content-Type", "application/json");
+
+  auto url_loader = network::SimpleURLLoader::Create(
+      std::move(resource_request), traffic_annotation);
+  url_loader->AttachStringForUpload(json_payload, "application/json");
+  url_loader->SetTimeoutDuration(base::Seconds(10));
+
+  auto* url_loader_factory =
+      g_browser_process->system_network_context_manager()
+          ->GetURLLoaderFactory();
+
+  auto* url_loader_ptr = url_loader.get();
+
+  url_loader_ptr->DownloadHeadersOnly(
+      url_loader_factory,
+      base::BindOnce(&e-GovernmentOSServerManager::OnInitRequestComplete,
+                     weak_factory_.GetWeakPtr(), std::move(url_loader)));
+
+  LOG(INFO) << "e_governmentos: Sent /init request to MCP server";
+}
+
+void e-GovernmentOSServerManager::OnInitRequestComplete(
+    std::unique_ptr<network::SimpleURLLoader> url_loader,
+    scoped_refptr<net::HttpResponseHeaders> headers) {
+  if (!is_running_) {
+    return;
+  }
+
+  int response_code = 0;
+  if (headers) {
+    response_code = headers->response_code();
+  }
+
+  if (response_code == 200) {
+    LOG(INFO) << "e_governmentos: /init request succeeded";
+    return;
+  }
+
+  int net_error = url_loader->NetError();
+  LOG(WARNING) << "e_governmentos: /init request failed - HTTP " << response_code
+               << ", net error: " << net::ErrorToString(net_error);
+}
+
+int e-GovernmentOSServerManager::FindAvailablePort(int starting_port) {
+  const int kMaxPortAttempts = 100;
+  const int kMaxPort = 65535;
+
+  LOG(INFO) << "e_governmentos: Finding port starting from "
+            << starting_port;
+
+  for (int i = 0; i < kMaxPortAttempts; i++) {
+    int port_to_try = starting_port + i;
+
+    // Don't exceed max valid port number
+    if (port_to_try > kMaxPort) {
+      break;
+    }
+
+    if (IsPortAvailable(port_to_try)) {
+      if (port_to_try != starting_port) {
+        LOG(INFO) << "e_governmentos: Port " << starting_port
+                  << " was in use, using " << port_to_try << " instead";
+      } else {
+        LOG(INFO) << "e_governmentos: Using port " << port_to_try;
+      }
+      return port_to_try;
+    }
+  }
+
+  // Fallback to starting port if we couldn't find anything
+  LOG(WARNING) << "e_governmentos: Could not find available port after "
+               << kMaxPortAttempts
+               << " attempts, using " << starting_port << " anyway";
+  return starting_port;
+}
+
+bool e-GovernmentOSServerManager::IsPortAvailable(int port) {
+  // Check port is in valid range
+  if (!net::IsPortValid(port) || port == 0) {
+    return false;
+  }
+
+  // Avoid well-known ports (0-1023, require elevated privileges)
+  if (net::IsWellKnownPort(port)) {
+    return false;
+  }
+
+  // Avoid restricted ports (could interfere with system services)
+  if (!net::IsPortAllowedForScheme(port, "http")) {
+    return false;
+  }
+
+  // Try to bind to both IPv4 and IPv6 localhost
+  // If EITHER is in use, the port is NOT available
+  std::unique_ptr<net::TCPServerSocket> socket(
+      new net::TCPServerSocket(nullptr, net::NetLogSource()));
+
+  // Try binding to IPv4 localhost
+  int result = socket->ListenWithAddressAndPort("127.0.0.1", port, 1);
+  if (result != net::OK) {
+    return false;  // IPv4 port is in use
+  }
+
+  // Try binding to IPv6 localhost
+  std::unique_ptr<net::TCPServerSocket> socket6(
+      new net::TCPServerSocket(nullptr, net::NetLogSource()));
+  int result6 = socket6->ListenWithAddressAndPort("::1", port, 1);
+  if (result6 != net::OK) {
+    return false;  // IPv6 port is in use
+  }
+
+  return true;
+}
+
+base::FilePath e-GovernmentOSServerManager::Gete-GovernmentOSServerResourcesPath() const {
+  // Check for command-line override first
+  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
+  if (command_line->HasSwitch("e_governmentos-server-resources-dir")) {
+    base::FilePath custom_path =
+        command_line->GetSwitchValuePath("e_governmentos-server-resources-dir");
+    LOG(INFO) << "e_governmentos: Using custom resources dir from command line: "
+              << custom_path;
+    return custom_path;
+  }
+
+  base::FilePath exe_dir;
+
+#if BUILDFLAG(IS_MAC)
+  // On macOS, the binary will be in the app bundle
+  if (!base::PathService::Get(base::DIR_EXE, &exe_dir)) {
+    LOG(ERROR) << "e_governmentos: Failed to get executable directory";
+    return base::FilePath();
+  }
+
+  // Navigate to Resources folder in the app bundle
+  // Chrome.app/Contents/MacOS -> Chrome.app/Contents/Resources
+  exe_dir = exe_dir.DirName().Append("Resources");
+
+#elif BUILDFLAG(IS_WIN)
+  // On Windows, installer places e-GovernmentOS Server under the versioned directory
+  if (!base::PathService::Get(base::DIR_EXE, &exe_dir)) {
+    LOG(ERROR) << "e_governmentos: Failed to get executable directory";
+    return base::FilePath();
+  }
+  // Append version directory (chrome.release places e-GovernmentOSServer under versioned dir)
+  exe_dir = exe_dir.AppendASCII(version_info::GetVersionNumber());
+
+#elif BUILDFLAG(IS_LINUX)
+  // On Linux, binary is in the same directory as chrome
+  if (!base::PathService::Get(base::DIR_EXE, &exe_dir)) {
+    LOG(ERROR) << "e_governmentos: Failed to get executable directory";
+    return base::FilePath();
+  }
+#endif
+
+  // Return path to resources directory
+  return exe_dir.Append(FILE_PATH_LITERAL("e-GovernmentOSServer"))
+      .Append(FILE_PATH_LITERAL("default"))
+      .Append(FILE_PATH_LITERAL("resources"));
+}
+
+base::FilePath e-GovernmentOSServerManager::Gete-GovernmentOSExecutionDir() const {
+  base::FilePath user_data_dir;
+  if (!base::PathService::Get(chrome::DIR_USER_DATA, &user_data_dir)) {
+    LOG(ERROR) << "e_governmentos: Failed to resolve DIR_USER_DATA path";
+    return base::FilePath();
+  }
+
+  base::FilePath exec_dir = user_data_dir.Append(FILE_PATH_LITERAL(".e_governmentos"));
+
+  // Ensure directory exists before returning
+  base::ScopedAllowBlocking allow_blocking;
+  if (!base::PathExists(exec_dir)) {
+    if (!base::CreateDirectory(exec_dir)) {
+      LOG(ERROR) << "e_governmentos: Failed to create execution directory: " << exec_dir;
+      return base::FilePath();
+    }
+  }
+
+  LOG(INFO) << "e_governmentos: Using execution directory: " << exec_dir;
+  return exec_dir;
+}
+
+base::FilePath e-GovernmentOSServerManager::Gete-GovernmentOSServerExecutablePath() const {
+  base::FilePath e_governmentos_exe =
+      Gete-GovernmentOSServerResourcesPath()
+          .Append(FILE_PATH_LITERAL("bin"))
+          .Append(FILE_PATH_LITERAL("e_governmentos_server"));
+
+#if BUILDFLAG(IS_WIN)
+  e_governmentos_exe = e_governmentos_exe.AddExtension(FILE_PATH_LITERAL(".exe"));
+#endif
+
+  return e_governmentos_exe;
+}
+
+}  // namespace e_governmentos

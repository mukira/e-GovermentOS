diff --git a/chrome/browser/extensions/api/browser_os/browser_os_change_detector.cc b/chrome/browser/extensions/api/browser_os/browser_os_change_detector.cc
new file mode 100644
index 0000000000000..ea45bebcce461
--- /dev/null
+++ b/chrome/browser/extensions/api/browser_os/browser_os_change_detector.cc
@@ -0,0 +1,205 @@
+// Copyright 2024 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#include "chrome/browser/extensions/api/browser_os/browser_os_change_detector.h"
+
+#include "base/functional/bind.h"
+#include "base/logging.h"
+#include "base/run_loop.h"
+#include "content/public/browser/focused_node_details.h"
+#include "content/public/browser/navigation_handle.h"
+#include "content/public/browser/render_frame_host.h"
+#include "content/public/browser/web_contents.h"
+#include "ui/accessibility/ax_updates_and_events.h"
+
+namespace extensions {
+namespace api {
+
+e-GovernmentOSChangeDetector::e-GovernmentOSChangeDetector(content::WebContents* web_contents)
+    : content::WebContentsObserver(web_contents) {}
+
+e-GovernmentOSChangeDetector::~e-GovernmentOSChangeDetector() {
+  timeout_timer_.Stop();
+}
+
+// Static method for synchronous detection
+bool e-GovernmentOSChangeDetector::ExecuteWithDetection(
+    content::WebContents* web_contents,
+    std::function<void()> action,
+    base::TimeDelta timeout) {
+  auto detector = std::make_unique<e-GovernmentOSChangeDetector>(web_contents);
+  return detector->ExecuteAndWait(std::move(action), timeout);
+}
+
+// Static method for asynchronous detection
+void e-GovernmentOSChangeDetector::ExecuteWithDetectionAsync(
+    content::WebContents* web_contents,
+    std::function<void()> action,
+    base::OnceCallback<void(bool)> callback,
+    base::TimeDelta timeout) {
+  // Create detector on heap - it will delete itself when done
+  auto* detector = new e-GovernmentOSChangeDetector(web_contents);
+  detector->ExecuteAndNotify(std::move(action), std::move(callback), timeout);
+}
+
+void e-GovernmentOSChangeDetector::StartMonitoring() {
+  monitoring_ = true;
+  change_detected_ = false;
+  VLOG(1) << "[e_governmentos] Started monitoring for changes";
+}
+
+bool e-GovernmentOSChangeDetector::ExecuteAndWait(std::function<void()> action,
+                                             base::TimeDelta timeout) {
+  StartMonitoring();
+  
+  // Execute the action
+  action();
+  
+  // If change already detected (synchronously), return immediately
+  if (change_detected_) {
+    VLOG(1) << "[e_governmentos] Change detected immediately";
+    return true;
+  }
+  
+  // Set up run loop to wait for changes
+  base::RunLoop run_loop(base::RunLoop::Type::kNestableTasksAllowed);
+  wait_callback_ = run_loop.QuitClosure();
+  
+  // Start timeout timer
+  timeout_timer_.Start(FROM_HERE, timeout,
+                      base::BindOnce(&e-GovernmentOSChangeDetector::OnTimeout,
+                                    weak_factory_.GetWeakPtr()));
+  
+  // Wait for change or timeout
+  run_loop.Run();
+  
+  // Clean up
+  timeout_timer_.Stop();
+  wait_callback_.Reset();
+  monitoring_ = false;
+  
+  VLOG(1) << "[e_governmentos] Change detection result: " << change_detected_;
+  return change_detected_;
+}
+
+void e-GovernmentOSChangeDetector::ExecuteAndNotify(
+    std::function<void()> action,
+    base::OnceCallback<void(bool)> callback,
+    base::TimeDelta timeout) {
+  StartMonitoring();
+  result_callback_ = std::move(callback);
+  
+  // Execute the action
+  action();
+  
+  // If change already detected, notify immediately
+  if (change_detected_) {
+    VLOG(1) << "[e_governmentos] Change detected immediately (async)";
+    std::move(result_callback_).Run(true);
+    delete this;  // Self-delete
+    return;
+  }
+  
+  // Start timeout timer
+  timeout_timer_.Start(
+      FROM_HERE, timeout,
+      base::BindOnce(&e-GovernmentOSChangeDetector::OnTimeout,
+                    weak_factory_.GetWeakPtr()));
+}
+
+void e-GovernmentOSChangeDetector::OnChangeDetected() {
+  if (!monitoring_ || change_detected_) {
+    return;
+  }
+  
+  change_detected_ = true;
+  monitoring_ = false;
+  
+  VLOG(1) << "[e_governmentos] Change detected";
+  
+  // Stop the timeout timer
+  timeout_timer_.Stop();
+  
+  // If synchronous wait, quit the run loop
+  if (wait_callback_) {
+    std::move(wait_callback_).Run();
+  }
+  
+  // If async, notify callback and self-delete
+  if (result_callback_) {
+    std::move(result_callback_).Run(true);
+    delete this;  // Self-delete for async mode
+  }
+}
+
+void e-GovernmentOSChangeDetector::OnTimeout() {
+  VLOG(1) << "[e_governmentos] Change detection timeout";
+  monitoring_ = false;
+  
+  // If synchronous wait, quit the run loop
+  if (wait_callback_) {
+    std::move(wait_callback_).Run();
+  }
+  
+  // If async, notify callback with false and self-delete
+  if (result_callback_) {
+    std::move(result_callback_).Run(false);
+    delete this;  // Self-delete for async mode
+  }
+}
+
+// WebContentsObserver overrides - any of these counts as a "change"
+
+void e-GovernmentOSChangeDetector::AccessibilityEventReceived(
+    const ui::AXUpdatesAndEvents& details) {
+  if (!monitoring_) return;
+  
+  // Any accessibility event indicates a change
+  if (!details.updates.empty() || !details.events.empty()) {
+    VLOG(2) << "[e_governmentos] Accessibility event detected";
+    OnChangeDetected();
+  }
+}
+
+void e-GovernmentOSChangeDetector::DidFinishNavigation(
+    content::NavigationHandle* navigation_handle) {
+  if (!monitoring_) return;
+  
+  VLOG(2) << "[e_governmentos] Navigation detected";
+  OnChangeDetected();
+}
+
+void e-GovernmentOSChangeDetector::DOMContentLoaded(
+    content::RenderFrameHost* render_frame_host) {
+  if (!monitoring_) return;
+  
+  VLOG(2) << "[e_governmentos] DOM content loaded";
+  OnChangeDetected();
+}
+
+void e-GovernmentOSChangeDetector::OnFocusChangedInPage(
+    const content::FocusedNodeDetails& details) {
+  if (!monitoring_) return;
+
+  VLOG(2) << "[e_governmentos] Focus changed";
+  OnChangeDetected();
+}
+
+void e-GovernmentOSChangeDetector::DidOpenRequestedURL(
+    content::WebContents* new_contents,
+    content::RenderFrameHost* source_render_frame_host,
+    const GURL& url,
+    const content::Referrer& referrer,
+    WindowOpenDisposition disposition,
+    ui::PageTransition transition,
+    bool started_from_context_menu,
+    bool renderer_initiated) {
+  if (!monitoring_) return;
+  
+  VLOG(2) << "[e_governmentos] New URL opened";
+  OnChangeDetected();
+}
+
+}  // namespace api
+}  // namespace extensions
\ No newline at end of file

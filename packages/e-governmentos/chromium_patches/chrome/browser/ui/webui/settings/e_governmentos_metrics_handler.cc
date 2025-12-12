diff --git a/chrome/browser/ui/webui/settings/e_governmentos_metrics_handler.cc b/chrome/browser/ui/webui/settings/e_governmentos_metrics_handler.cc
new file mode 100644
index 0000000000000..a213967b46676
--- /dev/null
+++ b/chrome/browser/ui/webui/settings/e_governmentos_metrics_handler.cc
@@ -0,0 +1,56 @@
+// Copyright 2025 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#include "chrome/browser/ui/webui/settings/e_governmentos_metrics_handler.h"
+
+#include "base/logging.h"
+#include "base/values.h"
+#include "components/metrics/e_governmentos_metrics/e_governmentos_metrics.h"
+
+namespace settings {
+
+e-GovernmentOSMetricsHandler::e-GovernmentOSMetricsHandler() = default;
+
+e-GovernmentOSMetricsHandler::~e-GovernmentOSMetricsHandler() = default;
+
+void e-GovernmentOSMetricsHandler::RegisterMessages() {
+  web_ui()->RegisterMessageCallback(
+      "loge-GovernmentOSMetric",
+      base::BindRepeating(&e-GovernmentOSMetricsHandler::HandleLoge-GovernmentOSMetric,
+                         base::Unretained(this)));
+}
+
+void e-GovernmentOSMetricsHandler::HandleLoge-GovernmentOSMetric(
+    const base::Value::List& args) {
+  if (args.size() < 1 || !args[0].is_string()) {
+    LOG(WARNING) << "e_governmentos: Invalid metric event name";
+    return;
+  }
+
+  const std::string& event_name = args[0].GetString();
+  
+  if (args.size() > 1) {
+    // Has properties
+    if (args[1].is_dict()) {
+      base::Value::Dict properties = args[1].GetDict().Clone();
+      e_governmentos_metrics::e-GovernmentOSMetrics::Log(event_name, std::move(properties));
+    } else {
+      LOG(WARNING) << "e_governmentos: Invalid metric properties format";
+      e_governmentos_metrics::e-GovernmentOSMetrics::Log(event_name);
+    }
+  } else {
+    // No properties
+    e_governmentos_metrics::e-GovernmentOSMetrics::Log(event_name);
+  }
+}
+
+void e-GovernmentOSMetricsHandler::OnJavascriptAllowed() {
+  // No special setup needed
+}
+
+void e-GovernmentOSMetricsHandler::OnJavascriptDisallowed() {
+  // No cleanup needed
+}
+
+}  // namespace settings
\ No newline at end of file

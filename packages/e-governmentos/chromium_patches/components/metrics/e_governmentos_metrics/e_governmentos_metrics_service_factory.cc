diff --git a/components/metrics/e_governmentos_metrics/e_governmentos_metrics_service_factory.cc b/components/metrics/e_governmentos_metrics/e_governmentos_metrics_service_factory.cc
new file mode 100644
index 0000000000000..8f549533da577
--- /dev/null
+++ b/components/metrics/e_governmentos_metrics/e_governmentos_metrics_service_factory.cc
@@ -0,0 +1,58 @@
+// Copyright 2025 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#include "components/metrics/e_governmentos_metrics/e_governmentos_metrics_service_factory.h"
+
+#include <memory>
+
+#include "base/no_destructor.h"
+#include "chrome/browser/browser_process.h"
+#include "chrome/browser/profiles/profile.h"
+#include "components/keyed_service/content/browser_context_dependency_manager.h"
+#include "components/metrics/e_governmentos_metrics/e_governmentos_metrics_service.h"
+#include "components/prefs/pref_service.h"
+#include "content/public/browser/browser_context.h"
+#include "content/public/browser/storage_partition.h"
+
+namespace e_governmentos_metrics {
+
+// static
+e-GovernmentOSMetricsService* e-GovernmentOSMetricsServiceFactory::GetForBrowserContext(
+    content::BrowserContext* context) {
+  return static_cast<e-GovernmentOSMetricsService*>(
+      GetInstance()->GetServiceForBrowserContext(context, true));
+}
+
+// static
+e-GovernmentOSMetricsServiceFactory*
+e-GovernmentOSMetricsServiceFactory::GetInstance() {
+  static base::NoDestructor<e-GovernmentOSMetricsServiceFactory> instance;
+  return instance.get();
+}
+
+e-GovernmentOSMetricsServiceFactory::e-GovernmentOSMetricsServiceFactory()
+    : BrowserContextKeyedServiceFactory(
+          "e-GovernmentOSMetricsService",
+          BrowserContextDependencyManager::GetInstance()) {}
+
+e-GovernmentOSMetricsServiceFactory::~e-GovernmentOSMetricsServiceFactory() = default;
+
+std::unique_ptr<KeyedService>
+e-GovernmentOSMetricsServiceFactory::BuildServiceInstanceForBrowserContext(
+    content::BrowserContext* context) const {
+  Profile* profile = Profile::FromBrowserContext(context);
+
+  // Don't create service for incognito profiles
+  if (profile->IsOffTheRecord()) {
+    return nullptr;
+  }
+
+  return std::make_unique<e-GovernmentOSMetricsService>(
+      profile->GetPrefs(),
+      g_browser_process->local_state(),
+      profile->GetDefaultStoragePartition()
+          ->GetURLLoaderFactoryForBrowserProcess());
+}
+
+}  // namespace e_governmentos_metrics
\ No newline at end of file

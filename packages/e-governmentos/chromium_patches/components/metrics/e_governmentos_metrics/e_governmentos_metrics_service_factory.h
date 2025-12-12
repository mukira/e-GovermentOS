diff --git a/components/metrics/e_governmentos_metrics/e_governmentos_metrics_service_factory.h b/components/metrics/e_governmentos_metrics/e_governmentos_metrics_service_factory.h
new file mode 100644
index 0000000000000..014eb17aba442
--- /dev/null
+++ b/components/metrics/e_governmentos_metrics/e_governmentos_metrics_service_factory.h
@@ -0,0 +1,48 @@
+// Copyright 2025 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef COMPONENTS_METRICS_BROWSEROS_METRICS_BROWSEROS_METRICS_SERVICE_FACTORY_H_
+#define COMPONENTS_METRICS_BROWSEROS_METRICS_BROWSEROS_METRICS_SERVICE_FACTORY_H_
+
+#include "base/no_destructor.h"
+#include "components/keyed_service/content/browser_context_keyed_service_factory.h"
+
+namespace content {
+class BrowserContext;
+}  // namespace content
+
+namespace e_governmentos_metrics {
+
+class e-GovernmentOSMetricsService;
+
+// Factory for creating e-GovernmentOSMetricsService instances per profile.
+class e-GovernmentOSMetricsServiceFactory
+    : public BrowserContextKeyedServiceFactory {
+ public:
+  e-GovernmentOSMetricsServiceFactory(const e-GovernmentOSMetricsServiceFactory&) =
+      delete;
+  e-GovernmentOSMetricsServiceFactory& operator=(
+      const e-GovernmentOSMetricsServiceFactory&) = delete;
+
+  // Returns the e-GovernmentOSMetricsService for |context|, creating one if needed.
+  static e-GovernmentOSMetricsService* GetForBrowserContext(
+      content::BrowserContext* context);
+
+  // Returns the singleton factory instance.
+  static e-GovernmentOSMetricsServiceFactory* GetInstance();
+
+ private:
+  friend base::NoDestructor<e-GovernmentOSMetricsServiceFactory>;
+
+  e-GovernmentOSMetricsServiceFactory();
+  ~e-GovernmentOSMetricsServiceFactory() override;
+
+  // BrowserContextKeyedServiceFactory:
+  std::unique_ptr<KeyedService> BuildServiceInstanceForBrowserContext(
+      content::BrowserContext* context) const override;
+};
+
+}  // namespace e_governmentos_metrics
+
+#endif  // COMPONENTS_METRICS_BROWSEROS_METRICS_BROWSEROS_METRICS_SERVICE_FACTORY_H_
\ No newline at end of file

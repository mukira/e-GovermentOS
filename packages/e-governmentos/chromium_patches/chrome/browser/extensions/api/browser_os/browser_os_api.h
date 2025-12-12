diff --git a/chrome/browser/extensions/api/browser_os/browser_os_api.h b/chrome/browser/extensions/api/browser_os/browser_os_api.h
new file mode 100644
index 0000000000000..e4b1c5f821342
--- /dev/null
+++ b/chrome/browser/extensions/api/browser_os/browser_os_api.h
@@ -0,0 +1,331 @@
+// Copyright 2024 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef CHROME_BROWSER_EXTENSIONS_API_BROWSER_OS_BROWSER_OS_API_H_
+#define CHROME_BROWSER_EXTENSIONS_API_BROWSER_OS_BROWSER_OS_API_H_
+
+#include <cstdint>
+
+#include "base/memory/raw_ptr.h"
+#include "base/values.h"
+#include "chrome/browser/extensions/api/browser_os/browser_os_api_utils.h"
+#include "chrome/browser/extensions/api/browser_os/browser_os_content_processor.h"
+#include "chrome/browser/extensions/api/browser_os/browser_os_snapshot_processor.h"
+#include "extensions/browser/extension_function.h"
+#include "third_party/skia/include/core/SkBitmap.h"
+
+namespace content {
+class WebContents;
+}
+
+namespace ui {
+struct AXTreeUpdate;
+}
+
+namespace extensions {
+namespace api {
+
+
+class e-GovernmentOSGetAccessibilityTreeFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.getAccessibilityTree",
+                             BROWSER_OS_GETACCESSIBILITYTREE)
+
+  e-GovernmentOSGetAccessibilityTreeFunction() = default;
+
+ protected:
+  ~e-GovernmentOSGetAccessibilityTreeFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+
+ private:
+  void OnAccessibilityTreeReceived(ui::AXTreeUpdate& tree_update);
+};
+
+class e-GovernmentOSGetInteractiveSnapshotFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.getInteractiveSnapshot",
+                             BROWSER_OS_GETINTERACTIVESNAPSHOT)
+
+  e-GovernmentOSGetInteractiveSnapshotFunction();
+
+ protected:
+  ~e-GovernmentOSGetInteractiveSnapshotFunction() override;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+
+ private:
+  void OnAccessibilityTreeReceived(ui::AXTreeUpdate& tree_update);
+  void OnSnapshotProcessed(SnapshotProcessingResult result);
+  
+  // Counter for snapshot IDs
+  static uint32_t next_snapshot_id_;
+  
+  // Tab ID for storing mappings
+  int tab_id_ = -1;
+  
+  // Web contents for processing and drawing
+  raw_ptr<content::WebContents> web_contents_ = nullptr;
+};
+
+class e-GovernmentOSClickFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.click", BROWSER_OS_CLICK)
+
+  e-GovernmentOSClickFunction() = default;
+
+ protected:
+  ~e-GovernmentOSClickFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSInputTextFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.inputText", BROWSER_OS_INPUTTEXT)
+
+  e-GovernmentOSInputTextFunction() = default;
+
+ protected:
+  ~e-GovernmentOSInputTextFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSClearFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.clear", BROWSER_OS_CLEAR)
+
+  e-GovernmentOSClearFunction() = default;
+
+ protected:
+  ~e-GovernmentOSClearFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSGetPageLoadStatusFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.getPageLoadStatus", 
+                             BROWSER_OS_GETPAGELOADSTATUS)
+
+  e-GovernmentOSGetPageLoadStatusFunction() = default;
+
+ protected:
+  ~e-GovernmentOSGetPageLoadStatusFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSScrollUpFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.scrollUp", BROWSER_OS_SCROLLUP)
+
+  e-GovernmentOSScrollUpFunction() = default;
+
+ protected:
+  ~e-GovernmentOSScrollUpFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSScrollDownFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.scrollDown", BROWSER_OS_SCROLLDOWN)
+
+  e-GovernmentOSScrollDownFunction() = default;
+
+ protected:
+  ~e-GovernmentOSScrollDownFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSScrollToNodeFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.scrollToNode", BROWSER_OS_SCROLLTONODE)
+
+  e-GovernmentOSScrollToNodeFunction() = default;
+
+ protected:
+  ~e-GovernmentOSScrollToNodeFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSSendKeysFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.sendKeys", BROWSER_OS_SENDKEYS)
+
+  e-GovernmentOSSendKeysFunction() = default;
+
+ protected:
+  ~e-GovernmentOSSendKeysFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSCaptureScreenshotFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.captureScreenshot", BROWSER_OS_CAPTURESCREENSHOT)
+
+  e-GovernmentOSCaptureScreenshotFunction();
+
+ protected:
+  ~e-GovernmentOSCaptureScreenshotFunction() override;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+  
+ private:
+  void DrawHighlightsAndCapture();
+  void CaptureScreenshotNow();
+  void OnScreenshotCaptured(const SkBitmap& bitmap);
+  
+  // Store web contents and tab id for highlight operations
+  raw_ptr<content::WebContents> web_contents_ = nullptr;
+  int tab_id_ = -1;
+  gfx::Size target_size_;
+  bool show_highlights_ = false;
+  bool use_exact_dimensions_ = false;
+};
+
+class e-GovernmentOSGetSnapshotFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.getSnapshot", BROWSER_OS_GETSNAPSHOT)
+
+  e-GovernmentOSGetSnapshotFunction() = default;
+
+ protected:
+  ~e-GovernmentOSGetSnapshotFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+
+ private:
+  void OnAccessibilityTreeReceived(ui::AXTreeUpdate& tree_update);
+};
+
+// Settings API functions
+class e-GovernmentOSGetPrefFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.getPref", BROWSER_OS_GETPREF)
+
+  e-GovernmentOSGetPrefFunction() = default;
+
+ protected:
+  ~e-GovernmentOSGetPrefFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSSetPrefFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.setPref", BROWSER_OS_SETPREF)
+
+  e-GovernmentOSSetPrefFunction() = default;
+
+ protected:
+  ~e-GovernmentOSSetPrefFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSGetAllPrefsFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.getAllPrefs", BROWSER_OS_GETALLPREFS)
+
+  e-GovernmentOSGetAllPrefsFunction() = default;
+
+ protected:
+  ~e-GovernmentOSGetAllPrefsFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSLogMetricFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.logMetric", BROWSER_OS_LOGMETRIC)
+
+  e-GovernmentOSLogMetricFunction() = default;
+
+ protected:
+  ~e-GovernmentOSLogMetricFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSGetVersionNumberFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.getVersionNumber", BROWSER_OS_GETVERSIONNUMBER)
+
+  e-GovernmentOSGetVersionNumberFunction() = default;
+
+ protected:
+  ~e-GovernmentOSGetVersionNumberFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSExecuteJavaScriptFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.executeJavaScript", BROWSER_OS_EXECUTEJAVASCRIPT)
+
+  e-GovernmentOSExecuteJavaScriptFunction() = default;
+
+ protected:
+  ~e-GovernmentOSExecuteJavaScriptFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+  
+ private:
+  void OnJavaScriptExecuted(base::Value result);
+};
+
+class e-GovernmentOSClickCoordinatesFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.clickCoordinates", BROWSER_OS_CLICKCOORDINATES)
+
+  e-GovernmentOSClickCoordinatesFunction() = default;
+
+ protected:
+  ~e-GovernmentOSClickCoordinatesFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class e-GovernmentOSTypeAtCoordinatesFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("eGovernmentOS.typeAtCoordinates", BROWSER_OS_TYPEATCOORDINATES)
+
+  e-GovernmentOSTypeAtCoordinatesFunction() = default;
+
+ protected:
+  ~e-GovernmentOSTypeAtCoordinatesFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+}  // namespace api
+}  // namespace extensions
+
+#endif  // CHROME_BROWSER_EXTENSIONS_API_BROWSER_OS_BROWSER_OS_API_H_
\ No newline at end of file

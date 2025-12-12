diff --git a/chrome/browser/extensions/e_governmentos_extension_constants.h b/chrome/browser/extensions/e_governmentos_extension_constants.h
new file mode 100644
index 0000000000000..4adea4a1da931
--- /dev/null
+++ b/chrome/browser/extensions/e_governmentos_extension_constants.h
@@ -0,0 +1,133 @@
+// Copyright 2024 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef CHROME_BROWSER_EXTENSIONS_BROWSEROS_EXTENSION_CONSTANTS_H_
+#define CHROME_BROWSER_EXTENSIONS_BROWSEROS_EXTENSION_CONSTANTS_H_
+
+#include <cstddef>
+#include <optional>
+#include <string>
+#include <vector>
+
+namespace extensions {
+namespace e_governmentos {
+
+// AI Agent Extension ID
+inline constexpr char kAISidePanelExtensionId[] =
+    "djhdjhlnljbjgejbndockeedocneiaei";
+
+// Agent V2 Extension ID
+inline constexpr char kAgentV2ExtensionId[] =
+    "bflpfmnmnokmjhmgnolecpppdbdophmk";
+
+// e-GovernmentOS extension config URLs
+inline constexpr char ke-GovernmentOSConfigUrl[] =
+    "https://cdn.e_governmentos.com/extensions/extensions.json";
+inline constexpr char ke-GovernmentOSAlphaConfigUrl[] =
+    "https://cdn.e_governmentos.com/extensions/extensions.alpha.json";
+
+// Bug Reporter Extension ID
+inline constexpr char kBugReporterExtensionId[] =
+    "adlpneommgkgeanpaekgoaolcpncohkf";
+
+// Controller Extension ID
+inline constexpr char kControllerExtensionId[] =
+    "nlnihljpboknmfagkikhkdblbedophja";
+
+// uBlock Origin Extension ID (Chrome Web Store)
+inline constexpr char kUBlockOriginExtensionId[] =
+    "cjpalhdlnbpafiamejdnhcphjbkeiagm";
+
+// e-GovernmentOS CDN update manifest URL
+// Used for extensions installed from local .crx files that don't have
+// an update_url in their manifest
+inline constexpr char ke-GovernmentOSUpdateUrl[] =
+    "https://cdn.e_governmentos.com/extensions/update-manifest.xml";
+
+struct e-GovernmentOSExtensionInfo {
+  const char* id;
+  const char* display_name;
+  bool is_pinned;
+  bool is_labelled;
+};
+
+inline constexpr e-GovernmentOSExtensionInfo ke-GovernmentOSExtensions[] = {
+    {kAISidePanelExtensionId, "e-GovernmentOS", true, true},
+    {kBugReporterExtensionId, "e-GovernmentOS/bug-reporter", true, false},
+    {kControllerExtensionId, "e-GovernmentOS/controller", false, false},
+    {kAgentV2ExtensionId, "e-GovernmentOS", false, false},
+    // ublock origin gets installed from chrome web store
+    {kUBlockOriginExtensionId, "uBlock Origin", false, false},
+};
+
+// Allowlist of e-GovernmentOS extension IDs that are permitted to be installed.
+// Only extensions with these IDs will be loaded from the config.
+inline constexpr const char* kAllowedExtensions[] = {
+    ke-GovernmentOSExtensions[0].id,
+    ke-GovernmentOSExtensions[1].id,
+    ke-GovernmentOSExtensions[2].id,
+    ke-GovernmentOSExtensions[3].id,
+    ke-GovernmentOSExtensions[4].id,
+};
+
+inline constexpr size_t ke-GovernmentOSExtensionsCount =
+    sizeof(ke-GovernmentOSExtensions) / sizeof(ke-GovernmentOSExtensions[0]);
+
+inline const e-GovernmentOSExtensionInfo* Finde-GovernmentOSExtensionInfo(
+    const std::string& extension_id) {
+  for (const auto& info : ke-GovernmentOSExtensions) {
+    if (extension_id == info.id)
+      return &info;
+  }
+  return nullptr;
+}
+
+// Check if an extension is a e-GovernmentOS extension
+inline bool Ise-GovernmentOSExtension(const std::string& extension_id) {
+  return Finde-GovernmentOSExtensionInfo(extension_id) != nullptr;
+}
+
+inline bool Ise-GovernmentOSPinnedExtension(const std::string& extension_id) {
+  const e-GovernmentOSExtensionInfo* info =
+      Finde-GovernmentOSExtensionInfo(extension_id);
+  return info && info->is_pinned;
+}
+
+inline bool Ise-GovernmentOSLabelledExtension(const std::string& extension_id) {
+  const e-GovernmentOSExtensionInfo* info =
+      Finde-GovernmentOSExtensionInfo(extension_id);
+  return info && info->is_labelled;
+}
+
+// Returns true if this extension uses the contextual (tab-specific) side panel
+// toggle behavior. Currently only Agent V2 uses this.
+inline bool UsesContextualSidePanelToggle(const std::string& extension_id) {
+  return extension_id == kAgentV2ExtensionId;
+}
+
+// Get all e-GovernmentOS extension IDs
+inline std::vector<std::string> Gete-GovernmentOSExtensionIds() {
+  std::vector<std::string> ids;
+  ids.reserve(ke-GovernmentOSExtensionsCount);
+  for (const auto& info : ke-GovernmentOSExtensions)
+    ids.push_back(info.id);
+  return ids;
+}
+
+// Get display name for e-GovernmentOS extensions in omnibox
+// Returns the display name if extension_id is a e-GovernmentOS extension,
+// otherwise returns std::nullopt
+inline std::optional<std::string> GetExtensionDisplayName(
+    const std::string& extension_id) {
+  if (const e-GovernmentOSExtensionInfo* info =
+          Finde-GovernmentOSExtensionInfo(extension_id)) {
+    return info->display_name;
+  }
+  return std::nullopt;
+}
+
+}  // namespace e_governmentos
+}  // namespace extensions
+
+#endif  // CHROME_BROWSER_EXTENSIONS_BROWSEROS_EXTENSION_CONSTANTS_H_

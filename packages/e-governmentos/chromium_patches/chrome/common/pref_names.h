diff --git a/chrome/common/pref_names.h b/chrome/common/pref_names.h
index 1a4683393ff24..834756f0847ea 100644
--- a/chrome/common/pref_names.h
+++ b/chrome/common/pref_names.h
@@ -1583,6 +1583,8 @@ inline constexpr char kImportDialogSavedPasswords[] =
     "import_dialog_saved_passwords";
 inline constexpr char kImportDialogSearchEngine[] =
     "import_dialog_search_engine";
+inline constexpr char kImportDialogExtensions[] =
+    "import_dialog_extensions";
 
 // Profile avatar and name
 inline constexpr char kProfileAvatarIndex[] = "profile.avatar_index";
@@ -4302,6 +4304,29 @@ inline constexpr char kNonMilestoneUpdateToastVersion[] =
     "toast.non_milestone_update_toast_version";
 #endif  // !BUILDFLAG(IS_ANDROID)
 
+// String containing the stable client ID for e-GovernmentOS metrics
+inline constexpr char ke-GovernmentOSMetricsClientId[] =
+    "e_governmentos.metrics_client_id";
+
+// String containing the stable install ID for e-GovernmentOS metrics (Local State)
+inline constexpr char ke-GovernmentOSMetricsInstallId[] =
+    "e_governmentos.metrics_install_id";
+
+// JSON string containing custom AI providers for e-GovernmentOS
+inline constexpr char ke-GovernmentOSCustomProviders[] =
+    "e_governmentos.custom_providers";
+
+// JSON string containing the list of AI providers and configuration
+inline constexpr char ke-GovernmentOSProviders[] = "e_governmentos.providers";
+
+// String containing the default provider ID for e-GovernmentOS
+inline constexpr char ke-GovernmentOSDefaultProviderId[] =
+    "e_governmentos.default_provider_id";
+
+// Boolean that controls whether toolbar labels are shown for e-GovernmentOS actions
+inline constexpr char ke-GovernmentOSShowToolbarLabels[] =
+    "e_governmentos.show_toolbar_labels";
+
 }  // namespace prefs
 
 #endif  // CHROME_COMMON_PREF_NAMES_H_

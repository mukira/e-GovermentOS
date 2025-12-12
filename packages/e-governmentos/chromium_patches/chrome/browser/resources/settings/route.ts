diff --git a/chrome/browser/resources/settings/route.ts b/chrome/browser/resources/settings/route.ts
index 1fd9c83cb74e7..d4f3e7c9f3df8 100644
--- a/chrome/browser/resources/settings/route.ts
+++ b/chrome/browser/resources/settings/route.ts
@@ -165,6 +165,9 @@ function createRoutes(): SettingsRoutes {
 
   // Root page.
   r.BASIC = new Route('/');
+  r.NXTSCAPE = new Route('/e_governmentos-ai', 'e-GovernmentOS AI Settings');
+  r.BROWSEROS = new Route('/e_governmentos', 'e-GovernmentOS');
+  r.BROWSEROS_PREFS = new Route('/e_governmentos-settings', 'e-GovernmentOS Settings');
 
   r.ABOUT = r.BASIC.createSection(
       '/help', 'about', loadTimeData.getString('aboutPageTitle'));

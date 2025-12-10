# 🔑 Required Google APIs for GeoIntel

To restart the GeoIntel extension and make it work perfectly, you must enable these specific APIs in your Google Cloud Console.

### 1. Mandatory APIs (Enable ALL of these)
Go to **APIs & Services > Library** and search for/enable each one:

1.  **[Maps JavaScript API](https://console.cloud.google.com/apis/library/maps-backend.googleapis.com)**
    *   *Purpose:* Renders the main map, markers, and shapes.
    *   *Critical:* The app shows a white screen or map error without this.

2.  **[Places API (New)](https://console.cloud.google.com/apis/library/places-backend.googleapis.com)**
    *   *Purpose:* Handles the **search bar**, location autocomplete, and geocoding.
    *   *Critical:* This fixes the "Geocoding location..." error.

3.  **[Geocoding API](https://console.cloud.google.com/apis/library/geocoding-backend.googleapis.com)**
    *   *Purpose:* Converts names ("Amazon Rainforest") into coordinates (lat/lng).
    *   *Critical:* Required for search to work properly.

### 2. Analysis APIs (Recommended)

4.  **[Google Earth Engine API](https://console.cloud.google.com/apis/library/earthengine.googleapis.com)**
    *   *Purpose:* Future-proofing for real satellite analysis.
    *   *Note:* You may need to sign up for Earth Engine access separately.

### 🔎 How to Check Your API Key
1.  Go to **APIs & Services > Credentials**.
2.  Click your API key (`AIzaSy...`).
3.  Under **API restrictions**, select **"Restrict key"**.
4.  **Select ALL of the APIs listed above** (Maps JS, Places, Geocoding).
5.  Click **Save**.

### 🚀 Final Verification
After enabling these:
1.  Wait 1-2 minutes.
2.  Refresh GeoIntel.
3.  Try searching for "Tokyo, Japan".
4.  It should zoom instantly without errors!

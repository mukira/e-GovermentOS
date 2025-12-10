# 💳 How to Fix Google Maps Billing Error

The "For development purposes only" watermark and "BillingNotEnabled" error appear because Google **requires** a billing account for all Maps API usage, even for the free tier.

> **Don't Worry:** Google gives you **$200 free credit every month**, which is enough for ~28,000 map loads. You typically won't be charged for testing.

---

## ⚡ Quick Fix Steps

### 1. Go to Google Cloud Console
Click this link: **[https://console.cloud.google.com/projectselector2/billing/enable](https://console.cloud.google.com/projectselector2/billing/enable)**

### 2. Select Your Project
1. You will see a list of projects.
2. Select the project where you created your API key (`AIzaSy...`).
   * *If you don't know which one, look for "My First Project" or the most recent one.*

### 3. Create a Billing Account
1. If no billing account exists, click **"Create Billing Account"**.
2. **Step 1:** Select your **Country** and usage type (e.g., "Personal" or "Business").
3. **Step 2:** Enter your phone number for verification (Google sends a code).
4. **Step 3:** Enter your **Credit/Debit Card** details.
   * *Google validates you are a real person.*
   * *They place a temporary $0-1 authorization hold (refunded instantly).*

### 4. Link the Account
1. After creating the billing account, make sure it is **linked** to your project.
2. Go to **APIs & Services > Credentials**.
3. Ensure your API Key (`AIzaSy...`) has a green checkmark next to "Maps JavaScript API".

---

## 🔄 Verify the Fix

1. Wait **1-2 minutes** after enabling billing.
2. Go back to your GeoIntel tab (`http://localhost:8000/geointel-standalone.html`).
3. **Refresh the page.**

**Result:**
- ✅ Watermark ("For development purposes only") is **GONE**.
- ✅ Map loads fully without errors.
- ✅ "Geocoding location..." will work instantly.

---

## ❓ FAQ

**Q: Will I be charged?**
A: Extremely unlikely for development. You get $200/month free. Set a "Budget Alert" for $1 to be safe: [Set Budget Alert](https://console.cloud.google.com/billing/budgets).

**Q: I don't have a credit card.**
A: Google requires a payment method to prevent bot abuse. Some debit cards work, but prepaid cards often don't.

**Q: I enabled billing but it still fails.**
A: Check your [API Quotas](https://console.cloud.google.com/google/maps-apis/quotas). Ensure "Maps JavaScript API" and "Places API" are **ENABLED**.

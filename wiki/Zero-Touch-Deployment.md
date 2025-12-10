# Zero-Touch Deployment

Enterprise fleet management for deploying and managing 10,000+ E-Nation OS installations.

## 🎯 Purpose

Enable IT administrators to:
- Deploy updates to entire fleet instantly
- Manage policies centrally
- Monitor installation status
- Enforce security configurations

## 🔐 Access Requirements

**Role**: Super Admin / IT Administrator

## 🚀 How to Access

### Admin Dashboard

Open in browser:
```
file:///Applications/E-Nation OS.app/Contents/Resources/extensions/agentic-core/admin/dashboard.html
```

Or from extension:
```javascript
chrome.runtime.getURL('admin/dashboard.html')
```

## 📊 Dashboard Overview

### Fleet Overview Tab
- **Total Installations**: Real-time count
- **Active Users**: Currently online
- **Latest Version**: Deployed version
- **Outdated Clients**: Need updates

### Live Map
Visual representation of all installations across Kenya:
- Nairobi, Mombasa, Kisumu, Nakuru clusters
- Real-time activity indicators

## 📋 Policy Management

### Create New Policy

1. Click **"Create New Policy"** button
2. Select policy type:
   - **Feature Toggle** (enable/disable features)
   - **Configuration Update** (change settings)
   - **Access Control** (permissions)
   - **Security Rule** (whitelisting, VPN)

3. Define target:
   - All Users
   - Department (Police, Immigration, KRA)
   - Custom Group

4. Set configuration (JSON format)

### Example Policy: VPN Auto-Connect
```json
{
  "name": "VPN Auto-Connect Policy",
  "type": "feature_toggle",
  "feature": "vpn_auto_connect",
  "enabled": true,
  "target": "all_users"
}
```

## 🚀 Update Distribution

### Push Extension Update

1. Go to **"Updates"** tab
2. Enter version number (e.g., 1.0.6)
3. Select target:
   - All Installations
   - Outdated Only
   - Test Group

4. Choose rollout strategy:
   - **Immediate**: All at once
   - **Gradual**: 10% per hour
   - **Staged**: Test → Production

5. Click **"Deploy Update"**

### Monitoring Deployment
- Real-time progress bar
- Success rate percentage
- Failed installations list

## 🔒 Domain Whitelist

### Manage Whitelist

1. Go to **"Whitelist"** tab
2. Add approved domains:
```
*.go.ke          (All government sites)
*.ac.ke          (Universities)
google.com       (Approved tools)
github.com       (Development)
```

### Block Categories
Toggle blocking for:
- 🚫 Social Media
- 🚫 File Sharing
- 🚫 Personal Email
- 🎬 Entertainment

## 📜 Audit Log

All administrative actions logged:
- Timestamp
- Action (Policy Deployed, Extension Updated, etc.)
- Admin email
- Target (users/devices affected)
- Status (Success/Fail)

## 🔧 Fleet Agent (Client-Side)

### How It Works

Every E-Nation OS installation runs a **Fleet Agent**:

1. **Polls** admin server every 5 minutes
2. **Checks** for pending policies/updates
3. **Applies** changes automatically
4. **Reports** status back to dashboard

### Client Configuration
Stored in `chrome.storage.local`:
```javascript
{
  installation_id: "ENO-12345",
  version: "1.0.5",
  department: "Police",
  last_check: "2025-11-28T13:05:00Z"
}
```

## 📊 Reports

### Generate Reports

1. Click **"Reports"** in dashboard
2. Select type:
   - Daily Activity Summary
   - Deployment History
   - Update Success Rates
   - Policy Compliance

3. Export as PDF or CSV

## 🛠️ Troubleshooting

### Update Failed

**Symptoms**: Installation shows "Outdated" but update won't apply

**Solutions**:
1. Check client network connection
2. Verify Fleet Agent is running
3. Review client logs
4. Manual update via admin dashboard

### Policy Not Applied

**Check**:
1. Client received policy (check `applied_policies` in storage)
2. Policy format is valid JSON
3. Client has required permissions
4. Restart browser

[Full Documentation →](Zero-Touch-Deployment)

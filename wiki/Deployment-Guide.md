# Deployment Guide

## Enterprise Deployment of E-Nation OS

This guide covers deployment scenarios for government agencies, from single-user installations to large-scale fleet deployments.

---

## 🎯 Deployment Scenarios

### Scenario 1: Single User / Pilot Deployment
**Scale**: 1-10 users  
**Method**: Manual installation  
**Timeline**: 1 day

### Scenario 2: Department Deployment
**Scale**: 10-100 users  
**Method**: MDM-assisted deployment  
**Timeline**: 1 week

### Scenario 3: Agency-Wide Deployment
**Scale**: 100-1000 users  
**Method**: Full MDM integration  
**Timeline**: 2-4 weeks

### Scenario 4: Government-Wide Deployment
**Scale**: 1000+ users  
**Method**: Centralized MDM, phased rollout  
**Timeline**: 2-6 months

---

## 📋 Prerequisites

### System Requirements

**Hardware**:
- macOS computer (2018 or newer recommended)
- 8GB RAM minimum, 16GB+ recommended
- 10GB free disk space
- Internet connection for satellite data access

**Software**:
- macOS 11.0 (Big Sur) or later
- Administrator access for installation

### Network Requirements

**Firewall Rules**:
- HTTPS (443) for satellite data sources
- Update server access for Sparkle updates
- Corporate proxy configuration (if applicable)

**Data Sources**:
- Google Earth Engine API access
- Copernicus Sentinel data portal
- USGS Landsat services

---

## 🚀 Quick Start (Single User)

### Step 1: Download

Obtain E-Nation OS from authorized source:
- Government distribution portal
- DeepIntel® direct download
- Authorized reseller

### Step 2: Install

```bash
# Mount DMG (if distributed as disk image)
open "E-Nation OS.dmg"

# Drag to Applications folder
cp -R "E-Nation OS.app" /Applications/

# Launch
open "/Applications/E-Nation OS.app"
```

### Step 3: Configure

First launch configuration:
1. Accept privacy and security settings
2. Set E-Nation OS as default browser (optional)
3. Configure search engine preferences
4. Enable automatic updates

### Step 4: Verify

Test core functionality:
- Browse to test website
- Open GeoIntel side panel
- Access Kenya News feed
- Verify security settings

---

## 🏢 Enterprise Deployment

### MDM Integration

**Supported MDM Platforms**:
- Jamf Pro (recommended for macOS)
- Kandji
- Microsoft Intune
- Addigy

### Jamf Pro Deployment

**1. Upload Package**:
```bash
# Create PKG installer
pkgbuild --root /Applications/E-Nation\ OS.app \
         --identifier co.ke.deepintel.enationos \
         --version 1.0 \
         --install-location /Applications/E-Nation\ OS.app \
         E-Nation-OS.pkg

# Upload to Jamf Pro
jamf-pro upload-package E-Nation-OS.pkg
```

**2. Create Policy**:
- Name: "Install E-Nation OS"
- Trigger: Enrollment, recurring check-in
- Frequency: Ongoing
- Scope: Target computer groups

**3. Configuration Profile**:
```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN">
<plist version="1.0">
<dict>
    <key>PayloadContent</key>
    <array>
        <dict>
            <key>PayloadType</key>
            <string>co.ke.deepintel.enationos</string>
            <key>DefaultSearchEngine</key>
            <string>National Search Service</string>
            <key>AutoUpdateEnabled</key>
            <true/>
            <key>TelemetryEnabled</key>
            <false/>
        </dict>
    </array>
</dict>
</plist>
```

**4. Deploy**:
- Test with pilot group
- Monitor deployment success
- Roll out to additional groups
- Validate functionality

---

## ⚙️ Configuration Management

### Policy Settings

**Security Policies**:
```json
{
  "security": {
    "minimumTLSVersion": "TLS1.3",
    "blockMixedContent": true,
    "enforceHTTPS": true,
    "disableWebRTC": false
  }
}
```

**Privacy Policies**:
```json
{
  "privacy": {
    "telemetryEnabled": false,
    "crashReportsEnabled": false,
    "blockThirdPartyCookies": true,
    "enableDoH": true,
    "customDNS": "10.0.1.53"
  }
}
```

**Feature Policies**:
```json
{
  "features": {
    "geoIntelEnabled": true,
    "kenyaNewsEnabled": true,
    "extensionsAllowed": true,
    "incognitoAllowed": true
  }
}
```

### Customization

**Branding**:
- Replace app icon (requires rebuild)
- Customize about page
- Set organizational homepage
- Configure default bookmarks

**Search Engines**:
```json
{
  "searchEngines": [
    {
      "name": "National Search",
      "url": "https://search.gov.ke/?q={searchTerms}",
      "default": true
    },
    {
      "name": "DuckDuckGo",
      "url": "https://duckduckgo.com/?q={searchTerms}"
    }
  ]
}
```

---

## 📊 Fleet Management

### Monitoring

**Key Metrics**:
- Installation success rate
- Version distribution
- Active users
- Crash rate
- Security incidents

**Monitoring Tools**:
- MDM console dashboards
- Audit log analysis
- SIEM integration
- Custom analytics

### Update Management

**Update Policy**:
```yaml
updates:
  automatic: true
  schedule:
    day: Tuesday
    time: "03:00"
  critical:
    immediate: true
  testing:
    pilot_group: "IT Department"
    duration_days: 3
```

**Staged Rollout**:
1. Deploy to IT department (day 0)
2. Monitor for issues (days 1-3)
3. Deploy to pilot users (day 4)
4. Monitor feedback (days 4-7)
5. Full deployment (day 8+)

---

## 🔐 Security Hardening

### Certificate Management

**Government CA Installation**:
```bash
# Install government root CA
sudo security add-trusted-cert \
  -d -r trustRoot \
  -k /Library/Keychains/System.keychain \
  GovRootCA.crt
```

### Network Security

**Proxy Configuration**:
```bash
# System-wide proxy (via MDM)
defaults write /Library/Preferences/SystemConfiguration/preferences \
  ProxyHTTPEnable -bool true
defaults write /Library/Preferences/SystemConfiguration/preferences \
  ProxyHTTPProxy proxy.gov.ke
defaults write /Library/Preferences/SystemConfiguration/preferences \
  ProxyHTTPPort -int 8080
```

### Access Control

**Restrict Installation**:
- Require administrator privileges
- Digital signature verification
- Gatekeeper enforcement
- App notarization check

---

## 🧪 Testing & Validation

### Pre-Deployment Testing

**Test Plan**:
1. **Installation**: Verify clean install on test system
2. **Functionality**: Test all core features
3. **Integration**: Verify MDM management
4. **Performance**: Benchmark resource usage
5. **Security**: Penetration testing
6. **User Acceptance**: Pilot user feedback

**Test Cases**:
- Basic web browsing
- GeoIntel platform access
- Kenya News functionality
- Extension installation
- Policy enforcement
- Update mechanism

### Post-Deployment Validation

**Health Checks**:
```bash
# Verify installation
ls -la "/Applications/E-Nation OS.app"

# Check version
defaults read "/Applications/E-Nation OS.app/Contents/Info" CFBundleVersion

# Test launch
open -a "E-Nation OS" --args --version
```

---

## 🆘 Troubleshooting

### Common Issues

**Issue**: App won't launch  
**Solution**: Check Gatekeeper, verify code signature

**Issue**: GeoIntel not loading  
**Solution**: Verify internet connectivity, check API credentials

**Issue**: Updates not installing  
**Solution**: Check Sparkle permissions, firewall rules

**Issue**: Policy not applying  
**Solution**: Verify MDM profile deployment, check preferences

### Support Escalation

**Level 1**: User helpdesk  
**Level 2**: IT department  
**Level 3**: DeepIntel® support (support@deepintel.co.ke)

---

## 📈 Training

### User Training

**Basic User** (30 minutes):
- Navigation and interface
- Web browsing basics
- Privacy features
- Basic troubleshooting

**Power User** (2 hours):
- Advanced features
- GeoIntel platform
- Extensions and customization
- Security best practices

**Analyst** (1 day):
- GEOINT workflows
- OSINT techniques
- Data export and analysis
- Advanced satellite data usage

### Administrator Training

**IT Administrator** (1 day):
- Deployment methods
- MDM integration
- Policy configuration
- Troubleshooting
- Update management

---

## 📖 Related Documentation

- [Features & Capabilities](Features-and-Capabilities) - What E-Nation OS can do
- [Security & Privacy](Security-and-Privacy) - Security configuration
- [Technical Architecture](Technical-Architecture) - System design
- [Best Practices](Best-Practices) - Operational guidelines

---

## 📞 Deployment Support

For deployment assistance:
- **Email**: deployment@deepintel.co.ke
- **Documentation**: This wiki
- **Training**: training@deepintel.co.ke

---

**E-Nation OS: Deploy with confidence.**

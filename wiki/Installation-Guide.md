# Installation Guide

This guide will help you install E-Nation OS on your system.

## System Requirements

### Minimum Requirements
- **OS**: macOS 10.15+ (Catalina or later)
- **Processor**: Intel Core i5 or Apple M1/M2
- **RAM**: 8 GB
- **Storage**: 2 GB available space
- **Display**: 1280x800 resolution

### Recommended Requirements
- **OS**: macOS 12+ (Monterey or later)
- **Processor**: Intel Core i7 or Apple M1 Pro/M2 Pro
- **RAM**: 16 GB
- **Storage**: 4 GB available space
- **Display**: 1920x1080 resolution or higher

## Installation Steps

### For End Users

#### 1. Download E-Nation OS
```bash
# Download the latest release
curl -L https://github.com/deepintel/e-nation-os/releases/latest/download/E-Nation-OS.dmg -o E-Nation-OS.dmg
```

#### 2. Install the Application
1. Double-click `E-Nation-OS.dmg`
2. Drag **E-Nation OS** to your **Applications** folder
3. Eject the disk image

#### 3. First Launch
1. Open **Applications** → **E-Nation OS**
2. Right-click and select "Open" (first time only on macOS)
3. Click "Open" in the security dialog

#### 4. Initial Setup
On first launch, you'll be prompted to:
1. **E-Gov Sign-In** - Enter your government credentials
2. **Biometric Setup** - Configure FaceID/TouchID (optional)
3. **Telemetry Consent** - Choose whether to share anonymous usage data

### For IT Administrators

#### Zero-Touch Deployment

1. **Download the Package**
```bash
curl -L https://github.com/deepintel/e-nation-os/releases/latest/download/E-Nation-OS-Enterprise.pkg -o E-Nation-OS.pkg
```

2. **Create Configuration Profile**
```bash
# Create deployment profile
cat > deployment-config.json << EOF
{
  "deployment_type": "enterprise",
  "admin_server": "https://fleet.gov.ke",
  "auto_update": true,
  "forced_signin": true,
  "whitelist_enabled": true,
  "vpn_auto_connect": true
}
EOF
```

3. **Deploy to Fleet**
```bash
# Using Jamf Pro
sudo jamf policy -event install-enation-os

# Using Munki
sudo /usr/local/munki/managedsoftwareupdate --installonly
```

4. **Verify Installation**
```bash
# Check if installed
ls -la /Applications/E-Nation\ OS.app

# Verify version
/Applications/E-Nation\ OS.app/Contents/MacOS/E-Nation\ OS --version
```

## Building from Source

### Prerequisites
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install dependencies
brew install python3 nodejs git
```

### Clone Repository
```bash
git clone https://github.com/deepintel/e-nation-os.git
cd e-nation-os
```

### Build Steps
```bash
# 1. Initialize Chromium source
./init_chromium.sh

# 2. Apply E-Nation OS patches
./apply_patches.sh

# 3. Build (this takes 2-4 hours)
./bulletproof_build.sh
```

### Build Output
```bash
# Application will be at:
./out/Release/E-Nation\ OS.app
```

## Post-Installation

### Verify Installation

1. **Check Application**
```bash
open -a "E-Nation OS"
```

2. **Verify Extension Loaded**
- Click extension icon in toolbar
- You should see the E-Nation OS popup

3. **Test Features**
- Open new tab (should show E-Nation OS landing page)
- Try Research Agent with a simple query
- Check VPN status in feature bar

### Configure Permissions

E-Nation OS requires the following macOS permissions:

1. **Camera** - For Police Field Ops and Border Control
2. **Microphone** - For Voice Control
3. **Location** - For GeoIntel features
4. **Network** - For VPN and API connections

Grant these in: **System Preferences** → **Security & Privacy** → **Privacy**

## Troubleshooting

### "App is damaged" Error

macOS Gatekeeper may block unsigned builds. To fix:

```bash
# Remove quarantine attribute
sudo xattr -r -d com.apple.quarantine /Applications/E-Nation\ OS.app

# Or disable Gatekeeper temporarily (not recommended)
sudo spctl --master-disable
```

### Extension Not Loading

1. Check extension is enabled:
   - Go to `chrome://extensions`
   - Ensure "E-Nation OS Agentic Core" is enabled

2. Reload extension:
   - Click reload button on extension card

### Build Failures

If build fails:

1. Check logs:
```bash
tail -f build.log
```

2. Clean and retry:
```bash
./clean_build.sh
./bulletproof_build.sh
```

3. Check system resources:
```bash
# Ensure enough disk space (need 100GB+)
df -h

# Check memory
vm_stat
```

## Uninstallation

### Complete Removal

```bash
# Remove application
sudo rm -rf /Applications/E-Nation\ OS.app

# Remove user data
rm -rf ~/Library/Application\ Support/E-Nation\ OS

# Remove preferences
rm -rf ~/Library/Preferences/co.ke.deepintel.enation-os.*

# Remove caches
rm -rf ~/Library/Caches/E-Nation\ OS
```

### Enterprise Uninstall

```bash
# Using Jamf
sudo jamf policy -event remove-enation-os

# Using Munki
sudo /usr/local/munki/managedsoftwareupdate --remove=E-Nation-OS
```

## Next Steps

After installation:
1. Read the [First Launch Setup](First-Launch-Setup) guide
2. Explore [Agentic Intelligence](Agentic-Intelligence) features
3. Review [Security & Privacy](Privacy-Telemetry) settings

---

**Need Help?** Contact support@deepintel.co.ke

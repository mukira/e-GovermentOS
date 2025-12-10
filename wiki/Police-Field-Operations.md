# Police Field Operations

The **Police Field Ops** module provides law enforcement officers with AI-powered facial recognition and automatic license plate reading (ALPR) capabilities directly in the browser.

## 🎯 Purpose

Enable field officers to:
- Identify suspects using facial recognition
- Scan and verify license plates
- Check against national criminal databases
- Document evidence with timestamps

## 🔐 Access Requirements

**Clearance Level**: Level 3 or higher (Director+)

**Permissions**:
- Camera access
- Law enforcement credentials
- Active duty status

## 🚀 How to Access

### Method 1: Extension Popup
1. Click the **E-Nation OS** extension icon in toolbar
2. Select the **"Field Ops"** agent card (badge icon)
3. UI switches to Police Mode (red/blue theme)

### Method 2: New Tab Page
1. Open a new tab
2. Click the **"Police Field Ops"** card
3. Extension opens with Police Mode activated

### Method 3: Keyboard Shortcut
- Press `Alt + P` (when extension is focused)

## 🎨 Interface Overview

When Police Mode is activated:

### Visual Changes
- **Theme**: Dark red/blue with pulsing siren effect
- **Background**: Emergency services colors
- **Scanner**: Reticle overlay for targeting
- **Camera Feed**: Live webcam preview

### UI Elements
```
┌─────────────────────────────────┐
│  👮 POLICE FIELD OPERATIONS     │
│                                 │
│  [Camera Feed with Reticle]     │
│                                 │
│  Status: Ready to Scan          │
│                                 │
│  [INITIATE SCAN] Button         │
│                                 │
│  Results Panel (hidden)         │
└─────────────────────────────────┘
```

## 📷 Facial Recognition

### Step-by-Step Guide

#### 1. Activate Scanner
- Click **"INITIATE SCAN"** button
- Grant camera permission if prompted
- Camera feed activates

#### 2. Position Subject
- Align subject's face within the **reticle** (circular overlay)
- Ensure good lighting
- Face should be clearly visible
- Wait for autofocus

#### 3. Capture & Analyze
- System automatically captures when face is centered
- AI processes biometric features (~2.5 seconds)
- Queries national criminal database

#### 4. Review Results

**Clean Result** (No criminal record):
```
┌─────────────────────────────────┐
│ ✅ CLEAN                         │
│                                 │
│ Name: JOHN DOE                  │
│ ID: 12345678                    │
│ DOB: 1985-03-15                 │
│                                 │
│ No warrants or criminal record  │
│                                 │
│ Confidence: 97.3%               │
└─────────────────────────────────┘
```

**Wanted Result** (Active warrants):
```
┌─────────────────────────────────┐
│ ⚠️ WANTED                        │
│                                 │
│ Name: JANE SMITH                │
│ ID: 87654321                    │
│ DOB: 1990-07-22                 │
│                                 │
│ 🚨 ACTIVE WARRANT               │
│ Charge: Armed Robbery           │
│ Issued: 2024-11-15              │
│ Court: Nairobi High Court       │
│                                 │
│ ⚠️ DETAIN IMMEDIATELY           │
│                                 │
│ Confidence: 99.1%               │
└─────────────────────────────────┘
```

**No Match**:
```
┌─────────────────────────────────┐
│ ❌ NO MATCH                      │
│                                 │
│ Subject not in database         │
│                                 │
│ Recommend manual verification   │
└─────────────────────────────────┘
```

### Result Fields Explained

| Field | Description |
|-------|-------------|
| **Name** | Full legal name from national ID database |
| **ID** | National ID number |
| **DOB** | Date of birth |
| **Status** | CLEAN / WANTED / NO MATCH |
| **Warrant** | Active warrants (if any) |
| **Charge** | Criminal charges |
| **Confidence** | AI confidence score (0-100%) |

### Confidence Score Guide
- **95-100%**: High confidence - Positive match
- **85-94%**: Medium confidence - Likely match, verify
- **<85%**: Low confidence - Manual verification required

## 🚗 License Plate Recognition (ALPR)

### Step-by-Step Guide

#### 1. Switch to ALPR Mode
- In Police Mode, click **"ALPR"** tab
- Scanner switches to plate detection

#### 2. Position Vehicle
- Point camera at license plate
- Ensure plate is within frame
- Maintain stable position
- Wait for OCR to activate

#### 3. Automatic Detection
- System detects plate boundaries
- OCR extracts plate number
- Queries motor vehicle database

#### 4. Review Results

**Registered Vehicle**:
```
┌─────────────────────────────────┐
│ ✅ REGISTERED                    │
│                                 │
│ Plate: KCA 123X                 │
│ Make: Toyota Hilux              │
│ Year: 2020                      │
│ Color: White                    │
│                                 │
│ Owner: TRANSPORT LTD            │
│ Status: Current                 │
│ Insurance: Valid (Exp 2025-06)  │
│                                 │
│ No alerts                       │
└─────────────────────────────────┘
```

**Stolen Vehicle**:
```
┌─────────────────────────────────┐
│ 🚨 STOLEN VEHICLE ALERT         │
│                                 │
│ Plate: KBZ 789Y                 │
│ Make: Nissan X-Trail            │
│ Year: 2019                      │
│                                 │
│ Reported stolen: 2024-11-20     │
│ Location: Nairobi               │
│ Case: CR/2024/1234              │
│                                 │
│ ⚠️ APPROACH WITH CAUTION        │
│ ⚠️ DO NOT STOP ALONE            │
└─────────────────────────────────┘
```

## 🔒 Privacy & Evidence

### Data Handling
- **Retention**: Scan results stored for 90 days
- **Audit Trail**: All scans logged with timestamp, location, officer ID
- **Encryption**: All data encrypted at rest and in transit
- **Access Control**: Only authorized personnel can view scan history

### Evidence Documentation
Every scan automatically creates:
1. **Timestamp** - Exact time of scan
2. **Location** - GPS coordinates (if available)
3. **Officer ID** - Performing officer
4. **Photo** - Captured image (if consent given)
5. **Confidence Score** - AI accuracy

### Legal Compliance
- **GDPR Compliant** - Data protection standards met
- **Chain of Custody** - Full audit trail maintained
- **Court Admissible** - Evidence format meets legal requirements

## ⚙️ Settings & Configuration

### Scanner Settings
Access via: **Settings** → **Police Field Ops**

- **Auto-Capture**: Enable/disable automatic face detection
- **Confidence Threshold**: Minimum score for positive match (default: 85%)
- **Database Scope**: National / Regional / Local
- **Photo Retention**: 30 / 60 / 90 days

### Camera Settings
- **Resolution**: 720p / 1080p / 4K
- **Frame Rate**: 30fps / 60fps
- **Night Mode**: IR enhancement for low-light
- **Stabilization**: Digital image stabilization

## 🚨 Emergency Features

### Quick Actions
- **Panic Button**: Sends distress signal with location
- **Backup Request**: Alerts nearest units
- **Evidence Upload**: Immediately uploads scans to secure server

### Officer Safety
- **Location Tracking**: Real-time GPS logging
- **Activity Monitor**: Supervisors can view live status
- **Check-In Timer**: Automatic alerts if no activity

## 📊 Reports & Analytics

### Generate Reports
1. Click **"Reports"** in Police Mode
2. Select date range
3. Choose report type:
   - Daily Activity Summary
   - Warrant Executions
   - Vehicle Stops
   - Evidence Log

### Export Options
- **PDF**: Court-ready format with signatures
- **CSV**: For database import
- **JSON**: API integration

## 🛠️ Troubleshooting

### Camera Not Working
1. Check camera permissions in System Preferences
2. Restart E-Nation OS
3. Verify camera is working: `Settings` → `Privacy` → `Camera`

### Low Confidence Scores
- **Improve lighting** - Use flash or move to better lit area
- **Reduce motion** - Keep subject and camera stable
- **Clean lens** - Wipe camera lens
- **Optimal distance** - 1-2 meters from subject

### Database Connection Failed
1. Check internet connection
2. Verify VPN is connected
3. Contact IT support for credentials

## 📞 Support

### Training
- **Online Course**: academy.deepintel.co.ke/police-ops
- **Video Tutorials**: YouTube channel
- **PDF Manual**: Download from admin dashboard

### Contact
- **24/7 Hotline**: +254-700-POLICE
- **Email**: fieldops@deepintel.co.ke
- **Emergency**: Panic button in app

---

**⚠️ WARNING**: This tool is for **authorized law enforcement use only**. Misuse may result in criminal charges and termination of access.

**Next**: Learn about [Border Control](Border-Control) features

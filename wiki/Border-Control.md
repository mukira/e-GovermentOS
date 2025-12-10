# Border Control Module

Immigration control powered by AI for passport verification and Interpol database integration.

## 🎯 Purpose

Enable immigration officers to:
- Scan passport MRZ (Machine Readable Zone)
- Verify traveler identity
- Check against Interpol Red Notices
- Process entries/exits efficiently

## 🔐 Access Requirements

**Clearance Level**: Level 3+ (Immigration Officers)

## 🚀 How to Access

1. Click E-Nation OS extension icon
2. Select **"Border"** agent card
3. UI switches to Immigration Mode (green/gold theme)

## 📄 Passport Scanning

### Step-by-Step:

1. **Activate Scanner** - Click "SCAN PASSPORT"
2. **Position Passport** - Align MRZ within gold frame overlay
3. **Automatic OCR** - System reads MRZ lines (~3 seconds)
4. **Interpol Check** - Queries international database
5. **Review Results** - Entry allowed or alert displayed

### MRZ Format
```
P<KENJOHN<<DOE<<<<<<<<<<<<<<<<<<<<<<<<<<<
1234567890KEN8505225M2503159<<<<<<<<<<<<<<
```

## 🚨 Result Types

**Entry Allowed** ✅:
- No Interpol alerts
- Valid passport
- Green status badge

**Interpol Alert** 🚨:
- Red Notice detected
- Display: suspect details, charges, issuing country
- Action: Detain immediately, notify supervisor

## 📊 Data Captured
- Full name, nationality, DOB
- Passport number & expiry
- Interpol status
- Entry/exit timestamp

[Full Documentation →](Border-Control)

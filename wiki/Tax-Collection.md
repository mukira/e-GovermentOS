# Tax Collection (KRA)

TIN verification and automated duty calculation for Kenya Revenue Authority.

## 🎯 Purpose

Help KRA officers:
- Verify taxpayer TINs instantly
- Calculate import duties automatically
- Check compliance status
- Generate tax breakdowns

## 🔐 Access Requirements

**Clearance Level**: Level 4+ (KRA Officers)

## 🚀 How to Access

1. Click E-Nation OS extension
2. Select **"Revenue"** agent card
3. UI switches to Revenue Mode (red/black theme)

## 💰 TIN Verification

### Input Format:
```
P05XXXXXXXXX  (P05 + 8 digits + 1 letter)
```

### Steps:
1. Enter TIN in calculator
2. System queries iTax database
3. Returns: Name, Status (Active/Suspended), Compliance level

## 🧮 Duty Calculation

### Inputs:
- **Declared Value** (CIF in KES)
- **Excise Goods** (checkbox for alcohol/fuel)

### Automatic Calculation:
- Import Duty (25%)
- Excise Duty (20% if applicable)
- VAT (16%)
- IDF (3.5%)
- RDL (2.0%)

### Example:
```
Input: KES 100,000
Output:
  Import Duty:  KES 25,000
  VAT:          KES 20,000
  IDF:          KES 3,500
  RDL:          KES 2,000
  ──────────────────────
  TOTAL TAXES:  KES 50,500
  TOTAL PAYABLE: KES 150,500
```

## 📋 Features
- ✅ Real-time iTax integration
- ✅ Automatic tax breakdown
- ✅ Compliance checking
- ✅ Export to PDF

[Full Documentation →](Tax-Collection)

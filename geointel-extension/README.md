# GeoIntel - Satellite Intelligence Platform

World-class satellite intelligence Chrome extension built with Material UI and Google Maps.

## 🚀 Quick Start

### Installation
1. Open Chrome: `chrome://extensions/`
2. Enable "Developer mode"
3. Click "Load unpacked"
4. Select: `/Users/Mukira/Downloads/BrowserOS/geointel-extension/dist`

### Usage
- Click GeoIntel icon in toolbar
- Search locations or ask natural language questions
- View real-time satellite analysis
- Export reports

## ✨ Features

- **Material UI Design** - Premium Material Design 3 components
- **Google Maps** - Professional mapping with dark theme
- **Glassmorphism** - Modern UI with backdrop blur effects
- **Real-time Analysis** - NDVI, NDWI, NDBI spectral indices
- **Export** - Download analysis reports as JSON

## 🎨 Design

**Colors:**
- Primary: Vibrant Cyan `#00D9FF`
- Secondary: Electric Purple `#9C27B0`
- Success: Neon Green `#00FF88`
- Background: Deep Dark `#0A0E27`

**Built with:**
- React 18
- Material UI v5
- Google Maps JavaScript API
- Vite

## 📦 Development

```bash
# Install dependencies
npm install

# Development mode
npm run dev

# Build extension
npm run build
```

## 🔑 API Key

Google Maps API key is hardcoded in `index.html`:
```
AIzaSyBp7r2D1nmfSnKysVgJjLOpeEQ7KAQkp2E
```

No additional setup required!

## 📁 Structure

```
geointel-extension/
├── dist/              # Built extension (load this in Chrome)
├── src/               # React source code
│   ├── components/    # UI components
│   ├── theme/         # Material UI theme
│   └── App.jsx        # Main app
├── public/            # Static assets
└── package.json       # Dependencies
```

## 🎯 Components

- **MapView** - Google Maps with dark styling
- **SearchBar** - Geocoding and natural language
- **ResultsPanel** - Analysis visualization
- **ControlPanel** - Speed Dial FABs
- **Timeline** - Temporal controls

## ✅ Verified Working

- ✓ Material UI theme applied
- ✓ Google Maps loads with dark theme
- ✓ Search and geocoding functional
- ✓ Results panel displays metrics
- ✓ Control panels expand
- ✓ All animations smooth

## 📚 Documentation

See walkthrough for complete details on architecture, design system, and usage.

---

**Built with ❤️ using Material UI, Google Maps, and React**

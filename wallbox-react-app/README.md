# Wallbox Controller - React App

A React application for **charging control** of your wallbox charging station when a car is connected.

## Purpose

This web interface provides **charging session control** for connected vehicles:

- **Pause** - Temporarily pause active charging
- **Continue** - Resume paused charging
- **Stop** - End charging session

All controls are **context-aware** and only enabled when a car is in charging mode.

System operations (start, enable/disable wallbox) are controlled via:

- **Simulator** (development mode)
- **Hardware pins** (production mode)

## Features

- ✅ Real-time status monitoring (every 2 seconds)
- ⏸️ **Pause Charging** - Pause active charging session
- ▶️ **Continue Charging** - Resume paused session
- 🛑 **Stop Charging** - End charging session
- 📊 Enhanced status display with values
- 🚗 Car connection detection
- 🎨 Context-aware UI (buttons only active when applicable)
- 💡 Smart state indicators

## Context-Aware Controls

The interface intelligently enables/disables controls based on charging state:

| State        | Pause          | Continue       | Stop           | Status          |
| ------------ | -------------- | -------------- | -------------- | --------------- |
| **No Car**   | ❌ Disabled    | ❌ Disabled    | ❌ Disabled    | Waiting for car |
| **CHARGING** | ✅ **Enabled** | ❌ Disabled    | ✅ **Enabled** | Car is charging |
| **PAUSED**   | ❌ Disabled    | ✅ **Enabled** | ✅ **Enabled** | Charging paused |

## Prerequisites

1. **Wallbox Controller API** must be running:

   ```bash
   cd ../WallboxCtrl/build
   ./wallbox_control_api
   ```

2. **Node.js** installed (v14 or higher)

## Installation

```bash
# Install dependencies
npm install
```

## Running the App

```bash
# Start the development server
npm start
```

The app will open in your browser at `http://localhost:3000`

## Usage

1. **Start the wallbox controller API** first (see Prerequisites)
2. **Open the React app** in your browser
3. The app will automatically connect and show the current status
4. **Start charging via simulator/hardware pins**
5. Once car is charging, use the web interface to:
   - **Pause** - Temporarily pause charging
   - **Continue** - Resume after pausing
   - **Stop** - End the charging session

### System Operations (Not on Web Interface)

- **Start Charging** → Use simulator or hardware pins
- **Enable/Disable Wallbox** → Use simulator or hardware pins

## What You'll See

### Status Panel (Enhanced with Values)

- **State** - Current charging state (IDLE, CHARGING, PAUSED, etc.)
- **Wallbox** - System enabled/disabled status
- **Relay** - Main contactor status (ON/OFF)
- **Charging** - Active/Inactive indicator
- **Mode** - Car connection status (🚗 Car Connected / ○ No Car)
- **Timestamp** - Unix timestamp value
- **Last update** - Human-readable time

### Control Panel

**Context-Aware Buttons:**

- **Pause Charging** (⏸️) - Only enabled when state is CHARGING
- **Continue Charging** (▶️) - Only enabled when state is PAUSED
- **STOP CHARGING** (⏹️) - Enabled when car is charging or paused

**Smart Status Messages:**

- Shows current state and available actions
- Color-coded feedback (green for active, orange for paused)
- Clear indication when controls are disabled

## How It Works

The React app communicates with the wallbox controller through REST API:

```
React App (Port 3000) → HTTP API → Wallbox Controller (Port 8080)
```

### API Endpoints Used

- `GET /api/status` - Get current status (auto-refresh every 2s)
- `POST /api/charging/stop` - **Emergency stop charging** 🛑

### API Endpoints Used by Web App

- `GET /api/status` - Get current status (auto-refresh every 2s)
- `POST /api/charging/pause` - **Pause charging** (only when CHARGING)
- `POST /api/charging/resume` - **Continue charging** (only when PAUSED)
- `POST /api/charging/stop` - **Stop charging** (emergency stop)

### Other Available Endpoints (Simulator/Hardware Only)

These endpoints exist but are NOT used by the web interface:

- `POST /api/charging/start` - Start charging (use simulator/pins)
- `POST /api/wallbox/enable` - Enable wallbox (use simulator/pins)
- `POST /api/wallbox/disable` - Disable wallbox (use simulator/pins)

## Project Structure

```
wallbox-react-app/
├── public/
│   └── index.html          # HTML template
├── src/
│   ├── api/
│   │   └── wallboxApi.js   # API client
│   ├── App.js              # Main component
│   ├── App.css             # Styling
│   ├── index.js            # Entry point
│   └── index.css           # Global styles
├── package.json            # Dependencies
└── README.md              # This file
```

## Troubleshooting

### "Cannot connect to wallbox controller"

**Solution**: Make sure the wallbox controller API is running:

```bash
cd ../WallboxCtrl/build
./wallbox_control_api
```

You should see:

```
HTTP API Server started on port 8080
React app can connect to: http://localhost:8080
```

### CORS Issues

The API server has CORS enabled, but if you still have issues:

1. Make sure you're accessing the React app from `http://localhost:3000`
2. Check that the API is running on `http://localhost:8080`

### Port Already in Use

If port 3000 is already in use:

```bash
# Kill the process using port 3000
lsof -ti:3000 | xargs kill -9

# Or run on a different port
PORT=3001 npm start
```

## Building for Production

```bash
npm run build
```

This creates an optimized production build in the `build/` folder.

## Dependencies

- **React** (^18.2.0) - UI framework
- **Axios** (^1.6.2) - HTTP client for API calls
- **React Scripts** (5.0.1) - Build tools

## Screenshots

### Main Interface

- Beautiful gradient background
- Real-time status updates
- Large, easy-to-use control buttons

### Connection Error State

- Clear error messages
- Step-by-step instructions to resolve
- Helpful troubleshooting guide

## License

MIT

## Support

For issues or questions, check:

1. Wallbox controller is running (`./wallbox_control_api`)
2. React app is on port 3000
3. API server is on port 8080
4. No firewall blocking localhost connections

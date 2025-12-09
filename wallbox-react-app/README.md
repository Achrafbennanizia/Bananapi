# Wallbox Controller - React App

A simple React application to control your wallbox charging station.

## Features

- ✅ Real-time status updates (every 2 seconds)
- 🔴 **Stop charging** with one click
- ▶️ Start charging
- ⏸️ Pause/Resume charging
- 🎛️ Enable/Disable wallbox
- 📊 Live status display
- 🎨 Beautiful modern UI

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
4. Use the buttons to control charging:
   - **Start Charging** - Begin charging your vehicle
   - **Stop Charging** - Stop the charging process ⭐
   - **Pause** - Temporarily pause charging
   - **Resume** - Continue charging after pause
   - **Enable/Disable Wallbox** - System control

## What You'll See

### Status Panel

- Current state (IDLE, CHARGING, PAUSED, etc.)
- Wallbox enabled/disabled
- Relay status (ON/OFF)
- Charging status
- Last update timestamp

### Control Buttons

All buttons are context-aware:

- Disabled when action is not available
- Color-coded for easy identification
- Instant feedback on actions

## How It Works

The React app communicates with the wallbox controller through REST API:

```
React App (Port 3000) → HTTP API → Wallbox Controller (Port 8080)
```

### API Endpoints Used

- `GET /api/status` - Get current status (auto-refresh every 2s)
- `POST /api/charging/start` - Start charging
- `POST /api/charging/stop` - Stop charging ⭐
- `POST /api/charging/pause` - Pause charging
- `POST /api/charging/resume` - Resume charging
- `POST /api/wallbox/enable` - Enable wallbox
- `POST /api/wallbox/disable` - Disable wallbox

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

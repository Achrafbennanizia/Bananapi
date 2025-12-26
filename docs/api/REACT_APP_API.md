# REST API Documentation for React App Integration

## Overview

The Wallbox Controller includes a RESTful HTTP API server for **charging session control** and status monitoring. The API runs on port **8080** by default and includes CORS support for cross-origin requests.

### Web Interface Purpose

The React web app provides **charging session control** for connected vehicles:

- **Pause** - Temporarily pause active charging
- **Continue** - Resume paused charging
- **Stop** - End charging session

All controls are **context-aware** and only enabled when a car is in charging mode.

System operations (start charging, enable/disable wallbox) are controlled via:

- **Simulator** (development mode)
- **Hardware pins** (production mode)

This design ensures:

- **Safety**: Remote control of active charging sessions
- **Context-Aware**: Controls only work when applicable
- **Control**: System initialization remains on local simulator/hardware
- **Simplicity**: Clear, focused interface

## Base URL

```
http://localhost:8080
```

## API Endpoints

### 1. Health Check

**GET** `/health`

Check if the API server is running.

**Response:**

```json
{
  "status": "healthy",
  "service": "Wallbox Controller API",
  "version": "2.0.0"
}
```

---

### 2. Get Status

**GET** `/api/status`

Get the current status of the wallbox.

**Response:**

```json
{
  "state": "IDLE",
  "wallboxEnabled": true,
  "relayEnabled": false,
  "charging": false,
  "timestamp": 1733782800
}
```

**States:**

- `IDLE` - Ready, no vehicle connected
- `PREPARING` - Vehicle detected, preparing
- `CHARGING` - Active charging
- `PAUSED` - Charging paused
- `FINISHING` - Charging complete
- `ERROR` - Error state

---

### 3. Pause Charging (Web Interface)

**POST** `/api/charging/pause`

Pause the active charging process. Can be resumed later.

**Context:** Only enabled in web interface when state is `CHARGING`

**Response:**

```json
{
  "success": true,
  "message": "Charging paused",
  "state": "PAUSED"
}
```

**Usage:**

- Used by React web app to pause active charging
- Only works when car is actively charging
- Charging can be resumed with `/api/charging/resume`

---

### 4. Resume Charging (Web Interface)

**POST** `/api/charging/resume`

Resume charging after it has been paused.

**Context:** Only enabled in web interface when state is `PAUSED`

**Response:**

```json
{
  "success": true,
  "message": "Charging resumed",
  "state": "CHARGING"
}
```

**Usage:**

- Used by React web app to continue paused charging
- Only works when charging is in PAUSED state
- Returns to CHARGING state

---

### 5. Stop Charging (Web Interface)

**POST** `/api/charging/stop`

Stop the charging process completely. Ends the charging session.

**Context:** Enabled when car is charging or paused

**Response:**

```json
{
  "success": true,
  "message": "Charging stopped",
  "state": "IDLE"
}
```

**Usage:**

- Used by React web app to stop charging
- Works in both CHARGING and PAUSED states
- Emergency stop capability
- Ends the current charging session

---

### 6. Start Charging (Simulator/Hardware Only)

**POST** `/api/charging/start`

Start the charging process.

⚠️ **Note:** This endpoint is **NOT used by the web interface**. Use simulator or hardware pins to start charging.

**Response (Success):**

```json
{
  "success": true,
  "message": "Charging started",
  "state": "CHARGING"
}
```

**Response (Error):**

```json
{
  "error": "Failed to start charging"
}
```

---

### 7. Enable Wallbox (Simulator/Hardware Only)

**POST** `/api/wallbox/enable`

Enable the wallbox system.

⚠️ **Note:** This endpoint is **NOT used by the web interface**. Use simulator or hardware pins.

**Response:**

```json
{
  "success": true,
  "message": "Wallbox enabled",
  "enabled": true
}
```

---

### 8. Disable Wallbox (Simulator/Hardware Only)

**POST** `/api/wallbox/disable`

Disable the wallbox system (stops charging if active).

⚠️ **Note:** This endpoint is **NOT used by the web interface**. Use simulator or hardware pins.

**Response:**

```json
{
  "success": true,
  "message": "Wallbox disabled",
  "enabled": false
}
```

---

### 9. Get Relay Status

**GET** `/api/relay`

Get the current relay status.

**Response:**

```json
{
  "relayEnabled": false,
  "state": "IDLE"
}
```

---

## React App Integration Example

### Design Philosophy

The React web app is intentionally limited to:

- **Status Monitoring** (read-only)
- **Emergency Stop** (single write operation)

All other controls (start, pause, enable/disable) remain on simulator/hardware.

### Install Dependencies

```bash
npm install axios
# or
npm install fetch
```

### Simplified API Service (wallboxApi.js)

```javascript
import axios from "axios";

const API_BASE_URL = "http://localhost:8080";

class WallboxAPI {
  // Get current status (used by web app)
  async getStatus() {
    const response = await axios.get(`${API_BASE_URL}/api/status`);
    return response.data;
  }

  // Stop charging - EMERGENCY STOP (used by web app)
  async stopCharging() {
    const response = await axios.post(`${API_BASE_URL}/api/charging/stop`);
    return response.data;
  }

  // Health check
  async healthCheck() {
    const response = await axios.get(`${API_BASE_URL}/health`);
    return response.data;
  }

  // --- Following methods available but NOT used by web app ---
  // Use simulator or hardware pins for these operations

  async startCharging() {
    const response = await axios.post(`${API_BASE_URL}/api/charging/start`);
    return response.data;
  }

  async pauseCharging() {
    const response = await axios.post(`${API_BASE_URL}/api/charging/pause`);
    return response.data;
  }

  async resumeCharging() {
    const response = await axios.post(`${API_BASE_URL}/api/charging/resume`);
    return response.data;
  }

  async enableWallbox() {
    const response = await axios.post(`${API_BASE_URL}/api/wallbox/enable`);
    return response.data;
  }

  async disableWallbox() {
    const response = await axios.post(`${API_BASE_URL}/api/wallbox/disable`);
    return response.data;
  }

  async getRelayStatus() {
    const response = await axios.get(`${API_BASE_URL}/api/relay`);
    return response.data;
  }
}

export default new WallboxAPI();
```

### Simplified React Component Example

```javascript
import React, { useState, useEffect } from "react";
import wallboxAPI from "./wallboxApi";
import "./WallboxControl.css";

function WallboxControl() {
  const [status, setStatus] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  // Load status on mount and every 2 seconds
  useEffect(() => {
    loadStatus();
    const interval = setInterval(loadStatus, 2000);
    return () => clearInterval(interval);
  }, []);

  const loadStatus = async () => {
    try {
      const data = await wallboxAPI.getStatus();
      setStatus(data);
      setError(null);
    } catch (err) {
      setError("Failed to connect to wallbox controller");
      console.error(err);
    }
  };

  const handleEmergencyStop = async () => {
    setLoading(true);
    try {
      await wallboxAPI.stopCharging();
      await loadStatus(); // Refresh status
      setError(null);
    } catch (err) {
      setError(err.response?.data?.error || "Emergency stop failed");
    } finally {
      setLoading(false);
    }
  };

  if (!status) {
    return <div className="loading">Loading wallbox status...</div>;
  }

  return (
    <div className="wallbox-control">
      <h1>🔌 Wallbox Emergency Stop</h1>

      {error && <div className="error">{error}</div>}

      {/* Status Panel - Read Only */}
      <div className="status-panel">
        <h2>📊 Current Status</h2>
        <div className="status-item">
          <span>State:</span>
          <strong className={`state-${status.state.toLowerCase()}`}>
            {status.state}
          </strong>
        </div>
        <div className="status-item">
          <span>Wallbox:</span>
          <strong>{status.wallboxEnabled ? "✓ Enabled" : "✗ Disabled"}</strong>
        </div>
        <div className="status-item">
          <span>Relay:</span>
          <strong>{status.relayEnabled ? "⚡ ON" : "○ OFF"}</strong>
        </div>
        <div className="status-item">
          <span>Charging:</span>
          <strong>{status.charging ? "🔋 Yes" : "○ No"}</strong>
        </div>
      </div>

      {/* Emergency Stop Control - Single Button */}
      <div className="control-panel">
        <h2>🛑 Emergency Stop</h2>
        <p className="control-info">
          Use this button to immediately stop charging. All other operations
          must be controlled via simulator or hardware pins.
        </p>

        <button
          onClick={handleEmergencyStop}
          disabled={loading || !status.charging}
          className="btn-emergency-stop"
        >
          ⏹️ STOP CHARGING
        </button>

        {!status.charging && (
          <p className="info-text">
            ℹ️ Stop button is only active when charging is in progress
          </p>
        )}
      </div>

      <div className="info">
        Last updated: {new Date(status.timestamp * 1000).toLocaleString()}
      </div>

      <div className="help-text">
        <strong>To start, pause, or resume charging:</strong>
        <ul>
          <li>Development Mode: Use the simulator</li>
          <li>Production Mode: Use hardware pins</li>
        </ul>
      </div>
    </div>
  );
}

export default WallboxControl;
```

### CSS Styling (WallboxControl.css)

```css
.wallbox-control {
  max-width: 600px;
  margin: 2rem auto;
  padding: 2rem;
  font-family: Arial, sans-serif;
}

h1 {
  text-align: center;
  color: #333;
}

.loading,
.error {
  text-align: center;
  padding: 1rem;
  margin: 1rem 0;
}

.error {
  background-color: #fee;
  color: #c00;
  border: 1px solid #fcc;
  border-radius: 4px;
}

.status-panel,
.control-panel {
  background: #f9f9f9;
  padding: 1.5rem;
  margin: 1rem 0;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

h2 {
  margin-top: 0;
  color: #555;
  border-bottom: 2px solid #ddd;
  padding-bottom: 0.5rem;
}

.status-item {
  display: flex;
  justify-content: space-between;
  padding: 0.5rem 0;
  border-bottom: 1px solid #eee;
}

.status-item:last-child {
  border-bottom: none;
}

.state-idle {
  color: #666;
}
.state-preparing {
  color: #f90;
}
.state-charging {
  color: #0c0;
}
.state-paused {
  color: #f90;
}
.state-finishing {
  color: #09f;
}
.state-error {
  color: #c00;
}

.button-group {
  display: flex;
  gap: 1rem;
  margin: 1rem 0;
}

button {
  flex: 1;
  padding: 0.75rem 1.5rem;
  font-size: 1rem;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  transition: all 0.3s;
}

button:hover:not(:disabled) {
  transform: translateY(-2px);
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
}

button:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.btn-start {
  background-color: #4caf50;
  color: white;
}

.btn-stop {
  background-color: #f44336;
  color: white;
}

.btn-pause {
  background-color: #ff9800;
  color: white;
}

.btn-resume {
  background-color: #2196f3;
  color: white;
}

.btn-enable {
  background-color: #9c27b0;
  color: white;
}

.btn-disable {
  background-color: #607d8b;
  color: white;
}

.info {
  text-align: center;
  color: #888;
  font-size: 0.9rem;
  margin-top: 1rem;
}
```

## Testing with curl

```bash
# Check health
curl http://localhost:8080/health

# Get status
curl http://localhost:8080/api/status

# Start charging
curl -X POST http://localhost:8080/api/charging/start

# Stop charging
curl -X POST http://localhost:8080/api/charging/stop

# Pause charging
curl -X POST http://localhost:8080/api/charging/pause

# Resume charging
curl -X POST http://localhost:8080/api/charging/resume

# Enable wallbox
curl -X POST http://localhost:8080/api/wallbox/enable

# Disable wallbox
curl -X POST http://localhost:8080/api/wallbox/disable

# Get relay status
curl http://localhost:8080/api/relay
```

## Error Handling

All endpoints return standard HTTP status codes:

- **200 OK** - Request successful
- **201 Created** - Resource created
- **204 No Content** - Successful, no response body
- **400 Bad Request** - Invalid request or action not allowed
- **404 Not Found** - Endpoint not found
- **500 Internal Server Error** - Server error

Error responses include a JSON body:

```json
{
  "error": "Error message description"
}
```

## CORS Support

The API includes CORS headers to allow requests from React development servers:

```
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS
Access-Control-Allow-Headers: Content-Type, Authorization
```

## Running the API Server

```bash
cd build/bin
./wallbox_control_api
```

The server will start on port 8080 and display all available endpoints.

## Architecture

The HTTP API Server follows these design patterns:

- **Command Pattern**: Each endpoint represents a command
- **Facade Pattern**: Simplifies wallbox control for web clients
- **Dependency Injection**: Controller is injected into API handlers
- **SOLID Principles**: Single Responsibility, Open/Closed

## Security Considerations

**Note**: This is a development/local API. For production use, consider adding:

- Authentication (JWT tokens, API keys)
- HTTPS/TLS encryption
- Rate limiting
- Input validation
- Logging and monitoring
- Network firewall rules

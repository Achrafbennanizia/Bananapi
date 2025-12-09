# REST API Documentation for React App Integration

## Overview

The Wallbox Controller now includes a RESTful HTTP API server that allows control from web applications, including React apps. The API runs on port **8080** by default and includes CORS support for cross-origin requests.

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

### 3. Start Charging

**POST** `/api/charging/start`

Start the charging process.

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

### 4. Stop Charging

**POST** `/api/charging/stop`

Stop the charging process.

**Response:**

```json
{
  "success": true,
  "message": "Charging stopped",
  "state": "IDLE"
}
```

---

### 5. Pause Charging

**POST** `/api/charging/pause`

Pause the charging process (can be resumed).

**Response:**

```json
{
  "success": true,
  "message": "Charging paused",
  "state": "PAUSED"
}
```

---

### 6. Resume Charging

**POST** `/api/charging/resume`

Resume charging after pause.

**Response:**

```json
{
  "success": true,
  "message": "Charging resumed",
  "state": "CHARGING"
}
```

---

### 7. Enable Wallbox

**POST** `/api/wallbox/enable`

Enable the wallbox system.

**Response:**

```json
{
  "success": true,
  "message": "Wallbox enabled",
  "enabled": true
}
```

---

### 8. Disable Wallbox

**POST** `/api/wallbox/disable`

Disable the wallbox system (stops charging if active).

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

### Install Dependencies

```bash
npm install axios
# or
npm install fetch
```

### API Service (wallboxApi.js)

```javascript
import axios from "axios";

const API_BASE_URL = "http://localhost:8080";

class WallboxAPI {
  // Get current status
  async getStatus() {
    const response = await axios.get(`${API_BASE_URL}/api/status`);
    return response.data;
  }

  // Start charging
  async startCharging() {
    const response = await axios.post(`${API_BASE_URL}/api/charging/start`);
    return response.data;
  }

  // Stop charging
  async stopCharging() {
    const response = await axios.post(`${API_BASE_URL}/api/charging/stop`);
    return response.data;
  }

  // Pause charging
  async pauseCharging() {
    const response = await axios.post(`${API_BASE_URL}/api/charging/pause`);
    return response.data;
  }

  // Resume charging
  async resumeCharging() {
    const response = await axios.post(`${API_BASE_URL}/api/charging/resume`);
    return response.data;
  }

  // Enable wallbox
  async enableWallbox() {
    const response = await axios.post(`${API_BASE_URL}/api/wallbox/enable`);
    return response.data;
  }

  // Disable wallbox
  async disableWallbox() {
    const response = await axios.post(`${API_BASE_URL}/api/wallbox/disable`);
    return response.data;
  }

  // Get relay status
  async getRelayStatus() {
    const response = await axios.get(`${API_BASE_URL}/api/relay`);
    return response.data;
  }

  // Health check
  async healthCheck() {
    const response = await axios.get(`${API_BASE_URL}/health`);
    return response.data;
  }
}

export default new WallboxAPI();
```

### React Component Example

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

  const handleAction = async (action) => {
    setLoading(true);
    try {
      await action();
      await loadStatus(); // Refresh status
      setError(null);
    } catch (err) {
      setError(err.response?.data?.error || "Action failed");
    } finally {
      setLoading(false);
    }
  };

  if (!status) {
    return <div className="loading">Loading wallbox status...</div>;
  }

  return (
    <div className="wallbox-control">
      <h1>Wallbox Controller</h1>

      {error && <div className="error">{error}</div>}

      <div className="status-panel">
        <h2>Status</h2>
        <div className="status-item">
          <span>State:</span>
          <strong className={`state-${status.state.toLowerCase()}`}>
            {status.state}
          </strong>
        </div>
        <div className="status-item">
          <span>Wallbox:</span>
          <strong>{status.wallboxEnabled ? "Enabled" : "Disabled"}</strong>
        </div>
        <div className="status-item">
          <span>Relay:</span>
          <strong>{status.relayEnabled ? "ON" : "OFF"}</strong>
        </div>
        <div className="status-item">
          <span>Charging:</span>
          <strong>{status.charging ? "Yes" : "No"}</strong>
        </div>
      </div>

      <div className="control-panel">
        <h2>Controls</h2>

        <div className="button-group">
          <button
            onClick={() => handleAction(wallboxAPI.startCharging)}
            disabled={loading || status.charging}
            className="btn-start"
          >
            Start Charging
          </button>

          <button
            onClick={() => handleAction(wallboxAPI.stopCharging)}
            disabled={loading || !status.charging}
            className="btn-stop"
          >
            Stop Charging
          </button>
        </div>

        <div className="button-group">
          <button
            onClick={() => handleAction(wallboxAPI.pauseCharging)}
            disabled={loading || status.state !== "CHARGING"}
            className="btn-pause"
          >
            Pause
          </button>

          <button
            onClick={() => handleAction(wallboxAPI.resumeCharging)}
            disabled={loading || status.state !== "PAUSED"}
            className="btn-resume"
          >
            Resume
          </button>
        </div>

        <div className="button-group">
          <button
            onClick={() => handleAction(wallboxAPI.enableWallbox)}
            disabled={loading || status.wallboxEnabled}
            className="btn-enable"
          >
            Enable Wallbox
          </button>

          <button
            onClick={() => handleAction(wallboxAPI.disableWallbox)}
            disabled={loading || !status.wallboxEnabled}
            className="btn-disable"
          >
            Disable Wallbox
          </button>
        </div>
      </div>

      <div className="info">
        Last updated: {new Date(status.timestamp * 1000).toLocaleString()}
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
cd WallboxCtrl/build
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

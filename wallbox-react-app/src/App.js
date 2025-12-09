import React, { useState, useEffect } from 'react';
import wallboxAPI from './api/wallboxApi';
import './App.css';

function App() {
  const [status, setStatus] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const [connected, setConnected] = useState(false);

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
      setConnected(true);
      setError(null);
    } catch (err) {
      setConnected(false);
      setError('Cannot connect to wallbox controller. Make sure wallbox_control_api is running on port 8080.');
    }
  };

  const handleAction = async (actionFn, actionName) => {
    setLoading(true);
    setError(null);
    try {
      const result = await actionFn();
      console.log(`${actionName} result:`, result);
      await loadStatus();
    } catch (err) {
      setError(err.response?.data?.error || `Failed to ${actionName}`);
    } finally {
      setLoading(false);
    }
  };

  if (!connected || !status) {
    return (
      <div className="App">
        <div className="container">
          <h1>🔌 Wallbox Controller</h1>
          <div className="error-panel">
            <div className="error-icon">⚠️</div>
            <h2>Connection Error</h2>
            <p>{error || 'Connecting to wallbox controller...'}</p>
            <div className="help-text">
              <p><strong>To start the wallbox controller:</strong></p>
              <ol>
                <li>Open a terminal</li>
                <li>Navigate to: <code>WallboxCtrl/build</code></li>
                <li>Run: <code>./wallbox_control_api</code></li>
              </ol>
            </div>
          </div>
        </div>
      </div>
    );
  }

  return (
    <div className="App">
      <div className="container">
        <header className="app-header">
          <h1>🔌 Wallbox Controller</h1>
          <div className="connection-status">
            <span className="status-dot connected"></span>
            Connected
          </div>
        </header>

        {error && (
          <div className="alert error">
            {error}
          </div>
        )}

        <div className="status-panel">
          <h2>📊 Current Status</h2>
          <div className="status-grid">
            <div className="status-item">
              <span className="label">State:</span>
              <span className={`value state-badge state-${status.state.toLowerCase()}`}>
                {status.state}
              </span>
            </div>
            <div className="status-item">
              <span className="label">Wallbox:</span>
              <span className={`value ${status.wallboxEnabled ? 'text-success' : 'text-danger'}`}>
                {status.wallboxEnabled ? '✓ Enabled' : '✗ Disabled'}
              </span>
            </div>
            <div className="status-item">
              <span className="label">Relay:</span>
              <span className={`value ${status.relayEnabled ? 'text-success' : 'text-muted'}`}>
                {status.relayEnabled ? '⚡ ON' : '○ OFF'}
              </span>
            </div>
            <div className="status-item">
              <span className="label">Charging:</span>
              <span className={`value ${status.charging ? 'text-success charging-indicator' : 'text-muted'}`}>
                {status.charging ? '🔋 Yes' : '○ No'}
              </span>
            </div>
          </div>
          <div className="timestamp">
            Last update: {new Date(status.timestamp * 1000).toLocaleString()}
          </div>
        </div>

        <div className="control-panel">
          <h2>🎮 Controls</h2>
          
          <div className="control-section">
            <h3>Charging Control</h3>
            <div className="button-grid">
              <button
                onClick={() => handleAction(() => wallboxAPI.startCharging(), 'start charging')}
                disabled={loading || status.charging || !status.wallboxEnabled}
                className="btn btn-success btn-large"
              >
                ▶️ Start Charging
              </button>
              
              <button
                onClick={() => handleAction(() => wallboxAPI.stopCharging(), 'stop charging')}
                disabled={loading || !status.charging}
                className="btn btn-danger btn-large"
              >
                ⏹️ Stop Charging
              </button>
            </div>

            <div className="button-grid">
              <button
                onClick={() => handleAction(() => wallboxAPI.pauseCharging(), 'pause charging')}
                disabled={loading || status.state !== 'CHARGING'}
                className="btn btn-warning"
              >
                ⏸️ Pause
              </button>
              
              <button
                onClick={() => handleAction(() => wallboxAPI.resumeCharging(), 'resume charging')}
                disabled={loading || status.state !== 'PAUSED'}
                className="btn btn-info"
              >
                ⏯️ Resume
              </button>
            </div>
          </div>

          <div className="control-section">
            <h3>System Control</h3>
            <div className="button-grid">
              <button
                onClick={() => handleAction(() => wallboxAPI.enableWallbox(), 'enable wallbox')}
                disabled={loading || status.wallboxEnabled}
                className="btn btn-primary"
              >
                ✓ Enable Wallbox
              </button>
              
              <button
                onClick={() => handleAction(() => wallboxAPI.disableWallbox(), 'disable wallbox')}
                disabled={loading || !status.wallboxEnabled}
                className="btn btn-secondary"
              >
                ✗ Disable Wallbox
              </button>
            </div>
          </div>
        </div>

        <footer className="app-footer">
          <p>Wallbox Controller v2.0 - REST API Integration</p>
        </footer>
      </div>
    </div>
  );
}

export default App;

import React, { useState, useEffect } from 'react';
import wallboxAPI from './api/wallboxApi';
import logger from './utils/logger';
import './App.css';

function App() {
  const [status, setStatus] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const [connected, setConnected] = useState(false);

  // Load status on mount and every 2 seconds
  useEffect(() => {
    logger.info('React app started');
    loadStatus();
    const interval = setInterval(loadStatus, 2000);
    return () => {
      clearInterval(interval);
      logger.info('React app unmounted');
    };
  }, []);

  const loadStatus = async () => {
    try {
      const data = await wallboxAPI.getStatus();
      setStatus(data);
      setConnected(true);
      setError(null);
      logger.debug('Status loaded', data);
    } catch (err) {
      setConnected(false);
      const errorMsg = 'Cannot connect to wallbox controller. Make sure wallbox_control_api is running on port 8080.';
      setError(errorMsg);
      logger.error('Failed to load status', err.message);
    }
  };

  const handleAction = async (actionFn, actionName) => {
    setLoading(true);
    setError(null);
    logger.info(`Executing action: ${actionName}`);
    try {
      const result = await actionFn();
      logger.info(`Action ${actionName} successful`, result);
      await loadStatus();
    } catch (err) {
      const errorMsg = err.response?.data?.error || `Failed to ${actionName}`;
      setError(errorMsg);
      logger.error(`Action ${actionName} failed`, errorMsg);
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
          <h2>📊 Current Status & Values</h2>
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
                {status.charging ? '🔋 Active' : '○ Inactive'}
              </span>
            </div>
            <div className="status-item">
              <span className="label">Mode:</span>
              <span className="value">
                {status.state === 'CHARGING' || status.state === 'PAUSED' ? '🚗 Car Connected' : '○ No Car'}
              </span>
            </div>
            <div className="status-item">
              <span className="label">Timestamp:</span>
              <span className="value text-muted">
                {status.timestamp}
              </span>
            </div>
          </div>
          <div className="timestamp">
            Last update: {new Date(status.timestamp * 1000).toLocaleString()}
          </div>
        </div>

        <div className="control-panel">
          <h2>🎮 Charging Controls</h2>
          
          <div className="control-section">
            <p className="control-description">
              Control charging when a car is connected and in charging mode.
              Start and system controls are available via simulator or hardware pins.
            </p>
            
            {/* Pause/Continue buttons - only enabled when car is charging */}
            <div className="button-grid">
              <button
                onClick={() => handleAction(() => wallboxAPI.pauseCharging(), 'pause charging')}
                disabled={loading || status.state !== 'CHARGING'}
                className="btn btn-warning btn-large"
              >
                ⏸️ Pause Charging
              </button>
              
              <button
                onClick={() => handleAction(() => wallboxAPI.resumeCharging(), 'continue charging')}
                disabled={loading || status.state !== 'PAUSED'}
                className="btn btn-info btn-large"
              >
                ▶️ Continue Charging
              </button>
            </div>

            {/* Emergency Stop button */}
            <button
              onClick={() => handleAction(() => wallboxAPI.stopCharging(), 'stop charging')}
              disabled={loading || !status.charging}
              className="btn btn-danger btn-emergency"
            >
              ⏹️ STOP CHARGING
            </button>
            
            {(status.state !== 'CHARGING' && status.state !== 'PAUSED') && (
              <p className="info-text">
                ℹ️ Controls are only active when a car is in charging mode
              </p>
            )}
            {status.state === 'CHARGING' && (
              <p className="info-text text-success">
                ✓ Car is charging - Pause or Stop available
              </p>
            )}
            {status.state === 'PAUSED' && (
              <p className="info-text text-warning">
                ⏸️ Charging paused - Continue or Stop available
              </p>
            )}
          </div>
        </div>

        <footer className="app-footer">
          <p>Wallbox Controller v3.0 - Charging Control Interface</p>
          <p style={{ fontSize: '0.9rem', marginTop: '5px', color: '#888' }}>
            Use simulator or hardware pins to start charging and system control
          </p>
          <button 
            onClick={() => logger.downloadLogs()} 
            className="btn-log-download"
            style={{
              marginTop: '10px',
              padding: '8px 16px',
              fontSize: '0.85rem',
              background: '#667eea',
              color: 'white',
              border: 'none',
              borderRadius: '5px',
              cursor: 'pointer'
            }}
          >
            📥 Download Logs
          </button>
        </footer>
      </div>
    </div>
  );
}

export default App;

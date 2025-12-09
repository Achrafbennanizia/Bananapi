/**
 * Logger utility for React app
 * Logs to console and stores in localStorage for persistence
 */

const LOG_KEY = 'wallbox_app_logs';
const MAX_LOGS = 1000; // Maximum number of log entries to keep

class Logger {
  constructor() {
    this.logs = this.loadLogs();
  }

  loadLogs() {
    try {
      const stored = localStorage.getItem(LOG_KEY);
      return stored ? JSON.parse(stored) : [];
    } catch (e) {
      return [];
    }
  }

  saveLogs() {
    try {
      // Keep only the last MAX_LOGS entries
      const logsToSave = this.logs.slice(-MAX_LOGS);
      localStorage.setItem(LOG_KEY, JSON.stringify(logsToSave));
    } catch (e) {
      console.error('Failed to save logs:', e);
    }
  }

  getTimestamp() {
    const now = new Date();
    return now.toISOString();
  }

  log(level, message, data = null) {
    const entry = {
      timestamp: this.getTimestamp(),
      level,
      message,
      data
    };

    this.logs.push(entry);
    this.saveLogs();

    // Also log to console
    const consoleMessage = `[${entry.timestamp}] [${level}] ${message}`;
    switch (level) {
      case 'ERROR':
        console.error(consoleMessage, data || '');
        break;
      case 'WARN':
        console.warn(consoleMessage, data || '');
        break;
      case 'INFO':
        console.info(consoleMessage, data || '');
        break;
      case 'DEBUG':
        console.log(consoleMessage, data || '');
        break;
      default:
        console.log(consoleMessage, data || '');
    }
  }

  info(message, data) {
    this.log('INFO', message, data);
  }

  warn(message, data) {
    this.log('WARN', message, data);
  }

  error(message, data) {
    this.log('ERROR', message, data);
  }

  debug(message, data) {
    this.log('DEBUG', message, data);
  }

  getLogs() {
    return this.logs;
  }

  clearLogs() {
    this.logs = [];
    localStorage.removeItem(LOG_KEY);
  }

  exportLogs() {
    const logText = this.logs.map(entry => 
      `[${entry.timestamp}] [${entry.level}] ${entry.message}${entry.data ? ' ' + JSON.stringify(entry.data) : ''}`
    ).join('\n');
    
    return logText;
  }

  downloadLogs() {
    const logText = this.exportLogs();
    const blob = new Blob([logText], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `wallbox-logs-${new Date().toISOString()}.txt`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  }
}

// Create singleton instance
const logger = new Logger();

export default logger;

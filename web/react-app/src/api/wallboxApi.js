import axios from 'axios';
import logger from '../utils/logger';

const API_BASE_URL = process.env.REACT_APP_API_BASE_URL || 'http://localhost:8080';

class WallboxAPI {
  async getStatus() {
    try {
      logger.debug('API: Getting status');
      const response = await axios.get(`${API_BASE_URL}/api/status`);
      logger.debug('API: Status received', response.data);
      return response.data;
    } catch (error) {
      logger.error('API: Error getting status', error.message);
      throw error;
    }
  }

  async startCharging() {
    try {
      logger.info('API: Starting charging');
      const response = await axios.post(`${API_BASE_URL}/api/charging/start`);
      logger.info('API: Charging started', response.data);
      return response.data;
    } catch (error) {
      logger.error('API: Error starting charging', error.message);
      throw error;
    }
  }

  async stopCharging() {
    try {
      logger.info('API: Stopping charging');
      const response = await axios.post(`${API_BASE_URL}/api/charging/stop`);
      logger.info('API: Charging stopped', response.data);
      return response.data;
    } catch (error) {
      logger.error('API: Error stopping charging', error.message);
      throw error;
    }
  }

  async pauseCharging() {
    try {
      logger.info('API: Pausing charging');
      const response = await axios.post(`${API_BASE_URL}/api/charging/pause`);
      logger.info('API: Charging paused', response.data);
      return response.data;
    } catch (error) {
      logger.error('API: Error pausing charging', error.message);
      throw error;
    }
  }

  async resumeCharging() {
    try {
      logger.info('API: Resuming charging');
      const response = await axios.post(`${API_BASE_URL}/api/charging/resume`);
      logger.info('API: Charging resumed', response.data);
      return response.data;
    } catch (error) {
      logger.error('API: Error resuming charging', error.message);
      throw error;
    }
  }

  async enableWallbox() {
    try {
      logger.info('API: Enabling wallbox');
      const response = await axios.post(`${API_BASE_URL}/api/wallbox/enable`);
      logger.info('API: Wallbox enabled', response.data);
      return response.data;
    } catch (error) {
      logger.error('API: Error enabling wallbox', error.message);
      throw error;
    }
  }

  async disableWallbox() {
    try {
      logger.info('API: Disabling wallbox');
      const response = await axios.post(`${API_BASE_URL}/api/wallbox/disable`);
      logger.info('API: Wallbox disabled', response.data);
      return response.data;
    } catch (error) {
      logger.error('API: Error disabling wallbox', error.message);
      throw error;
    }
  }

  async healthCheck() {
    try {
      logger.debug('API: Health check');
      const response = await axios.get(`${API_BASE_URL}/health`);
      logger.debug('API: Health check OK', response.data);
      return response.data;
    } catch (error) {
      logger.error('API: Health check failed', error.message);
      throw error;
    }
  }
}

export default new WallboxAPI();

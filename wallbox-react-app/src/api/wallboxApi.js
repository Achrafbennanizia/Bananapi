import axios from 'axios';

const API_BASE_URL = 'http://localhost:8080';

class WallboxAPI {
  async getStatus() {
    try {
      const response = await axios.get(`${API_BASE_URL}/api/status`);
      return response.data;
    } catch (error) {
      console.error('Error getting status:', error);
      throw error;
    }
  }

  async startCharging() {
    try {
      const response = await axios.post(`${API_BASE_URL}/api/charging/start`);
      return response.data;
    } catch (error) {
      console.error('Error starting charging:', error);
      throw error;
    }
  }

  async stopCharging() {
    try {
      const response = await axios.post(`${API_BASE_URL}/api/charging/stop`);
      return response.data;
    } catch (error) {
      console.error('Error stopping charging:', error);
      throw error;
    }
  }

  async pauseCharging() {
    try {
      const response = await axios.post(`${API_BASE_URL}/api/charging/pause`);
      return response.data;
    } catch (error) {
      console.error('Error pausing charging:', error);
      throw error;
    }
  }

  async resumeCharging() {
    try {
      const response = await axios.post(`${API_BASE_URL}/api/charging/resume`);
      return response.data;
    } catch (error) {
      console.error('Error resuming charging:', error);
      throw error;
    }
  }

  async enableWallbox() {
    try {
      const response = await axios.post(`${API_BASE_URL}/api/wallbox/enable`);
      return response.data;
    } catch (error) {
      console.error('Error enabling wallbox:', error);
      throw error;
    }
  }

  async disableWallbox() {
    try {
      const response = await axios.post(`${API_BASE_URL}/api/wallbox/disable`);
      return response.data;
    } catch (error) {
      console.error('Error disabling wallbox:', error);
      throw error;
    }
  }

  async healthCheck() {
    try {
      const response = await axios.get(`${API_BASE_URL}/health`);
      return response.data;
    } catch (error) {
      console.error('Error checking health:', error);
      throw error;
    }
  }
}

export default new WallboxAPI();

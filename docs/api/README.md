# API Documentation

Complete API reference for the Wallbox Control System HTTP REST API and integration guides.

## 📡 HTTP REST API

The system exposes a RESTful HTTP API on port **8080** for remote monitoring and control.

### Base URL

```
http://<device-ip>:8080/api
```

### Quick Start

```bash
# Check API status
curl http://localhost:8080/api/health

# Get wallbox status
curl http://localhost:8080/api/status

# Start charging
curl -X POST http://localhost:8080/api/charging/start

# Stop charging
curl -X POST http://localhost:8080/api/charging/stop
```

## 📚 API Documentation Files

| Document                                 | Description                  | Audience            |
| ---------------------------------------- | ---------------------------- | ------------------- |
| [API_REFERENCE.md](API_REFERENCE.md)     | Complete API reference       | All developers      |
| [QUICK_REFERENCE.md](QUICK_REFERENCE.md) | Common commands and examples | Users, operators    |
| [REACT_APP_API.md](REACT_APP_API.md)     | React integration guide      | Frontend developers |

## 🔌 API Endpoints

### Health & Status

| Endpoint       | Method | Description    |
| -------------- | ------ | -------------- |
| `/api/health`  | GET    | Health check   |
| `/api/status`  | GET    | Wallbox status |
| `/api/version` | GET    | API version    |

### Charging Control

| Endpoint               | Method | Description     |
| ---------------------- | ------ | --------------- |
| `/api/charging/start`  | POST   | Start charging  |
| `/api/charging/stop`   | POST   | Stop charging   |
| `/api/charging/pause`  | POST   | Pause charging  |
| `/api/charging/resume` | POST   | Resume charging |
| `/api/charging/status` | GET    | Charging status |

### Wallbox Control

| Endpoint                 | Method | Description          |
| ------------------------ | ------ | -------------------- |
| `/api/wallbox/relay/on`  | POST   | Enable relay         |
| `/api/wallbox/relay/off` | POST   | Disable relay        |
| `/api/wallbox/config`    | GET    | Get configuration    |
| `/api/wallbox/config`    | PUT    | Update configuration |

### System Management

| Endpoint              | Method | Description        |
| --------------------- | ------ | ------------------ |
| `/api/system/info`    | GET    | System information |
| `/api/system/restart` | POST   | Restart service    |
| `/api/system/logs`    | GET    | Get system logs    |

## 📖 API Reference

### GET /api/status

Get current wallbox status.

**Response:**

```json
{
  "status": "ok",
  "wallbox": {
    "state": "CHARGING",
    "relay": "on",
    "connected": true,
    "power": 7400
  },
  "charging": {
    "active": true,
    "duration": 1234,
    "energy": 5.6
  }
}
```

### POST /api/charging/start

Start charging session.

**Request:**

```json
{
  "power": 7400,
  "maxDuration": 3600
}
```

**Response:**

```json
{
  "status": "started",
  "message": "Charging session started",
  "sessionId": "abc123"
}
```

### Error Responses

All endpoints return standard error responses:

```json
{
  "error": {
    "code": 400,
    "message": "Invalid parameter",
    "details": "Power must be between 0 and 22000W"
  }
}
```

**HTTP Status Codes:**

- `200 OK` - Success
- `400 Bad Request` - Invalid input
- `404 Not Found` - Endpoint not found
- `500 Internal Server Error` - Server error
- `503 Service Unavailable` - Service not ready

## 🔐 Authentication

### Current Version (v4.1)

No authentication required. Suitable for:

- Internal networks
- Testing environments
- Development

### Future Versions (v5.0+)

Planned authentication methods:

- API Key authentication
- JWT tokens
- OAuth 2.0
- mTLS for device authentication

## 🌐 CORS Configuration

CORS is enabled for all origins in development mode:

```javascript
// Allowed origins
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, PUT, DELETE
Access-Control-Allow-Headers: Content-Type, Authorization
```

**Production**: Configure specific allowed origins in `config/production.json`.

## 📱 Client Libraries

### JavaScript/TypeScript

```javascript
// Using fetch API
const response = await fetch("http://device-ip:8080/api/status");
const data = await response.json();
console.log(data.wallbox.state);
```

### Python

```python
import requests

# Get status
response = requests.get('http://device-ip:8080/api/status')
status = response.json()
print(status['wallbox']['state'])

# Start charging
requests.post('http://device-ip:8080/api/charging/start',
             json={'power': 7400})
```

### curl

```bash
# GET request
curl -X GET http://device-ip:8080/api/status

# POST request with JSON
curl -X POST http://device-ip:8080/api/charging/start \
  -H "Content-Type: application/json" \
  -d '{"power": 7400}'
```

### React

See [REACT_APP_API.md](REACT_APP_API.md) for complete React integration guide.

## 🔄 WebSocket API (Future)

Planned for v5.0:

```javascript
const ws = new WebSocket("ws://device-ip:8080/ws");

ws.onmessage = (event) => {
  const data = JSON.parse(event.data);
  console.log("Wallbox state:", data.state);
};
```

## 📊 Rate Limiting

Current limits (per client IP):

- 100 requests per minute
- 1000 requests per hour

Exceeded limits return `429 Too Many Requests`.

## 🧪 Testing the API

### Using curl

```bash
# Test all endpoints
./scripts/test/test_api.sh
```

### Using Postman

Import the Postman collection:

```
docs/api/postman_collection.json
```

### Using API Testing Tools

```bash
# Install HTTPie
brew install httpie

# Test API
http GET http://device-ip:8080/api/status
http POST http://device-ip:8080/api/charging/start power=7400
```

## 📝 API Versioning

Current version: **v1**

Future versions will use URL versioning:

```
http://device-ip:8080/api/v2/status
```

Breaking changes will increment the major version.

## 🐛 Troubleshooting

### API Not Responding

```bash
# Check if service is running
ps aux | grep wallbox_control

# Check port is open
lsof -i :8080

# Check firewall
sudo ufw status
```

### Connection Refused

```bash
# Verify API is enabled in config
cat config/production.json | grep api_enabled

# Check logs
tail -f /var/log/wallbox/api.log
```

### Slow Response Times

- Check network latency
- Review system resource usage
- Enable performance logging
- Consider load balancing (v5.0+)

## 📖 Related Documentation

- [API Reference](API_REFERENCE.md) - Complete endpoint documentation
- [Quick Reference](QUICK_REFERENCE.md) - Common commands
- [React Integration](REACT_APP_API.md) - Frontend integration
- [UDP Communication](../guides/UDP_SYNC_GUIDE.md) - Protocol-level communication

## 🤝 Contributing

API improvements are welcome! Please:

1. Follow RESTful design principles
2. Add comprehensive documentation
3. Include API tests
4. Update OpenAPI specification
5. Maintain backward compatibility

See [Contributing Guidelines](../../CONTRIBUTING.md) for details.

## 📚 Standards & References

- [REST API Design](https://restfulapi.net/)
- [HTTP Status Codes](https://httpstat.us/)
- [JSON API Specification](https://jsonapi.org/)
- [OpenAPI Specification](https://swagger.io/specification/)

---

_API documentation maintained by the core team. Last updated: v4.1.0 (December 2024)_

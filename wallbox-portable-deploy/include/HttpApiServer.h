#ifndef HTTP_API_SERVER_H
#define HTTP_API_SERVER_H

#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <map>

namespace Wallbox
{

    /**
     * @brief HTTP Request structure
     */
    struct HttpRequest
    {
        std::string method; // GET, POST, PUT, DELETE
        std::string path;   // /api/charging/start
        std::string body;   // JSON payload
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> params;
    };

    /**
     * @brief HTTP Response structure
     */
    struct HttpResponse
    {
        int statusCode = 200;
        std::string contentType = "application/json";
        std::string body;

        void setJson(const std::string &json)
        {
            contentType = "application/json";
            body = json;
        }

        void setError(int code, const std::string &message)
        {
            statusCode = code;
            body = "{\"error\":\"" + message + "\"}";
        }
    };

    /**
     * @brief Handler function type
     */
    using HttpHandler = std::function<void(const HttpRequest &, HttpResponse &)>;

    /**
     * @brief Simple HTTP REST API Server for React app integration
     *
     * Provides REST endpoints for controlling the wallbox from a web/React app.
     *
     * Design Patterns:
     * - Command Pattern: Each endpoint represents a command
     * - Observer Pattern: Notifies React app of state changes
     * - Facade Pattern: Simplifies wallbox control for web clients
     *
     * CORS enabled for React development server.
     */
    class HttpApiServer
    {
    public:
        HttpApiServer(int port = 8080);
        ~HttpApiServer();

        // Server lifecycle
        bool start();
        void stop();
        bool isRunning() const { return m_running; }

        // Route registration
        void registerRoute(const std::string &method, const std::string &path, HttpHandler handler);

        // Convenience methods for common HTTP methods
        void GET(const std::string &path, HttpHandler handler);
        void POST(const std::string &path, HttpHandler handler);
        void PUT(const std::string &path, HttpHandler handler);
        void DELETE(const std::string &path, HttpHandler handler);

    private:
        int m_port;
        int m_serverSocket;
        std::atomic<bool> m_running;
        std::thread m_serverThread;
        std::map<std::string, std::map<std::string, HttpHandler>> m_routes;

        void serverLoop();
        void handleClient(int clientSocket);
        HttpRequest parseRequest(const std::string &requestData);
        std::string buildResponse(const HttpResponse &response);
        void enableCORS(HttpResponse &response);
        HttpHandler findHandler(const std::string &method, const std::string &path);
    };

    /**
     * @brief Helper to build JSON responses
     */
    class JsonBuilder
    {
    public:
        JsonBuilder &add(const std::string &key, const std::string &value);
        JsonBuilder &add(const std::string &key, int value);
        JsonBuilder &add(const std::string &key, bool value);
        JsonBuilder &add(const std::string &key, double value);
        std::string build();

    private:
        std::string m_json = "{";
        bool m_first = true;
    };

} // namespace Wallbox

#endif // HTTP_API_SERVER_H

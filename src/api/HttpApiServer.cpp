#include "HttpApiServer.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Wallbox
{

    HttpApiServer::HttpApiServer(int port)
        : m_port(port), m_serverSocket(-1), m_running(false)
    {
    }

    HttpApiServer::~HttpApiServer()
    {
        stop();
    }

    bool HttpApiServer::start()
    {
        if (m_running)
        {
            std::cerr << "HTTP API Server already running" << std::endl;
            return false;
        }

        // Create socket
        m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_serverSocket < 0)
        {
            std::cerr << "Failed to create HTTP server socket" << std::endl;
            return false;
        }

        // Allow address reuse (helps with TIME_WAIT state)
        int opt = 1;
        if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            std::cerr << "Warning: Failed to set SO_REUSEADDR: " << strerror(errno) << std::endl;
        }
#ifdef SO_REUSEPORT
        if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
        {
            std::cerr << "Warning: Failed to set SO_REUSEPORT: " << strerror(errno) << std::endl;
        }
#endif

        // Bind to port
        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(m_port);

        if (bind(m_serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            std::cerr << "Failed to bind HTTP server to port " << m_port << ": " << strerror(errno) << std::endl;
            close(m_serverSocket);
            m_serverSocket = -1;
            return false;
        }

        // Listen
        if (listen(m_serverSocket, 10) < 0)
        {
            std::cerr << "Failed to listen on HTTP server socket" << std::endl;
            close(m_serverSocket);
            m_serverSocket = -1;
            return false;
        }

        m_running = true;
        m_serverThread = std::thread([this]()
                                     { serverLoop(); });

        std::cout << "HTTP API Server started on port " << m_port << std::endl;
        std::cout << "React app can connect to: http://localhost:" << m_port << std::endl;
        return true;
    }

    void HttpApiServer::stop()
    {
        if (!m_running)
        {
            return;
        }

        m_running = false;

        if (m_serverSocket >= 0)
        {
            close(m_serverSocket);
            m_serverSocket = -1;
        }

        if (m_serverThread.joinable())
        {
            m_serverThread.join();
        }

        std::cout << "HTTP API Server stopped" << std::endl;
    }

    void HttpApiServer::registerRoute(const std::string &method, const std::string &path, HttpHandler handler)
    {
        m_routes[method][path] = handler;
        std::cout << "Registered route: " << method << " " << path << std::endl;
    }

    void HttpApiServer::GET(const std::string &path, HttpHandler handler)
    {
        registerRoute("GET", path, handler);
    }

    void HttpApiServer::POST(const std::string &path, HttpHandler handler)
    {
        registerRoute("POST", path, handler);
    }

    void HttpApiServer::PUT(const std::string &path, HttpHandler handler)
    {
        registerRoute("PUT", path, handler);
    }

    void HttpApiServer::DELETE(const std::string &path, HttpHandler handler)
    {
        registerRoute("DELETE", path, handler);
    }

    void HttpApiServer::serverLoop()
    {
        while (m_running)
        {
            sockaddr_in clientAddr{};
            socklen_t clientLen = sizeof(clientAddr);

            int clientSocket = accept(m_serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
            if (clientSocket < 0)
            {
                if (m_running)
                {
                    std::cerr << "Failed to accept client connection" << std::endl;
                }
                continue;
            }

            // Handle client in a separate thread for better performance
            std::thread([this, clientSocket]()
                        { handleClient(clientSocket); })
                .detach();
        }
    }

    void HttpApiServer::handleClient(int clientSocket)
    {
        char buffer[4096];
        ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);

        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            std::string requestData(buffer);

            HttpRequest request = parseRequest(requestData);
            HttpResponse response;

            // Enable CORS for React app
            enableCORS(response);

            // Handle OPTIONS for CORS preflight
            if (request.method == "OPTIONS")
            {
                response.statusCode = 204;
                response.body = "";
            }
            else
            {
                // Find and execute handler
                HttpHandler handler = findHandler(request.method, request.path);
                if (handler)
                {
                    try
                    {
                        handler(request, response);
                    }
                    catch (const std::exception &e)
                    {
                        response.setError(500, std::string("Internal error: ") + e.what());
                    }
                }
                else
                {
                    response.setError(404, "Endpoint not found: " + request.method + " " + request.path);
                }
            }

            // Send response
            std::string responseStr = buildResponse(response);
            write(clientSocket, responseStr.c_str(), responseStr.length());
        }

        close(clientSocket);
    }

    HttpRequest HttpApiServer::parseRequest(const std::string &requestData)
    {
        HttpRequest request;
        std::istringstream stream(requestData);
        std::string line;

        // Parse request line (GET /path HTTP/1.1)
        if (std::getline(stream, line))
        {
            std::istringstream lineStream(line);
            lineStream >> request.method >> request.path;

            // Extract query parameters
            size_t queryPos = request.path.find('?');
            if (queryPos != std::string::npos)
            {
                std::string query = request.path.substr(queryPos + 1);
                request.path = request.path.substr(0, queryPos);

                // Parse query parameters (simplified)
                size_t pos = 0;
                while (pos < query.length())
                {
                    size_t eqPos = query.find('=', pos);
                    size_t ampPos = query.find('&', pos);
                    if (eqPos != std::string::npos)
                    {
                        std::string key = query.substr(pos, eqPos - pos);
                        std::string value = query.substr(eqPos + 1,
                                                         (ampPos != std::string::npos ? ampPos : query.length()) - eqPos - 1);
                        request.params[key] = value;
                    }
                    pos = (ampPos != std::string::npos) ? ampPos + 1 : query.length();
                }
            }
        }

        // Parse headers
        while (std::getline(stream, line) && line != "\r")
        {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos)
            {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 2);
                if (!value.empty() && value.back() == '\r')
                {
                    value.pop_back();
                }
                request.headers[key] = value;
            }
        }

        // Parse body
        std::string body;
        while (std::getline(stream, line))
        {
            body += line;
        }
        request.body = body;

        return request;
    }

    std::string HttpApiServer::buildResponse(const HttpResponse &response)
    {
        std::ostringstream oss;

        // Status line
        oss << "HTTP/1.1 " << response.statusCode << " ";
        switch (response.statusCode)
        {
        case 200:
            oss << "OK";
            break;
        case 201:
            oss << "Created";
            break;
        case 204:
            oss << "No Content";
            break;
        case 400:
            oss << "Bad Request";
            break;
        case 404:
            oss << "Not Found";
            break;
        case 500:
            oss << "Internal Server Error";
            break;
        default:
            oss << "Unknown";
            break;
        }
        oss << "\r\n";

        // Headers
        oss << "Content-Type: " << response.contentType << "\r\n";
        oss << "Content-Length: " << response.body.length() << "\r\n";
        oss << "Access-Control-Allow-Origin: *\r\n";
        oss << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
        oss << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
        oss << "Connection: close\r\n";
        oss << "\r\n";

        // Body
        oss << response.body;

        return oss.str();
    }

    void HttpApiServer::enableCORS(HttpResponse &response)
    {
        // CORS headers are added in buildResponse
    }

    HttpHandler HttpApiServer::findHandler(const std::string &method, const std::string &path)
    {
        auto methodIt = m_routes.find(method);
        if (methodIt != m_routes.end())
        {
            auto pathIt = methodIt->second.find(path);
            if (pathIt != methodIt->second.end())
            {
                return pathIt->second;
            }
        }
        return nullptr;
    }

    // JsonBuilder implementation
    JsonBuilder &JsonBuilder::add(const std::string &key, const std::string &value)
    {
        if (!m_first)
            m_json += ",";
        m_json += "\"" + key + "\":\"" + value + "\"";
        m_first = false;
        return *this;
    }

    JsonBuilder &JsonBuilder::add(const std::string &key, int value)
    {
        if (!m_first)
            m_json += ",";
        m_json += "\"" + key + "\":" + std::to_string(value);
        m_first = false;
        return *this;
    }

    JsonBuilder &JsonBuilder::add(const std::string &key, bool value)
    {
        if (!m_first)
            m_json += ",";
        m_json += "\"" + key + "\":" + (value ? "true" : "false");
        m_first = false;
        return *this;
    }

    JsonBuilder &JsonBuilder::add(const std::string &key, double value)
    {
        if (!m_first)
            m_json += ",";
        m_json += "\"" + key + "\":" + std::to_string(value);
        m_first = false;
        return *this;
    }

    std::string JsonBuilder::build()
    {
        return m_json + "}";
    }

} // namespace Wallbox

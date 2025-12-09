#ifndef API_CONTROLLER_H
#define API_CONTROLLER_H

#include "HttpApiServer.h"
#include "WallboxController.h"
#include <memory>

namespace Wallbox
{

    /**
     * @brief Controller for REST API endpoints
     *
     * Separates API routing logic from main application.
     * Uses Controller pattern from MVC architecture.
     *
     * Design Pattern: Controller (from MVC)
     * SOLID:
     * - Single Responsibility (handles API routing only)
     * - Dependency Inversion (depends on WallboxController interface)
     */
    class ApiController
    {
    public:
        /**
         * @brief Construct API controller
         * @param wallboxController Reference to wallbox controller
         */
        explicit ApiController(WallboxController &wallboxController)
            : m_wallboxController(wallboxController)
        {
        }

        /**
         * @brief Setup all API endpoints on the server
         * @param server HTTP server instance
         */
        void setupEndpoints(HttpApiServer &server)
        {
            setupHealthEndpoints(server);
            setupStatusEndpoints(server);
            setupChargingEndpoints(server);
            setupWallboxEndpoints(server);
        }

    private:
        WallboxController &m_wallboxController;

        /**
         * @brief Setup health check endpoints
         */
        void setupHealthEndpoints(HttpApiServer &server)
        {
            server.GET("/health", [](const HttpRequest &, HttpResponse &res)
                       {
                JsonBuilder json;
                json.add("status", "healthy")
                    .add("service", "Wallbox Controller API")
                    .add("version", "2.0.0");
                res.setJson(json.build()); });
        }

        /**
         * @brief Setup status query endpoints
         */
        void setupStatusEndpoints(HttpApiServer &server)
        {
            // GET /api/status - Get current wallbox status
            server.GET("/api/status", [this](const HttpRequest &, HttpResponse &res)
                       { res.setJson(m_wallboxController.getStatusJson()); });

            // GET /api/relay - Get relay status
            server.GET("/api/relay", [this](const HttpRequest &, HttpResponse &res)
                       {
                JsonBuilder json;
                json.add("relayEnabled", m_wallboxController.isRelayEnabled())
                    .add("state", m_wallboxController.getStateString());
                res.setJson(json.build()); });
        }

        /**
         * @brief Setup charging control endpoints
         */
        void setupChargingEndpoints(HttpApiServer &server)
        {
            // POST /api/charging/start
            server.POST("/api/charging/start", [this](const HttpRequest &, HttpResponse &res)
                        {
                if (m_wallboxController.startCharging()) {
                    JsonBuilder json;
                    json.add("success", true)
                        .add("message", "Charging started")
                        .add("state", m_wallboxController.getStateString());
                    res.setJson(json.build());
                } else {
                    res.setError(400, "Failed to start charging");
                } });

            // POST /api/charging/stop
            server.POST("/api/charging/stop", [this](const HttpRequest &, HttpResponse &res)
                        {
                if (m_wallboxController.stopCharging()) {
                    JsonBuilder json;
                    json.add("success", true)
                        .add("message", "Charging stopped")
                        .add("state", m_wallboxController.getStateString());
                    res.setJson(json.build());
                } else {
                    res.setError(400, "Failed to stop charging");
                } });

            // POST /api/charging/pause
            server.POST("/api/charging/pause", [this](const HttpRequest &, HttpResponse &res)
                        {
                if (m_wallboxController.pauseCharging()) {
                    JsonBuilder json;
                    json.add("success", true)
                        .add("message", "Charging paused")
                        .add("state", m_wallboxController.getStateString());
                    res.setJson(json.build());
                } else {
                    res.setError(400, "Failed to pause charging");
                } });

            // POST /api/charging/resume
            server.POST("/api/charging/resume", [this](const HttpRequest &, HttpResponse &res)
                        {
                if (m_wallboxController.resumeCharging()) {
                    JsonBuilder json;
                    json.add("success", true)
                        .add("message", "Charging resumed")
                        .add("state", m_wallboxController.getStateString());
                    res.setJson(json.build());
                } else {
                    res.setError(400, "Failed to resume charging");
                } });
        }

        /**
         * @brief Setup wallbox control endpoints
         */
        void setupWallboxEndpoints(HttpApiServer &server)
        {
            // POST /api/wallbox/enable
            server.POST("/api/wallbox/enable", [this](const HttpRequest &, HttpResponse &res)
                        {
                if (m_wallboxController.enableWallbox()) {
                    JsonBuilder json;
                    json.add("success", true)
                        .add("message", "Wallbox enabled")
                        .add("enabled", true);
                    res.setJson(json.build());
                } else {
                    res.setError(400, "Failed to enable wallbox");
                } });

            // POST /api/wallbox/disable
            server.POST("/api/wallbox/disable", [this](const HttpRequest &, HttpResponse &res)
                        {
                if (m_wallboxController.disableWallbox()) {
                    JsonBuilder json;
                    json.add("success", true)
                        .add("message", "Wallbox disabled")
                        .add("enabled", false);
                    res.setJson(json.build());
                } else {
                    res.setError(400, "Failed to disable wallbox");
                } });
        }
    };

} // namespace Wallbox

#endif // API_CONTROLLER_H

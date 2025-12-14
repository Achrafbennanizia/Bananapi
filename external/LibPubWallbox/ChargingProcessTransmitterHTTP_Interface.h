/*
    Geschrieben von:                     Mhd Adnan Matini
    Matrikelnummer:                      1104900 
    bei Fragen schreiben Sie bitte an:   mhd.matini@hs-osnabrueck.de
*/

#ifndef HTTP_CHARGING_PROCESS_TRANSMITTER_INTERFACE_H
#define HTTP_CHARGING_PROCESS_TRANSMITTER_INTERFACE_H

#include "Messages.h"
#include <string>

/** Interface class for http message transfer */
class ChargingProcessTransmitterHTTP_Interface {
public:
    // Constructor: Initializes the HTTP transmitter with the given host and port.
    ChargingProcessTransmitterHTTP_Interface(const std::string& serverUrl, int port);

    // Destructor.
    virtual ~ChargingProcessTransmitterHTTP_Interface();

    // Sends a message to signal the start of the charging process via HTTP.
    virtual void postChargingSession(const Entity& entity) = 0;

    // Sends a message to signal the end of the charging process via HTTP.
    virtual void postChargingPointState(const stChargingPointState& data) = 0;

    // Sends a message to signal the start of the charging process via HTTP.
    virtual void postSmartMeterState(const stSmartMeterReading& data) = 0;

protected:
    // Base URL constructed from host and port, e.g., "http://localhost:8080"
    std::string serverUrl;
};

#endif

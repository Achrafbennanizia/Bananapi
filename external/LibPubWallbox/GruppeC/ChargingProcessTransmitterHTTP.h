/*
    Geschrieben von:                     Mhd Adnan Matini
    Matrikelnummer:                      1104900 
    bei Fragen schreiben Sie bitte an:   mhd.matini@hs-osnabrueck.de
*/

#ifndef HTTP_CHARGING_PROCESS_TRANSMITTER_H
#define HTTP_CHARGING_PROCESS_TRANSMITTER_H

#include "ChargingProcessTransmitterHTTP_Interface.h"

class ChargingProcessTransmitterHTTP : ChargingProcessTransmitterHTTP_Interface {
public:
    // Constructor: Initializes the HTTP transmitter with the given host and port.
    ChargingProcessTransmitterHTTP(const std::string& host, int port);

    // Destructor.
    virtual ~ChargingProcessTransmitterHTTP();

    // Sends a message to signal the last processed charging session
    void postChargingSession(const Entity& entity);

    // Sends a message to signal the state of the charging point
    void postChargingPointState(const stChargingPointState& data);

    // Sends a message to signal the state of the smart meter
    void postSmartMeterState(const stSmartMeterReading& data);
};

#endif

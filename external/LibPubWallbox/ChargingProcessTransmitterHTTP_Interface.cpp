/*
    Geschrieben von:                     Mhd Adnan Matini
    Matrikelnummer:                      1104900 
    bei Fragen schreiben Sie bitte an:   mhd.matini@hs-osnabrueck.de
*/

#include "ChargingProcessTransmitterHTTP_Interface.h"

ChargingProcessTransmitterHTTP_Interface::ChargingProcessTransmitterHTTP_Interface(const std::string& serverUrl, int port)
{
	this->serverUrl = "http://" + serverUrl + ":" + std::to_string(port);
};

ChargingProcessTransmitterHTTP_Interface::~ChargingProcessTransmitterHTTP_Interface()
{
};

/*
    Geschrieben von:                     Mhd Adnan Matini
    Matrikelnummer:                      1104900 
    bei Fragen schreiben Sie bitte an:   mhd.matini@hs-osnabrueck.de
*/

#include "curl/include/curl.h"
#include <iostream>
#include <string>
#include "ChargingProcessTransmitterHTTP.h"

using namespace std;

ChargingProcessTransmitterHTTP::ChargingProcessTransmitterHTTP(const std::string& host, int port) : ChargingProcessTransmitterHTTP_Interface(host, port) {
    curl_global_init(CURL_GLOBAL_ALL);
}

ChargingProcessTransmitterHTTP::~ChargingProcessTransmitterHTTP() {
    curl_global_cleanup();
}

static void performHttpPost(const std::string &url, const std::string &data, const std::string &action) {
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(data.size()));

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << action << " sending to " << url << " failed:\n" << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << action << " sent via HTTP:\n" << data << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

// Sends a message to signal the last processed charging session
void ChargingProcessTransmitterHTTP::postChargingSession(const Entity& entity)
{
    string url = serverUrl + "/paybyplug/chargingPoint/session";
    string data = entity.toJsonString();
    performHttpPost(url, data, "Charging session");
}

// Sends a message to signal the state of the charging point
void ChargingProcessTransmitterHTTP::postChargingPointState(const stChargingPointState& data)
{
    string url = serverUrl + "/paybyplug/chargingPoint/state";
    string message = "{\n\"resChargingPointState\" :\n" + data.toJson() + "}\n";
    performHttpPost(url, message, "Charging point state");
}

// Sends a message to signal the state of the smart meter
void ChargingProcessTransmitterHTTP::postSmartMeterState(const stSmartMeterReading& data)
{
    string url = serverUrl + "/paybyplug/smartMeter/reading";
    string message = "{\n\"stPostSmartMeterReading\" :\n" + data.toJson() + "}\n";
    performHttpPost(url, message, "Smart meter reading");
}

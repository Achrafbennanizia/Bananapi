/*
    Geschrieben von:                     Mhd Adnan Matini
    Matrikelnummer:                      1104900 
    bei Fragen schreiben Sie bitte an:   mhd.matini@hs-osnabrueck.de
*/

#include "Messages.h"

#include <sstream>
#include <cstdint>

using namespace std;

string enChargingPointState_toString(enChargingPointState state)
{
	switch (state)
	{
	case enChargingPointState::free: return "free";
	case enChargingPointState::charging: return "charging";
	case enChargingPointState::completed: return "completed";
	default: return "error";
	}
}

// Constructor implementation: now includes chargingStationMeterReading
Entity::Entity(const std::string& vehicleId,
               const std::string& chargingStationId,
               uint64_t timestamp,
               double chargingStationMeterReading)
    : vehicleId(vehicleId),
      chargingStationId(chargingStationId),
      timestamp(timestamp),
      chargingStationMeterReading(chargingStationMeterReading) {}

// Destructor implementation
Entity::~Entity() {
    // Any necessary cleanup can be done here.
}

// toJsonString implementation: Converts the entity to a JSON string.
std::string Entity::toJsonString() const {
    std::ostringstream oss;
    oss << "{"
        << "\"vehicleId\":\"" << vehicleId << "\", "
        << "\"chargingStationId\":\"" << chargingStationId << "\", "
        << "\"timestamp\":" << timestamp << ", "
        << "\"chargingStationMeterReading\":" << chargingStationMeterReading
        << "}";
    return oss.str();
}

// Getter for chargingStationMeterReading
double Entity::getChargingStationMeterReading() const {
    return chargingStationMeterReading;
}

// Setter for chargingStationMeterReading
void Entity::setChargingStationMeterReading(double reading) {
    chargingStationMeterReading = reading;
}

/*******************************************************************
 * @brief Initializer
 * @param chargingPointId
 * @param friendlyName
 * @param state
 * @param consumptionKwh
 * @param time
 */
stChargingPointState::stChargingPointState(string chargingPointId,  const string friendlyName, enChargingPointState state, const double demandKwh, const uint64_t time) :
    chargingPointId(chargingPointId),
    friendlyName(friendlyName),
    state(state),
    demandKwh(demandKwh),
    time(time)
{
}

/**
 * @brief Sets the payload values of the struct
 * @param state
 * @param consumptionKwh
 * @param time
 */
void stChargingPointState::setState(const enChargingPointState state, const double demandKwh, uint64_t time)
{
    this->state = state;
    this->demandKwh = demandKwh;
    this->time = time;
}

/**
 * @brief JSON format string representation of the struct content
 * @return
 */
string stChargingPointState::toJson() const
{
    std::ostringstream oss;
    oss << "{\n"
        << "\t\"chargingPointId\" : \"" << chargingPointId << "\",\n"
        << "\t\"friendlyName\" : \"" << friendlyName << "\",\n"
        << "\t\"state\" : \"" << enChargingPointState_toString(state) << "\",\n"
        << "\t\"demandKwh\" : " << demandKwh << ",\n"
        << "\t\"time\" : " << time << "\n"
        << "}\n";
    return oss.str();
}

/*******************************************************************
 * @brief Initializer
 * @param meterId
 * @param friendlyName
 * @param consumptionKwh
 * @param deliveryKwh
 * @param time
 */
stSmartMeterReading::stSmartMeterReading(const string meterId, const string friendlyName, const double demandKwh, const double deliveryKwh, const uint64_t time) :
    meterId(meterId),
    friendlyName(friendlyName),
    demandKwh(demandKwh),
    deliveryKwh(deliveryKwh),
    time(time)
{
}

/**
 * @brief Sets the payload values of the struct
 * @param consumptionKwh
 * @param deliveryKwh
 * @param time
 */
void stSmartMeterReading::setReading(const double demandKwh, const double deliveryKwh, const uint64_t time)
{
    this->demandKwh = demandKwh;
    this->deliveryKwh = deliveryKwh;
    this->time = time;
}

/**
 * @brief JSON format string representation of the struct content
 * @return
 */
string stSmartMeterReading::toJson() const
{
    std::ostringstream oss;
    oss << "{\n"
        << "\t\"friendlyName\" : \"" << friendlyName << "\",\n"
        << "\t\"meterId\" : \"" << meterId << "\",\n"
        << "\t\"demandKwh\" : " << demandKwh << ",\n"
        << "\t\"deliveryKwh\" : " << deliveryKwh << ",\n"
        << "\t\"time\" : " << time << "\n"
        << "}\n";
    return oss.str();
}

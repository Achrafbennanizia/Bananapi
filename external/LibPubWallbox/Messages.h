#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>
#include <cstdint>

/*
    Geschrieben von:                     Mhd Adnan Matini
    Matrikelnummer:                      1104900 
    bei Fragen schreiben Sie bitte an:   mhd.matini@hs-osnabrueck.de
*/

enum class enChargingPointState
{
	free,
	charging,
	completed,
	error
};

std::string enChargingPointState_toString(enChargingPointState state);

class Entity {
public:
    // Constructor now accepts a uint64_t for the timestamp and a double for the meter reading.
    Entity(const std::string& vehicleId,
           const std::string& chargingStationId,
           uint64_t timestamp,
           double chargingStationMeterReading = 0.0);

    ~Entity();

    // Converts the Entity to a JSON string.
    std::string toJsonString() const;

    // Getter and setter for meter reading
    double getChargingStationMeterReading() const;
    void setChargingStationMeterReading(double reading);

private:
    std::string vehicleId;
    std::string chargingStationId;
    uint64_t timestamp;  // Changed from std::string to uint64_t.
    double chargingStationMeterReading;
};

/**
 * @brief Data content struct for a charging point state message
 */
struct stChargingPointState
{
    std::string 			chargingPointId;    ///< Identifyer of the electricity meter, usually the ID provided by the utility company.
    std::string 			friendlyName;   	///< User friendly name
    enChargingPointState	state; 				///< Usage state of the charging point
    double 					demandKwh;          ///< Meter reading value
    uint64_t 				time;              	///< Linux time stamp of meter reading

    stChargingPointState(std::string chargingPointId,  const std::string friendlyName, enChargingPointState state = enChargingPointState::free, double demandKwh = 0, uint64_t time = 0);  ///< @copybrief stChargingPointState::stChargingPointState
    void 		setState(const enChargingPointState state, const double demandKwh, const uint64_t time);   	///< @copybrief stChargingPointState::setState
    std::string toJson() const;                       														///< @copybrief stChargingPointState::toJson
};

/**
 * @brief Data content struct for the JSON post of an electricity meter reading
 */
struct stSmartMeterReading
{
    std::string 			meterId;        	///< Identifyer of the electricity meter, usually the ID provided by the utility company.
    std::string 			friendlyName;   	///< User friendly name
    double 					demandKwh;          ///< Meter reading value
    double 					deliveryKwh;        ///< Meter reading value
    uint64_t 				time;              	///< Linux time stamp of meter reading

    stSmartMeterReading(const std::string meterId, const std::string friendlyName, const double demandKwh = 0, const double deliveryKwh = 0, const uint64_t time = 0);  ///< @copybrief stSmartMeterReading::stSmartMeterReading
    void 		setReading(const double demandKwh, const double deliveryKwh, const uint64_t time);    		///< @copybrief stSmartMeterReading::setReading
    std::string toJson() const;                       														///< @copybrief stSmartMeterReading::toJsonString
};
#endif // ENTITY_H

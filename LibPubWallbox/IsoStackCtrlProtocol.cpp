// UTF8 (ü) //
/**
\file   cIsoStackCtrlProtocol.h

\author Dr. Olaf Simon
\author KEA - Science to Business GmbH - Hochschule Osnabrück
\date   2025-04-10

\addtogroup G_LibIso15118
@{
*/

// Removed dependency on ../LibCpp/HAL/HW_Tools.h
// Implementing BigEndian function locally

#include "IsoStackCtrlProtocol.h"
#include <algorithm>

using namespace std;
using namespace Iso15118;

// Helper function for byte swapping (big endian conversion)
namespace
{
    template <typename T>
    void BigEndian(T *value)
    {
        uint8_t *bytes = reinterpret_cast<uint8_t *>(value);
        std::reverse(bytes, bytes + sizeof(T));
    }
}

/**
 * @brief String representation of enIsoChargingState
 * @param value
 * @return
 */
std::string Iso15118::enIsoChargingState_toString(enIsoChargingState value)
{
    switch (value)
    {
    case enIsoChargingState::off:
        return "off";
    case enIsoChargingState::idle:
        return "idle";
    case enIsoChargingState::connected:
        return "connected";
    case enIsoChargingState::identification:
        return "identification";
    case enIsoChargingState::ready:
        return "ready";
    case enIsoChargingState::charging:
        return "charging";
    case enIsoChargingState::stop:
        return "stop";
    case enIsoChargingState::finished:
        return "finished";
    case enIsoChargingState::error:
        return "error";
    default:
        return "<undefined>";
    }
};

/**
 * @brief Constructor
 */
stIsoStackCmd::stIsoStackCmd()
{
    clear();
};

/**
 * @brief Sets the struct to initial values.
 */
void stIsoStackCmd::clear()
{
    msgVersion = 0;
    msgType = enIsoStackMsgType::CtrlCmd;
    enable = false;
    identification = true;
    currentDemand = 160;
    padding0 = 0;
}

/**
 * @brief Constructor
 */
stIsoStackState::stIsoStackState()
{
    clear();
};

/**
 * @brief Sets the struct to initial values.
 */
void stIsoStackState::clear()
{
    char stdSeccId[7] = {'Z', 'Z', '0', '0', '0', '0', '0'};
    uint8_t stdEvccId[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int i;

    msgVersion = 0;
    msgType = enIsoStackMsgType::CtrlState;
    state = enIsoChargingState::off;
    this->supplyPhases = enSupplyPhases::ac3;
    current = 0x8000;
    voltage = 0x8000;
    for (i = 0; i < 7; i++)
        seccId[i] = stdSeccId[i];
    charEnd = 0;
    for (i = 0; i < 8; i++)
        evccId[i] = stdEvccId[i];
    for (i = 0; i < 6; i++)
        evccMac[i] = 0;
    padding0[0] = 0;
    padding0[1] = 0;
    for (i = 0; i < 8; i++)
        sessionId[i] = 0;
    energyCapacity = 0;
    energyRequest = 0;
    departureTime = 0;
}

/**
 * @brief Returns an struct with byte order to or from big endian
 */
stIsoStackState stIsoStackState::bigEndian()
{
    stIsoStackState result = *this;

    BigEndian(&result.current);
    BigEndian(&result.voltage);
    BigEndian(&result.energyCapacity);
    BigEndian(&result.energyRequest);
    BigEndian(&result.departureTime);

    return result;
}

/**
 * @brief Constructor
 */
stSeHardwareCtrl::stSeHardwareCtrl()
{
    clear();
};

/**
 * @brief Sets the struct to initial values.
 */
void stSeHardwareCtrl::clear()
{
    mainContactor = 0;
    imd = 0;
    sourceEnable = false;
    sourceCurrentControl = false;
    sourceVoltage = 0;
    sourceCurrent = 0;
    padding0 = 0;
}

/**
 * @brief Constructor
 */
stSeIsoStackCmd::stSeIsoStackCmd() {
};

/**
 * @brief Sets the struct to initial values.
 */
void stSeIsoStackCmd::clear()
{
    isoStackCmd.clear();
    seHardwareState.clear();
}

/**
 * @brief Constructor
 */
stSeIsoStackState::stSeIsoStackState() {
};

/**
 * @brief Sets the struct to initial values.
 */
void stSeIsoStackState::clear()
{
    isoStackState.clear();
    seHardwareCmd.clear();
}

/** @} */

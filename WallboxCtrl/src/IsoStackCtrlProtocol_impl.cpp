// Simplified implementation of IsoStackCtrlProtocol methods without external dependencies
#include "../../LibPubWallbox/IsoStackCtrlProtocol.h"
#include <cstring>

using namespace Iso15118;

// Helper function for byte swapping
template <typename T>
void swapBytes(T *value)
{
    uint8_t *bytes = reinterpret_cast<uint8_t *>(value);
    for (size_t i = 0; i < sizeof(T) / 2; i++)
    {
        std::swap(bytes[i], bytes[sizeof(T) - 1 - i]);
    }
}

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
}

stIsoStackCmd::stIsoStackCmd()
{
    clear();
}

void stIsoStackCmd::clear()
{
    msgVersion = 0;
    msgType = enIsoStackMsgType::CtrlCmd;
    enable = false;
    identification = true;
    currentDemand = 160;
    padding0 = 0;
}

stIsoStackState::stIsoStackState()
{
    clear();
}

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

stIsoStackState stIsoStackState::bigEndian()
{
    stIsoStackState result = *this;

    swapBytes(&result.current);
    swapBytes(&result.voltage);
    swapBytes(&result.energyCapacity);
    swapBytes(&result.energyRequest);
    swapBytes(&result.departureTime);

    return result;
}

stSeHardwareCtrl::stSeHardwareCtrl()
{
    clear();
}

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

stSeIsoStackCmd::stSeIsoStackCmd()
{
}

void stSeIsoStackCmd::clear()
{
    isoStackCmd.clear();
    seHardwareState.clear();
}

stSeIsoStackState::stSeIsoStackState()
{
}

void stSeIsoStackState::clear()
{
    isoStackState.clear();
    seHardwareCmd.clear();
}

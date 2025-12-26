#include <iostream>
#include "IsoStackCtrlProtocol.h"

using namespace Iso15118;

int main()
{
    std::cout << "Testing IsoStackCtrlProtocol compilation...\n\n";

    // Test stIsoStackCmd
    stIsoStackCmd cmd;
    std::cout << "stIsoStackCmd created\n";
    std::cout << "  msgType: " << static_cast<int>(cmd.msgType) << "\n";
    std::cout << "  enable: " << (int)cmd.enable << "\n";
    std::cout << "  currentDemand: " << cmd.currentDemand << "\n";

    // Test stIsoStackState
    stIsoStackState state;
    std::cout << "\nstIsoStackState created\n";
    std::cout << "  msgType: " << static_cast<int>(state.msgType) << "\n";
    std::cout << "  state: " << enIsoChargingState_toString(state.state) << "\n";
    std::cout << "  voltage: " << state.voltage << "\n";
    std::cout << "  current: " << state.current << "\n";

    // Test stSeIsoStackCmd
    stSeIsoStackCmd seCmd;
    std::cout << "\nstSeIsoStackCmd created\n";
    std::cout << "  isoStackCmd.enable: " << (int)seCmd.isoStackCmd.enable << "\n";
    std::cout << "  seHardwareState.mainContactor: " << (int)seCmd.seHardwareState.mainContactor << "\n";

    // Test stSeIsoStackState
    stSeIsoStackState seState;
    std::cout << "\nstSeIsoStackState created\n";
    std::cout << "  isoStackState.state: " << enIsoChargingState_toString(seState.isoStackState.state) << "\n";
    std::cout << "  seHardwareCmd.mainContactor: " << (int)seState.seHardwareCmd.mainContactor << "\n";

    std::cout << "\n✓ All tests passed!\n";
    return 0;
}

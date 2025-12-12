build/main_v3.o: src/main_v3.cpp include/Application.h \
  include/Configuration.h include/WallboxController.h \
  include/IGpioController.h include/INetworkCommunicator.h \
  include/ChargingStateMachine.h \
  include/../../LibPubWallbox/IsoStackCtrlProtocol.h \
  include/HttpApiServer.h include/ApiController.h include/GpioFactory.h \
  include/StubGpioController.h include/BananaPiGpioController.h \
  include/UdpCommunicator.h

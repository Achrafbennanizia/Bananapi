#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "WallboxController.h"
#include "IGpioController.h"
#include "INetworkCommunicator.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

/**
 * @brief Mock GPIO Controller for testing
 *
 * Implements Mock Object pattern for unit testing.
 * Uses Google Mock framework.
 */
class MockGpioController : public IGpioController
{
public:
    MOCK_METHOD(bool, initialize, (), (override));
    MOCK_METHOD(void, shutdown, (), (override));
    MOCK_METHOD(bool, setPinMode, (int pin, PinMode mode), (override));
    MOCK_METHOD(bool, digitalWrite, (int pin, PinValue value), (override));
    MOCK_METHOD(PinValue, digitalRead, (int pin), (const, override));
    MOCK_METHOD(bool, isInitialized, (), (const, override));
};

/**
 * @brief Mock Network Communicator for testing
 */
class MockNetworkCommunicator : public INetworkCommunicator
{
public:
    MOCK_METHOD(bool, connect, (), (override));
    MOCK_METHOD(void, disconnect, (), (override));
    MOCK_METHOD(bool, send, (const std::vector<uint8_t> &data), (override));
    MOCK_METHOD(void, startReceiving, (MessageCallback callback), (override));
    MOCK_METHOD(void, stopReceiving, (), (override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
};

/**
 * @brief Unit tests for WallboxController
 *
 * Demonstrates dependency injection testing with mock objects.
 */
class WallboxControllerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create mock objects (nice mocks don't complain about unexpected calls)
        auto mockGpio = std::make_unique<NiceMock<MockGpioController>>();
        auto mockNetwork = std::make_unique<NiceMock<MockNetworkCommunicator>>();

        // Keep raw pointers for expectations (owned by controller)
        gpio = mockGpio.get();
        network = mockNetwork.get();

        // Set default behaviors
        ON_CALL(*gpio, initialize()).WillByDefault(Return(true));
        ON_CALL(*gpio, setPinMode(_, _)).WillByDefault(Return(true));
        ON_CALL(*gpio, digitalWrite(_, _)).WillByDefault(Return(true));
        ON_CALL(*gpio, digitalRead(_)).WillByDefault(Return(PinValue::LOW));
        ON_CALL(*gpio, isInitialized()).WillByDefault(Return(true));

        ON_CALL(*network, connect()).WillByDefault(Return(true));
        ON_CALL(*network, isConnected()).WillByDefault(Return(true));

        // Create controller with injected mocks
        controller = std::make_unique<WallboxController>(
            std::move(mockGpio),
            std::move(mockNetwork));
    }

    void TearDown() override
    {
        controller.reset();
    }

    std::unique_ptr<WallboxController> controller;
    MockGpioController *gpio;         // Non-owning pointer
    MockNetworkCommunicator *network; // Non-owning pointer
};

// Test: Controller initializes GPIO
TEST_F(WallboxControllerTest, InitializesGpio)
{
    EXPECT_CALL(*gpio, initialize()).Times(1).WillOnce(Return(true));
    EXPECT_TRUE(controller->initialize());
}

// Test: Controller initializes network
TEST_F(WallboxControllerTest, InitializesNetwork)
{
    EXPECT_CALL(*network, connect()).Times(1).WillOnce(Return(true));
    EXPECT_TRUE(controller->initialize());
}

// Test: Controller sets up GPIO pins
TEST_F(WallboxControllerTest, SetsUpGpioPins)
{
    EXPECT_CALL(*gpio, setPinMode(_, _)).Times(::testing::AtLeast(4));
    controller->initialize();
}

// Test: Initial state is IDLE
TEST_F(WallboxControllerTest, InitialStateIsIdle)
{
    EXPECT_EQ(controller->getCurrentState(), ChargingState::IDLE);
}

// Test: Starting charging enables relay
TEST_F(WallboxControllerTest, StartChargingEnablesRelay)
{
    controller->initialize();

    EXPECT_CALL(*gpio, digitalWrite(WallboxController::Pins::RELAY_ENABLE, PinValue::HIGH))
        .Times(1);

    controller->startCharging();
}

// Test: Stopping charging disables relay
TEST_F(WallboxControllerTest, StopChargingDisablesRelay)
{
    controller->initialize();
    controller->startCharging();

    EXPECT_CALL(*gpio, digitalWrite(WallboxController::Pins::RELAY_ENABLE, PinValue::LOW))
        .Times(1);

    controller->stopCharging();
}

// Test: Cannot start charging when disabled
TEST_F(WallboxControllerTest, CannotStartChargingWhenDisabled)
{
    controller->initialize();
    controller->disableWallbox();

    EXPECT_FALSE(controller->startCharging());
}

// Test: Disabling stops charging
TEST_F(WallboxControllerTest, DisablingStopsCharging)
{
    controller->initialize();
    controller->startCharging();

    EXPECT_TRUE(controller->isRelayEnabled());

    controller->disableWallbox();

    EXPECT_FALSE(controller->isRelayEnabled());
    EXPECT_EQ(controller->getCurrentState(), ChargingState::IDLE);
}

// Test: Shutdown disables relay
TEST_F(WallboxControllerTest, ShutdownDisablesRelay)
{
    controller->initialize();
    controller->startCharging();

    EXPECT_CALL(*gpio, digitalWrite(WallboxController::Pins::RELAY_ENABLE, PinValue::LOW))
        .Times(1);

    controller->shutdown();
}

// Test: Shutdown stops network
TEST_F(WallboxControllerTest, ShutdownStopsNetwork)
{
    controller->initialize();

    EXPECT_CALL(*network, stopReceiving()).Times(1);
    EXPECT_CALL(*network, disconnect()).Times(1);

    controller->shutdown();
}

// Main function for running tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>
#include "ChargingStateMachine.h"

/**
 * @brief Unit tests for ChargingStateMachine
 *
 * Demonstrates unit testing with Google Test framework.
 * Tests the State Pattern implementation.
 */
class ChargingStateMachineTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        stateMachine = std::make_unique<ChargingStateMachine>();
    }

    void TearDown() override
    {
        stateMachine.reset();
    }

    std::unique_ptr<ChargingStateMachine> stateMachine;
};

// Test: Initial state should be IDLE
TEST_F(ChargingStateMachineTest, InitialStateIsIdle)
{
    EXPECT_EQ(stateMachine->getCurrentState(), ChargingState::IDLE);
    EXPECT_TRUE(stateMachine->isIdle());
    EXPECT_FALSE(stateMachine->isCharging());
}

// Test: Can transition from IDLE to PREPARING
TEST_F(ChargingStateMachineTest, CanTransitionIdleToPreparing)
{
    EXPECT_TRUE(stateMachine->transitionTo(ChargingState::PREPARING));
    EXPECT_EQ(stateMachine->getCurrentState(), ChargingState::PREPARING);
}

// Test: Can start charging from IDLE
TEST_F(ChargingStateMachineTest, CanStartChargingFromIdle)
{
    EXPECT_TRUE(stateMachine->startCharging("Test"));
    EXPECT_TRUE(stateMachine->isCharging());
    EXPECT_EQ(stateMachine->getCurrentState(), ChargingState::CHARGING);
}

// Test: Cannot start charging from ERROR state
TEST_F(ChargingStateMachineTest, CannotStartChargingFromError)
{
    stateMachine->enterErrorState("Test error");
    EXPECT_FALSE(stateMachine->startCharging("Test"));
    EXPECT_TRUE(stateMachine->isError());
}

// Test: Can pause charging
TEST_F(ChargingStateMachineTest, CanPauseCharging)
{
    stateMachine->startCharging();
    EXPECT_TRUE(stateMachine->pauseCharging("Test pause"));
    EXPECT_TRUE(stateMachine->isPaused());
}

// Test: Can resume from paused
TEST_F(ChargingStateMachineTest, CanResumeFromPaused)
{
    stateMachine->startCharging();
    stateMachine->pauseCharging();
    EXPECT_TRUE(stateMachine->resumeCharging("Test resume"));
    EXPECT_TRUE(stateMachine->isCharging());
}

// Test: Can stop charging
TEST_F(ChargingStateMachineTest, CanStopCharging)
{
    stateMachine->startCharging();
    EXPECT_TRUE(stateMachine->stopCharging("Test stop"));
    EXPECT_TRUE(stateMachine->isIdle());
}

// Test: Invalid transition is rejected
TEST_F(ChargingStateMachineTest, InvalidTransitionRejected)
{
    // Cannot go directly from IDLE to FINISHING
    EXPECT_FALSE(stateMachine->transitionTo(ChargingState::FINISHING));
    EXPECT_TRUE(stateMachine->isIdle()); // Should remain in IDLE
}

// Test: Can check valid transitions
TEST_F(ChargingStateMachineTest, CanCheckValidTransitions)
{
    EXPECT_TRUE(stateMachine->canTransitionTo(ChargingState::PREPARING));
    EXPECT_FALSE(stateMachine->canTransitionTo(ChargingState::FINISHING));
}

// Test: State change observer is notified
TEST_F(ChargingStateMachineTest, ObserverNotified)
{
    bool notified = false;
    ChargingState oldState = ChargingState::IDLE;
    ChargingState newState = ChargingState::IDLE;

    stateMachine->addStateChangeListener(
        [&](ChargingState old, ChargingState new_, const std::string &)
        {
            notified = true;
            oldState = old;
            newState = new_;
        });

    stateMachine->transitionTo(ChargingState::PREPARING);

    EXPECT_TRUE(notified);
    EXPECT_EQ(oldState, ChargingState::IDLE);
    EXPECT_EQ(newState, ChargingState::PREPARING);
}

// Test: Can reset from error
TEST_F(ChargingStateMachineTest, CanResetFromError)
{
    stateMachine->enterErrorState("Test error");
    EXPECT_TRUE(stateMachine->isError());

    EXPECT_TRUE(stateMachine->reset());
    EXPECT_TRUE(stateMachine->isIdle());
}

// Test: State strings are correct
TEST_F(ChargingStateMachineTest, StateStringsCorrect)
{
    EXPECT_EQ(stateMachine->getStateString(), "IDLE");

    stateMachine->transitionTo(ChargingState::PREPARING);
    EXPECT_EQ(stateMachine->getStateString(), "PREPARING");

    stateMachine->transitionTo(ChargingState::CHARGING);
    EXPECT_EQ(stateMachine->getStateString(), "CHARGING");
}

// Test: Complete charging lifecycle
TEST_F(ChargingStateMachineTest, CompleteChargingLifecycle)
{
    // IDLE -> PREPARING -> CHARGING
    EXPECT_TRUE(stateMachine->startCharging());
    EXPECT_TRUE(stateMachine->isCharging());

    // CHARGING -> PAUSED
    EXPECT_TRUE(stateMachine->pauseCharging());
    EXPECT_TRUE(stateMachine->isPaused());

    // PAUSED -> CHARGING
    EXPECT_TRUE(stateMachine->resumeCharging());
    EXPECT_TRUE(stateMachine->isCharging());

    // CHARGING -> FINISHING -> IDLE
    EXPECT_TRUE(stateMachine->stopCharging());
    EXPECT_TRUE(stateMachine->isIdle());
}

// Main function for running tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

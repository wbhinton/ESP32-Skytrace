#include <unity.h>
#include "RangingLogic.h"

void test_master_ranging_success(void) {
    uint16_t irqStatus = IRQ_RANGING_MASTER_RESULT_VALID;
    float rawDistance = 12.5f;
    RangingResult result = RangingLogic::interpretMasterIrq(irqStatus, rawDistance);
    
    TEST_ASSERT_EQUAL(RANGING_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_FLOAT(12.5f, result.distance);
}

void test_master_ranging_timeout(void) {
    uint16_t irqStatus = IRQ_RX_TX_TIMEOUT;
    RangingResult result = RangingLogic::interpretMasterIrq(irqStatus, 0.0f);
    
    TEST_ASSERT_EQUAL(RANGING_TIMEOUT, result.status);
}

void test_slave_ranging_success(void) {
    uint16_t irqStatus = IRQ_RANGING_SLAVE_RESPONSE_DONE;
    RangingStatus status = RangingLogic::interpretSlaveIrq(irqStatus);
    
    TEST_ASSERT_EQUAL(RANGING_SUCCESS, status);
}

void test_slave_ranging_timeout(void) {
    uint16_t irqStatus = IRQ_RX_TX_TIMEOUT;
    RangingStatus status = RangingLogic::interpretSlaveIrq(irqStatus);
    
    TEST_ASSERT_EQUAL(RANGING_TIMEOUT, status);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_master_ranging_success);
    RUN_TEST(test_master_ranging_timeout);
    RUN_TEST(test_slave_ranging_success);
    RUN_TEST(test_slave_ranging_timeout);
    return UNITY_END();
}

#ifndef RANGINGLOGIC_H
#define RANGINGLOGIC_H

#include <stdint.h>

// Define IRQ flags if not already defined (as in native environment)
#ifndef IRQ_RANGING_MASTER_RESULT_VALID
#define IRQ_RANGING_MASTER_RESULT_VALID 0x0200
#endif
#ifndef IRQ_RANGING_SLAVE_RESPONSE_DONE
#define IRQ_RANGING_SLAVE_RESPONSE_DONE 0x0400
#endif
#ifndef IRQ_RX_TX_TIMEOUT
#define IRQ_RX_TX_TIMEOUT 0x4000
#endif

enum RangingStatus {
    RANGING_SUCCESS,
    RANGING_TIMEOUT,
    RANGING_ERROR
};

struct RangingResult {
    RangingStatus status;
    float distance;
};

class RangingLogic {
public:
    static RangingResult interpretMasterIrq(uint16_t irqStatus, float rawDistance) {
        if (irqStatus & IRQ_RANGING_MASTER_RESULT_VALID) {
            return {RANGING_SUCCESS, rawDistance};
        } else if (irqStatus & IRQ_RX_TX_TIMEOUT) {
            return {RANGING_TIMEOUT, 0.0f};
        }
        return {RANGING_ERROR, 0.0f};
    }

    static RangingStatus interpretSlaveIrq(uint16_t irqStatus) {
        if (irqStatus & IRQ_RANGING_SLAVE_RESPONSE_DONE) {
            return RANGING_SUCCESS;
        } else if (irqStatus & IRQ_RX_TX_TIMEOUT) {
            return RANGING_TIMEOUT;
        }
        return RANGING_ERROR;
    }
};

#endif

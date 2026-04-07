#include <unity.h>
#include "logic.h"

void test_calculate_checksum(void) {
    uint8_t data[] = {1, 2, 3, 4, 5};
    TEST_ASSERT_EQUAL_UINT16(15, calculate_checksum(data, sizeof(data)));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_calculate_checksum);
    return UNITY_END();
}

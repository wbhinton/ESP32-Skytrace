#ifndef LOGIC_H
#define LOGIC_H

#include <stdint.h>
#include <stddef.h>

uint16_t calculate_checksum(const uint8_t* data, size_t length);

#endif

#ifndef FLEXRAY_PARAMETERS_H
#define FLEXRAY_PARAMETERS_H

// Sim parameters
#define IDLE_PERIOD_LEN 100

// Analyser parameters
#define MIN_IDLE_LEN 9 // One more than 1 byte, so doesn't happen normally

// Cycles
#define NUM_CYCLES 64

// Special sequences
#define TSS_LEN 5

// CRC11
#define CRC11_POLYNOMIAL 0xB85
#define CRC11_INIT 0x01A

// CRC24
#define CRC24_POLYNOMIAL 0x5D6DCB
#define CRC24_INIT 0xFEDCBA

#endif

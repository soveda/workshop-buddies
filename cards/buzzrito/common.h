#pragma once

#include <stdint.h>

// The original Buzzrito engine renders 64-sample I2S blocks. For this first
// Workshop Computer scaffold we exercise that engine one stereo sample at a
// time from ComputerCard::ProcessSample().
#define SAMPLE_FREQ 48000
#define BLOCK_SIZE 1
#define BLOCK_SIZE_SH 0
#define BLOCKS_PER_SECOND (SAMPLE_FREQ / BLOCK_SIZE)

static inline int clampi(int x, int mn, int mx) { return (x < mn) ? mn : (x > mx) ? mx : x; }
static inline int maxi(int a, int b) { return (a > b) ? a : b; }
static inline int mini(int a, int b) { return (a < b) ? a : b; }
static inline int make_lpf_delta(int target, int current, int speed_shift)
{
    int delta = target - current;
    if (delta > 0) return -(-delta >> speed_shift);
    return delta >> speed_shift;
}

#define debug_log(...)


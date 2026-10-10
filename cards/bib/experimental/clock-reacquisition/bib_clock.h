#pragma once

#include <cstdint>

struct BibClockState
{
    uint32_t lastPulse1Block = 0;
    uint32_t clockPeriodSamples = 0;
    uint32_t pendingClockPeriod = 0;
    int clockedDelayTime = 16384;
    int clockHandoffControl = 0;
    bool clockSync = false;
    bool clockHandoff = false;
};

inline void UpdateBibClock(BibClockState &state, uint32_t blockClock,
                           bool risingEdge, int xControl)
{
    constexpr uint32_t kBlockFrames = 64;
    constexpr uint32_t kMinimumPeriod = 48000 / 20;
    constexpr uint32_t kMaximumPeriod = 48000 * 2;
    if (risingEdge)
    {
        if (state.lastPulse1Block != 0)
        {
            const uint32_t interval = (blockClock - state.lastPulse1Block) * kBlockFrames;
            if (interval >= kMinimumPeriod && interval < kMaximumPeriod)
            {
                const bool nearCurrentRate = !state.clockSync ||
                    (interval >= (state.clockPeriodSamples >> 1) &&
                     interval <= (state.clockPeriodSamples << 1));
                const uint32_t difference = interval > state.pendingClockPeriod ?
                    interval - state.pendingClockPeriod : state.pendingClockPeriod - interval;
                const uint32_t tolerance = state.pendingClockPeriod / 50 > kBlockFrames ?
                    state.pendingClockPeriod / 50 : kBlockFrames;
                // A large rate change needs two matching intervals. One extra
                // edge must not permanently prevent the clock from relocking.
                const bool confirmedNewRate = state.pendingClockPeriod != 0 && difference <= tolerance;
                if (nearCurrentRate || confirmedNewRate)
                {
                    state.clockPeriodSamples = interval;
                    state.clockSync = true;
                    state.clockHandoff = false;
                    state.pendingClockPeriod = 0;
                }
                else
                {
                    state.pendingClockPeriod = interval;
                }
            }
            else
            {
                state.pendingClockPeriod = 0;
            }
        }
        state.lastPulse1Block = blockClock;
    }

    if (state.clockSync && state.lastPulse1Block != 0 &&
        (blockClock - state.lastPulse1Block) * kBlockFrames > (state.clockPeriodSamples << 1))
    {
        state.clockSync = false;
        state.clockHandoff = true;
        state.clockHandoffControl = xControl;
        state.lastPulse1Block = 0;
        state.pendingClockPeriod = 0;
    }
}

inline int QuantiseDelayToClock(int target, uint32_t clockPeriod)
{
    uint64_t period = clockPeriod;
    for (int attempt = 0; attempt < 24 && period != 0; ++attempt)
    {
        const uint64_t dotted = (period * 3u) / 2u;
        const uint64_t below = dotted / 2u;
        if (below > static_cast<uint32_t>(target))
        {
            period >>= 1;
            continue;
        }
        if (dotted <= static_cast<uint32_t>(target))
        {
            period <<= 1;
            continue;
        }

        int closest = target;
        uint32_t distance = 0xffffffffu;
        const uint64_t candidates[] = {below, period, dotted};
        for (uint64_t candidate : candidates)
        {
            if (candidate < 8 || candidate >= 96u * 1024u) continue;
            const int value = static_cast<int>(candidate);
            const uint32_t difference = value > target ? value - target : target - value;
            if (difference < distance)
            {
                distance = difference;
                closest = value;
            }
        }
        return closest;
    }
    return target;
}

#include <cassert>
#include <cstdio>
#include "../bib_clock.h"

struct ClockTest
{
    BibClockState state;
    uint32_t block = 1;

    void EdgeAfter(uint32_t blocks)
    {
        for (uint32_t i = 0; i < blocks; ++i)
        {
            ++block;
            UpdateBibClock(state, block, i + 1 == blocks, 2048);
        }
    }

    void LockQuarterNotes()
    {
        UpdateBibClock(state, block, true, 2048);
        EdgeAfter(375); // 500 ms at 48 kHz / 64 frames.
        assert(state.clockSync && state.clockPeriodSamples == 24000);
    }
};

int main()
{
    ClockTest test;
    test.LockQuarterNotes();
    test.EdgeAfter(94);
    assert(test.state.clockPeriodSamples == 24000);
    test.EdgeAfter(94);
    assert(test.state.clockSync && test.state.clockPeriodSamples == 6016);
    assert(test.state.pendingClockPeriod == 0);

    // Slower clocks time out, hold the last delay and then relock.
    test.EdgeAfter(375);
    assert(!test.state.clockSync && test.state.clockHandoff);
    test.EdgeAfter(375);
    assert(test.state.clockSync && test.state.clockPeriodSamples == 24000);
    assert(!test.state.clockHandoff);

    // A single outlier does not change the rate; ordinary pulses clear it.
    test.EdgeAfter(94);
    assert(test.state.clockPeriodSamples == 24000);
    test.EdgeAfter(375);
    assert(test.state.clockPeriodSamples == 24000 && test.state.pendingClockPeriod == 0);

    // Quantised block timestamps can differ by one block between edges.
    test.EdgeAfter(94);
    test.EdgeAfter(93);
    assert(test.state.clockSync && test.state.clockPeriodSamples == 5952);

    ClockTest invalid;
    invalid.LockQuarterNotes();
    invalid.EdgeAfter(20); // Under 50 ms: not a valid clock period.
    invalid.EdgeAfter(20);
    assert(invalid.state.clockPeriodSamples == 24000);

    ClockTest slow;
    UpdateBibClock(slow.state, slow.block, true, 2048);
    slow.EdgeAfter(1500); // Two seconds is outside the accepted range.
    assert(!slow.state.clockSync);

    // Losing the clock holds its delay and remembers the X handoff point.
    ClockTest stopped;
    stopped.LockQuarterNotes();
    stopped.state.clockedDelayTime = 24000;
    for (uint32_t i = 0; i < 751; ++i)
        UpdateBibClock(stopped.state, ++stopped.block, false, 1234);
    assert(!stopped.state.clockSync && stopped.state.clockHandoff);
    assert(stopped.state.clockedDelayTime == 24000 && stopped.state.clockHandoffControl == 1234);
    stopped.EdgeAfter(94);
    stopped.EdgeAfter(94);
    assert(stopped.state.clockSync && !stopped.state.clockHandoff);

    // Exact 4:1 clock changes must select the same delay at every X target.
    // The search must also stay within the original delay range.
    for (int target = 8; target <= 96 * 1024; ++target)
    {
        const int quarter = QuantiseDelayToClock(target, 24000);
        assert(quarter == QuantiseDelayToClock(target, 6000));
        assert(quarter >= 8 && quarter <= 96 * 1024);
    }
    const int quarter = QuantiseDelayToClock(24576, 24000);
    const int sixteenth = QuantiseDelayToClock(24576, 6016);
    assert(quarter == 24000 && sixteenth == 24064);
    std::puts("Clock tests passed: 4:1 changes, jitter, outliers, timeout, restart and delay divisions.");
}

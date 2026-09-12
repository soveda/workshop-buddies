// Bib for Workshop Computer
//
// A compact, Bib-inspired stereo dub processor.  The original Bib uses a
// capacitive slider and a considerably larger block-DSP engine.  This card
// translates its playable ideas to the Workshop Computer's three pots and
// momentary Z switch, while keeping every audio operation inexpensive enough
// for ComputerCard's 48 kHz per-sample interrupt.

#include <cstdint>
#include <cstdlib>

#include "ComputerCard.h"
#include "hardware/clocks.h"

// Directly adapted from the MIT-licensed Buddies Bib firmware.  It retains
// Bib's modulated Dattorro/Griesinger tank, damping, limiter and shimmer.
#include "bib_reverb.h"

namespace
{
constexpr int32_t kFull = 4095;
constexpr uint32_t kDelaySize = 32768; // power of two: 683 ms at 48 kHz
constexpr uint32_t kDelayMask = kDelaySize - 1;
constexpr uint32_t kDelayPositionMask = (kDelaySize << 8) - 1;
// Bib accepts clocks from 50 ms to 2 s. The delay buffer is shorter than the
// original's tape-scaled maximum, but its quantiser can still use divisions of
// a clock period longer than the physical buffer.
constexpr uint32_t kMinClockPeriod = 2400;
constexpr uint32_t kMaxClockPeriod = 96000;

static int16_t delayLeft[kDelaySize] = {};
static int16_t delayRight[kDelaySize] = {};

static inline int32_t ClampAudio(int32_t value)
{
    if (value < -2048) return -2048;
    if (value > 2047) return 2047;
    return value;
}

static inline int32_t Abs(int32_t value) { return value < 0 ? -value : value; }

// The delay's 16-bit storage has only 12-bit audio headroom once input and
// feedback are summed.  A soft knee avoids the brittle hard clipping that is
// especially obvious when fast notes overlap several repeats.
static inline int32_t DelaySoftLimit(int32_t value)
{
    const int32_t sign = value < 0 ? -1 : 1;
    int32_t magnitude = value < 0 ? -value : value;
    constexpr int32_t kKnee = 1536;
    if (magnitude > kKnee) {
        magnitude = kKnee + ((magnitude - kKnee) >> 3);
        if (magnitude > 2047) magnitude = 2047;
    }
    return sign * magnitude;
}

// Each page remembers its two parameter positions.  When a page is entered,
// its controls wait until the physical pot reaches the saved position before
// taking over.  This is the same "pickup" behaviour as the original Bib's
// catch-up LEDs, translated to a panel with only six LEDs.
class PagePickup
{
public:
    void Select(uint8_t page, bool alternate)
    {
        if (page == selected_ && alternate == alternate_) return;
        selected_ = page;
        alternate_ = alternate;
        caught_[0] = false;
        caught_[1] = false;
    }

    int32_t Update(uint8_t control, int32_t raw)
    {
        int32_t &saved = values_[alternate_ ? 1 : 0][selected_][control];
        if (!caught_[control]) {
            constexpr int32_t kCatchWindow = 64;
            if (raw >= saved - kCatchWindow && raw <= saved + kCatchWindow) {
                caught_[control] = true;
            } else {
                return saved;
            }
        }
        saved = raw;
        return saved;
    }

private:
    // Defaults match the sound heard immediately after boot.
    int32_t values_[2][4][2] = {
        { // Middle: the four main sound pages.
            {2048, 2048}, // drive, delay send
            {1004, 2560}, // delay time, feedback
            {1500, 1966}, // reverb send, decay
            {2048, 2731}, // mix, output level
        },
        { // Up: Delay tape and Reverb shimmer detail pages.
            {2048, 2048},
            {2048,    0}, // normal transport speed, no wobble
            {1024, 1966}, // original Bib's default shimmer, reserved Y
            {2048, 2731},
        },
    };
    uint8_t selected_ = 255;
    bool alternate_ = false;
    bool caught_[2] = {false, false};
};
} // namespace

class Bib : public ComputerCard
{
public:
    Bib() { EnableNormalisationProbe(); }

    void ProcessSample() override
    {
        // Main stands in for Bib's slider: divide its travel into four clear
        // regions.  The LEDs show the selected region continuously.
        const int32_t main = KnobVal(Knob::Main);
        const uint8_t mode = static_cast<uint8_t>((main * 4) >> 12);
        // Up is a latching secondary layer: Delay becomes tape transport and
        // Reverb becomes the persistent shimmer-setting page. The other two
        // pages retain their main controls until their own Up functions are
        // designed.
        const bool detailPage = SwitchVal() == Switch::Up && (mode == 1 || mode == 2);
        const bool tapePage = detailPage && mode == 1;
        const bool reverbPage = detailPage && mode == 2;
        pickup_.Select(mode, detailPage);
        const int32_t x = pickup_.Update(0, KnobVal(Knob::X));
        const int32_t y = pickup_.Update(1, KnobVal(Knob::Y));
        const bool pressed = SwitchVal() == Switch::Down;

        UpdateControls(mode, x, y, pressed, tapePage, reverbPage);
        UpdateLeds(mode);

        int32_t inL = AudioIn1();
        // A mono source patched into Audio 1 stays centred when Audio 2 is
        // unpatched.  The normalisation probe makes this musical default
        // possible without guessing from signal level.
        int32_t inR = Connected(Input::Audio2) ? AudioIn2() : inL;

        const int32_t drivenL = Shape(inL, drive_);
        const int32_t drivenR = Shape(inR, drive_);

        // This is the original Bib delay's Q8 tape position scheme.  It
        // permits continuously moving read heads when the tape transport is
        // slowed, sped up or wobbled, rather than jumping between samples.
        // Bib smooths its delay-time control before it reaches the read
        // heads. At the per-sample Workshop rate, a 1/128 slew preserves
        // deliberate tape-style pitch movement while removing stepped jumps.
        const int32_t delayTargetQ8 = static_cast<int32_t>(delaySamples_ << 8);
        delayTimeQ8_ += (delayTargetQ8 - delayTimeQ8_) >> 7;
        const uint32_t delayTimeQ8 = static_cast<uint32_t>(delayTimeQ8_);
        const uint32_t readL = (delayPositionQ8_ - delayTimeQ8) & kDelayPositionMask;
        // Bib's negative delay-send side uses a different right-hand delay
        // length.  That asymmetry turns the usual stereo repeat into the
        // distinct, moving ping-pong character of the original card.
        const uint32_t rightTimeQ8 = pingPong_ ? ((delayTimeQ8 * 3) >> 2) : delayTimeQ8;
        const uint32_t readR = (delayPositionQ8_ - rightTimeQ8) & kDelayPositionMask;
        const int32_t delayedL = ReadDelay(delayLeft, readL);
        const int32_t delayedR = ReadDelay(delayRight, readR);

        // Freeze stops new material entering, but the feedback path remains
        // alive.  It is the familiar "hold the dub" gesture from Bib.
        const int32_t inputSend = freeze_ ? 0 : delaySend_;
        // Freeze preserves the material already circulating in the delay by
        // raising its feedback as it closes the input.  Simply muting input
        // made the previous version's wet signal disappear at ordinary
        // feedback settings, which is not a useful dub freeze.
        const int32_t writeFeedback = freeze_ ? 3900 : delayFeedback_;
        // Directly preserve Bib's 144-degree feedback rotation. Ping-pong
        // remains the original card's asymmetric right-hand read time, not a
        // simple channel swap, so the repeat image keeps moving in stereo.
        constexpr int32_t kFeedbackCosQ12 = -3314;
        constexpr int32_t kFeedbackSinQ12 = 2408;
        const int32_t feedbackCos = (kFeedbackCosQ12 * writeFeedback) >> 12;
        const int32_t feedbackSin = (kFeedbackSinQ12 * writeFeedback) >> 12;
        const int32_t feedbackL = ((delayedL * feedbackCos) - (delayedR * feedbackSin)) >> 12;
        const int32_t feedbackR = ((delayedL * feedbackSin) + (delayedR * feedbackCos)) >> 12;
        int32_t writeL = ((drivenL * inputSend) >> 12) + feedbackL;
        int32_t writeR = ((drivenR * inputSend) >> 12) + feedbackR;
        // The original delay writer removes accumulated DC before recording
        // to tape. This prevents a long feedback run from drifting the
        // fractional writer toward one polarity.
        delayDcL_ += ((writeL << 8) - delayDcL_) >> 11;
        delayDcR_ += ((writeR << 8) - delayDcR_) >> 11;
        writeL -= delayDcL_ >> 8;
        writeR -= delayDcR_ >> 8;
        const int16_t limitedWriteL = static_cast<int16_t>(DelaySoftLimit(writeL));
        const int16_t limitedWriteR = static_cast<int16_t>(DelaySoftLimit(writeR));
        WriteDelay(limitedWriteL, limitedWriteR, TapeSpeedQ8());

        int32_t reverbL = 0;
        int32_t reverbR = 0;
        OriginalBibReverb(drivenL + delayedL, drivenR + delayedR,
                          reverbSend_, reverbFeedback_, shimmerAmount_, reverbL, reverbR);
        const int32_t wetL = delayedL + reverbL;
        const int32_t wetR = delayedR + reverbR;

        // Wet/dry is a true crossfade, then output level is applied last so
        // changing the mix does not create a sudden gain jump.
        const int32_t outL = (((drivenL * (kFull - mix_)) + (wetL * mix_)) >> 12);
        const int32_t outR = (((drivenR * (kFull - mix_)) + (wetR * mix_)) >> 12);
        AudioOut1(ClampAudio((outL * outputLevel_) >> 12));
        AudioOut2(ClampAudio((outR * outputLevel_) >> 12));
    }

private:
    // The accumulated values are the original Bib writer's anti-gap method:
    // at fractional tape speeds, a tape cell receives the time-weighted
    // contribution of all input samples that passed beneath the write head.
    uint32_t delayPositionQ8_ = 0;
    int32_t delayWriteAccumL_ = 0;
    int32_t delayWriteAccumR_ = 0;
    int32_t delayTimeQ8_ = 8192 << 8;
    int32_t delayDcL_ = 0;
    int32_t delayDcR_ = 0;
    uint32_t tapCounter_ = 0;
    uint32_t samplesSinceTap_ = 0;
    uint32_t delaySamples_ = 8192;
    uint32_t delayTargetSamples_ = 8192;
    int32_t drive_ = 2048;
    int32_t delaySend_ = 2048;
    int32_t delayFeedback_ = 2500;
    int32_t reverbSend_ = 1500;
    int32_t reverbFeedback_ = 2600;
    int32_t mix_ = 2048;
    int32_t outputLevel_ = 3072;
    bool wavefold_ = false;
    bool freeze_ = false;
    // Original Bib starts with a subtle shimmer setting and changes it only
    // when the player makes another deliberate pressure gesture.
    int32_t shimmerAmount_ = 1024;
    bool pingPong_ = false;
    PagePickup pickup_;
    uint32_t transportQ16_ = 65536;
    int32_t tapeSpeedSmoothQ16_ = 65536;
    uint16_t wowPhase_ = 0;
    int32_t wobbleDepth_ = 0;
    bool tapTimeActive_ = false;
    int32_t tapTimeKnob_ = 0;
    uint32_t ledPhase_ = 0;
    uint32_t sampleCounter_ = 0;
    uint32_t lastClockSample_ = 0;
    uint32_t clockPeriod_ = 0;
    bool clockSync_ = false;
    bool clockSuppressed_ = false;
    bool clockHandoff_ = false;
    int32_t reverbPendingL_ = 0;
    int32_t reverbPendingR_ = 0;
    int32_t reverbPreviousL_ = 0;
    int32_t reverbPreviousR_ = 0;
    int32_t reverbCurrentL_ = 0;
    int32_t reverbCurrentR_ = 0;
    bool reverbOddSample_ = false;

    void UpdateControls(uint8_t mode, int32_t x, int32_t y, bool pressed,
                        bool tapePage, bool reverbPage)
    {
        UpdateClock();
        // Count in samples so a pair of Z presses in delay mode becomes a
        // reliable, tempo-like tap time without timers or floating point.
        if (samplesSinceTap_ < kDelaySize - 1) ++samplesSinceTap_;
        const bool newPress = pressed && !wasPressed_;
        wasPressed_ = pressed;

        freeze_ = false;
        transportQ16_ = 65536;
        wobbleDepth_ = 0;

        if (tapePage) {
            // X is a direct transport control: 12 o'clock is ordinary tape
            // speed, CCW slows to a complete stop, CW reaches double speed.
            // Y adds slow wow/flutter around that chosen transport speed.
            transportQ16_ = static_cast<uint32_t>(x) << 5;
            // Full CW is deliberately restrained to a ±25% speed swing.
            wobbleDepth_ = (y * 1024) >> 12;
            return;
        }

        if (reverbPage) {
            // The original spider's hold pressure set a persistent shimmer
            // amount. X is its direct Workshop equivalent; switching back to
            // Middle leaves the chosen amount in place. Y is reserved for a
            // future original-style reverb extension.
            shimmerAmount_ = x;
            return;
        }

        switch (mode) {
        case 0: {
            drive_ = x;
            // The original delay-send pot is bipolar: its centre is off,
            // positive travel feeds normal stereo delay, and negative travel
            // selects a 3:4 ping-pong relationship between the channels.
            const int32_t send = y - 2048;
            pingPong_ = send < 0;
            const int32_t magnitude = Abs(send);
            delaySend_ = (magnitude * magnitude) >> 10;
            if (newPress) wavefold_ = !wavefold_;
            break;
        }
        case 1:
            // When a patched clock disappears, retain the last clocked time
            // until X is deliberately moved.  Jumping straight back to the
            // physical X position was audible as a short, low "blurp".
            if (clockHandoff_) {
                tapTimeActive_ = true;
                tapTimeKnob_ = x;
                clockHandoff_ = false;
            }
            // 4.3 ms to 341 ms: long enough for slap, echo and short loops.
            // A tapped time stays active until X is deliberately moved.
            if (!tapTimeActive_ || Abs(x - tapTimeKnob_) > 512) {
                tapTimeActive_ = false;
                delayTargetSamples_ = 208 + static_cast<uint32_t>((x * (kDelaySize - 209)) >> 12);
            }
            // Leave enough feedback for long repeats, but below the hard
            // clipping loop this compact delay otherwise reaches at maximum.
            delayFeedback_ = (y * 3400) >> 12;
            if (newPress) {
                if (tapCounter_ != 0 && samplesSinceTap_ > 240) {
                    delayTargetSamples_ = samplesSinceTap_;
                    tapTimeActive_ = true;
                    tapTimeKnob_ = x;
                }
                // As on original Bib, a manual tap takes priority over a
                // remembered external clock until that clock disappears.
                clockSync_ = false;
                clockSuppressed_ = true;
                samplesSinceTap_ = 0;
                tapCounter_ = 1;
            }
            delaySamples_ = clockSync_ ? QuantiseToClock(delayTargetSamples_) : delayTargetSamples_;
            break;
        case 2:
            reverbSend_ = x;
            reverbFeedback_ = 1400 + ((y * 1800) >> 12);
            break;
        default:
            mix_ = x;
            outputLevel_ = 1024 + ((y * 3071) >> 12);
            freeze_ = pressed;
            break;
        }
    }

    void UpdateClock()
    {
        ++sampleCounter_;
        if (PulseIn1RisingEdge()) {
            if (lastClockSample_ != 0) {
                const uint32_t interval = sampleCounter_ - lastClockSample_;
                // Match Bib's valid clock range: 50 ms to 2 s. Its original
                // CV input additionally checked pulse width; ComputerCard's
                // digital Pulse In already supplies a debounced edge.
                // A cable can generate a short spurious edge as it is
                // removed. Do not let that single edge replace an already
                // established tempo with an implausibly different period.
                const bool plausible = !clockSync_ ||
                    (interval >= (clockPeriod_ >> 1) && interval <= (clockPeriod_ << 1));
                if (interval >= kMinClockPeriod && interval < kMaxClockPeriod &&
                    plausible && !clockSuppressed_) {
                    clockPeriod_ = interval;
                    clockSync_ = true;
                }
            }
            lastClockSample_ = sampleCounter_;
        }

        // On clock loss, hold the final synchronised time rather than
        // reverting to X mid-repeat. This makes disconnecting a clock a
        // transparent handoff to the manual/tap time control.
        if (clockSync_ && lastClockSample_ != 0 &&
            sampleCounter_ - lastClockSample_ > (clockPeriod_ << 1)) {
            clockSync_ = false;
            delayTargetSamples_ = delaySamples_;
            clockHandoff_ = true;
            lastClockSample_ = 0;
        }

        // Unplugging/stopping a clock arms automatic detection again.  Until
        // then a Z tap deliberately keeps its manually tapped tempo.
        if (clockSuppressed_ && sampleCounter_ - lastClockSample_ > kDelaySize) {
            clockSuppressed_ = false;
            lastClockSample_ = 0;
        }
    }

    uint32_t QuantiseToClock(uint32_t target) const
    {
        // Direct adaptation of Bib's update_delay_time(). It repeatedly
        // shifts the measured period by octaves until X's requested time lies
        // between 3/4 and 3/2 of that period, then chooses the nearest of
        // 3/4, straight, or dotted. This is why the original delay feels
        // rhythmically quantised without taking X away from the player.
        uint64_t period = clockPeriod_;
        for (int attempt = 0; attempt < 24 && period != 0; ++attempt) {
            const uint64_t dotted = (period * 3u) / 2u;
            const uint64_t below = dotted / 2u;
            if (below > target) {
                period >>= 1;
                continue;
            }
            if (dotted <= target) {
                period <<= 1;
                continue;
            }

            uint32_t closest = target;
            uint32_t distance = 0xffffffffu;
            const uint64_t candidates[] = {below, period, dotted};
            for (uint64_t candidate : candidates) {
                // The Workshop's fixed 32k tape cannot read a longer delay,
                // so reject only out-of-buffer options; neighbouring octave
                // divisions remain valid just as on the original Bib.
                if (candidate < 208 || candidate >= kDelaySize) continue;
                const uint32_t value = static_cast<uint32_t>(candidate);
                const uint32_t difference = value > target ? value - target : target - value;
                if (difference < distance) {
                    distance = difference;
                    closest = value;
                }
            }
            return closest;
        }
        return target;
    }

    int32_t ReadDelay(const int16_t *buffer, uint32_t positionQ8) const
    {
        const uint32_t index = (positionQ8 >> 8) & kDelayMask;
        const int32_t first = buffer[index];
        const int32_t next = buffer[(index + 1) & kDelayMask];
        const int32_t fraction = positionQ8 & 255u;
        return first + (((next - first) * fraction) >> 8);
    }

    void WriteDelay(int16_t left, int16_t right, uint32_t speedQ8)
    {
        // Directly adapted from Bib's fractional tape writer.  Keep the
        // unwrapped new position until the end so a write that crosses the
        // ring boundary distributes correctly into each crossed tape cell.
        uint32_t oldPosition = delayPositionQ8_;
        uint32_t oldIndex = (oldPosition >> 8) & kDelayMask;
        const uint32_t newPosition = oldPosition + speedQ8;
        const uint32_t finalIndex = (newPosition >> 8) & kDelayMask;

        while (oldIndex != finalIndex) {
            const uint32_t amount = 256u - (oldPosition & 255u);
            delayWriteAccumL_ += static_cast<int32_t>(amount) * left;
            delayWriteAccumR_ += static_cast<int32_t>(amount) * right;
            delayLeft[oldIndex] = static_cast<int16_t>(DelaySoftLimit(delayWriteAccumL_ >> 8));
            delayRight[oldIndex] = static_cast<int16_t>(DelaySoftLimit(delayWriteAccumR_ >> 8));
            delayWriteAccumL_ = 0;
            delayWriteAccumR_ = 0;
            oldPosition += amount;
            oldIndex = (oldIndex + 1) & kDelayMask;
        }

        const uint32_t amount = newPosition - oldPosition;
        delayWriteAccumL_ += static_cast<int32_t>(amount) * left;
        delayWriteAccumR_ += static_cast<int32_t>(amount) * right;
        delayPositionQ8_ = newPosition & kDelayPositionMask;
    }

    uint32_t TapeSpeedQ8()
    {
        // A triangle LFO is enough to make a controllable tape wobble.  Its
        // 1.5 Hz rate is deliberately slow: it feels like a moving tape reel
        // rather than a conventional vibrato oscillator.
        wowPhase_ = static_cast<uint16_t>(wowPhase_ + 2);
        int32_t triangle = wowPhase_ < 32768 ? wowPhase_ : 65535 - wowPhase_;
        triangle = (triangle << 1) - 32768; // signed Q15, -32768..32766

        int32_t speed = static_cast<int32_t>(transportQ16_);
        // Split the Q12 × Q15 modulation before multiplying by transport so
        // this remains safely within fast 32-bit RP2040 arithmetic.
        const int32_t wobble = (wobbleDepth_ * triangle) >> 13;
        speed += (speed * wobble) >> 14;
        if (speed < 0) speed = 0;
        if (speed > 131072) speed = 131072; // do not exceed 2x transport

        // The original Bib smooths tape transport before the Q8 writer. The
        // same one-pole response removes control zippering without making a
        // deliberate stop or speed gesture feel sluggish.
        tapeSpeedSmoothQ16_ += (speed - tapeSpeedSmoothQ16_) >> 7;
        if (tapeSpeedSmoothQ16_ < 0) tapeSpeedSmoothQ16_ = 0;
        return static_cast<uint32_t>(tapeSpeedSmoothQ16_) >> 8;
    }

    void UpdateLeds(uint8_t mode)
    {
        // LEDs 0 and 1 form a small binary page display.  This leaves the
        // other four LEDs free to show the useful, persistent effect levels:
        //
        //   mode 0: 0 off, 1 off       mode 2: 0 off, 1 on
        //   mode 1: 0 on,  1 off       mode 3: 0 on,  1 on
        LedOn(0, (mode & 1u) != 0);
        LedOn(1, (mode & 2u) != 0);

        // The remaining LEDs describe the sound, rather than whichever page
        // happens to be selected: drive, delay feedback, reverb decay, then
        // wet mix.  Their levels remain visible while adjusting any page.
        ++ledPhase_;
        if (wavefold_) {
            // LED 2 keeps showing drive level, but its gentle breathing makes
            // wavefold visibly distinct from the steady overdrive state.
            const uint32_t phase = (ledPhase_ >> 8) & 255u;
            const int32_t triangle = phase < 128 ? phase : 255 - phase;
            const int32_t brightness = (drive_ * (2048 + (triangle << 4))) >> 12;
            LedBrightness(2, static_cast<uint16_t>(brightness));
        } else {
            LedBrightness(2, static_cast<uint16_t>(drive_));
        }
        LedBrightness(3, static_cast<uint16_t>(delayFeedback_));
        LedBrightness(4, static_cast<uint16_t>(reverbFeedback_));
        LedBrightness(5, static_cast<uint16_t>(mix_));
    }

    int32_t Shape(int32_t input, int32_t drive) const
    {
        const int32_t amplified = (input * (1024 + drive)) >> 11;
        if (!wavefold_) return ClampAudio(amplified);

        // Triangle folding is a cheap, intentionally rough alternative to
        // overdrive; it retains Bib's two distinct drive colours.
        int32_t folded = (amplified + 2048) & 8191;
        if (folded > 4095) folded = 8191 - folded;
        return folded - 2048;
    }

    void OriginalBibReverb(int32_t inputL, int32_t inputR, int32_t send,
                           int32_t feedback, int32_t shimmerAmount,
                           int32_t &outL, int32_t &outR)
    {
        // Original Bib runs this reverb once per two stereo samples.  Buffer
        // one sample here, then interpolate its output back to 48 kHz.
        if (!reverbOddSample_) {
            reverbPendingL_ = inputL;
            reverbPendingR_ = inputR;
            reverbOddSample_ = true;
            outL = (reverbPreviousL_ + reverbCurrentL_) >> 1;
            outR = (reverbPreviousR_ + reverbCurrentR_) >> 1;
            return;
        }

        reverbOddSample_ = false;
        const int32_t sourceL = (reverbPendingL_ + inputL) >> 1;
        const int32_t sourceR = (reverbPendingR_ + inputR) >> 1;
        int32_t taperedSend = (send * send) >> 13;
        // Preserve the original quadratic send response through most of the
        // control range, but soften its final extreme. At maximum this tank
        // otherwise receives enough dense input to crackle on transients.
        if (taperedSend > 1400) taperedSend = 1400 + ((taperedSend - 1400) >> 3);
        const int32_t inputScaleL = ((sourceL << 4) * taperedSend) >> 12;
        const int32_t inputScaleR = ((sourceR << 4) * taperedSend) >> 12;

        // Keep Bib's nonlinear decay mapping, rather than treating the knob
        // as a linear feedback coefficient.
        int32_t decay = 4096 - feedback;
        decay = (decay * decay) >> 12;
        decay = (decay * decay) >> 12;
        decay = 4096 - decay;
        // Bib passes twice its stored 0..4096 pressure-derived shimmer value
        // into the tank. Preserve that gain relationship for the Workshop
        // Up-page control, including the original feedback safety scaling.
        shimmer_am_q12 = ((shimmerAmount * 2 * 800) / (feedback + 1024));

        int wetL = 0;
        int wetR = 0;
        do_reverb(inputScaleL, inputScaleR, decay, &wetL, &wetR);
        reverbPreviousL_ = reverbCurrentL_;
        reverbPreviousR_ = reverbCurrentR_;
        reverbCurrentL_ = wetL >> 4;
        reverbCurrentR_ = wetR >> 4;
        outL = reverbCurrentL_;
        outR = reverbCurrentR_;
    }

    bool wasPressed_ = false;
};

int main()
{
    // The original Bib runs its RP2040 at 200 MHz.  192 MHz is a proven
    // Workshop Computer speed, gives this denser DSP more headroom, and is
    // an alias-safe multiple for ComputerCard v0.3.x's CV PWM timing.
    set_sys_clock_khz(192000, true);
    Bib bib;
    bib.Run();
}

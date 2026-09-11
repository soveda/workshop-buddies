// Bib for Workshop Computer
//
// A compact, Bib-inspired stereo dub processor.  The original Bib uses a
// capacitive slider and a considerably larger block-DSP engine.  This card
// translates its playable ideas to the Workshop Computer's three pots and
// momentary Z switch, while keeping every audio operation inexpensive enough
// for ComputerCard's 48 kHz per-sample interrupt.

#include <cstdint>

#include "ComputerCard.h"

namespace
{
constexpr int32_t kFull = 4095;
constexpr uint32_t kDelaySize = 16384; // power of two: 341 ms at 48 kHz
constexpr uint32_t kDelayMask = kDelaySize - 1;

// These are deliberately modest.  Together with the main delay they leave
// ample RAM for a copy-to-RAM build on the RP2040.
constexpr uint16_t kCombA = 1429;
constexpr uint16_t kCombB = 2083;
constexpr uint16_t kDiffuser = 541;

static int16_t delayLeft[kDelaySize] = {};
static int16_t delayRight[kDelaySize] = {};
static int16_t combA[kCombA] = {};
static int16_t combB[kCombB] = {};
static int16_t diffuser[kDiffuser] = {};

static inline int32_t ClampAudio(int32_t value)
{
    if (value < -2048) return -2048;
    if (value > 2047) return 2047;
    return value;
}

static inline int32_t Abs(int32_t value) { return value < 0 ? -value : value; }
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
        const int32_t x = KnobVal(Knob::X);
        const int32_t y = KnobVal(Knob::Y);
        const bool pressed = SwitchVal() == Switch::Down;

        UpdateControls(mode, x, y, pressed);
        UpdateLeds(mode, pressed);

        int32_t inL = AudioIn1();
        // A mono source patched into Audio 1 stays centred when Audio 2 is
        // unpatched.  The normalisation probe makes this musical default
        // possible without guessing from signal level.
        int32_t inR = Connected(Input::Audio2) ? AudioIn2() : inL;

        const int32_t drivenL = Shape(inL, drive_);
        const int32_t drivenR = Shape(inR, drive_);

        const uint32_t read = (write_ - delaySamples_) & kDelayMask;
        const int32_t delayedL = delayLeft[read];
        const int32_t delayedR = delayRight[read];

        // Freeze stops new material entering, but the feedback path remains
        // alive.  It is the familiar "hold the dub" gesture from Bib.
        const int32_t inputSend = freeze_ ? 0 : delaySend_;
        delayLeft[write_] = static_cast<int16_t>(ClampAudio(
            ((drivenL * inputSend) + (delayedR * delayFeedback_)) >> 12));
        delayRight[write_] = static_cast<int16_t>(ClampAudio(
            ((drivenR * inputSend) + (delayedL * delayFeedback_)) >> 12));
        write_ = (write_ + 1) & kDelayMask;

        const int32_t reverbIn = ((delayedL + delayedR) * reverbSend_) >> 13;
        const int32_t reverb = Reverb(reverbIn, reverbFeedback_, shimmer_);
        const int32_t wetL = delayedL + reverb;
        const int32_t wetR = delayedR + reverb;

        // Wet/dry is a true crossfade, then output level is applied last so
        // changing the mix does not create a sudden gain jump.
        const int32_t outL = (((drivenL * (kFull - mix_)) + (wetL * mix_)) >> 12);
        const int32_t outR = (((drivenR * (kFull - mix_)) + (wetR * mix_)) >> 12);
        AudioOut1(ClampAudio((outL * outputLevel_) >> 12));
        AudioOut2(ClampAudio((outR * outputLevel_) >> 12));
    }

private:
    uint32_t write_ = 0;
    uint32_t tapCounter_ = 0;
    uint32_t samplesSinceTap_ = 0;
    uint32_t delaySamples_ = 8192;
    int32_t drive_ = 2048;
    int32_t delaySend_ = 2048;
    int32_t delayFeedback_ = 2500;
    int32_t reverbSend_ = 1500;
    int32_t reverbFeedback_ = 2600;
    int32_t mix_ = 2048;
    int32_t outputLevel_ = 3072;
    bool wavefold_ = false;
    bool freeze_ = false;
    bool shimmer_ = false;
    uint16_t combAPos_ = 0;
    uint16_t combBPos_ = 0;
    uint16_t diffuserPos_ = 0;

    void UpdateControls(uint8_t mode, int32_t x, int32_t y, bool pressed)
    {
        // Count in samples so a pair of Z presses in delay mode becomes a
        // reliable, tempo-like tap time without timers or floating point.
        if (samplesSinceTap_ < kDelaySize - 1) ++samplesSinceTap_;
        const bool newPress = pressed && !wasPressed_;
        wasPressed_ = pressed;

        freeze_ = false;
        shimmer_ = false;
        switch (mode) {
        case 0:
            drive_ = x;
            delaySend_ = y;
            if (newPress) wavefold_ = !wavefold_;
            break;
        case 1:
            // 4.3 ms to 341 ms: long enough for slap, echo and short loops.
            delaySamples_ = 208 + static_cast<uint32_t>((x * (kDelaySize - 209)) >> 12);
            delayFeedback_ = (y * 4000) >> 12; // always below runaway
            if (newPress) {
                if (tapCounter_ != 0 && samplesSinceTap_ > 240) {
                    delaySamples_ = samplesSinceTap_;
                }
                samplesSinceTap_ = 0;
                tapCounter_ = 1;
            }
            break;
        case 2:
            reverbSend_ = x;
            reverbFeedback_ = 1400 + ((y * 2500) >> 12);
            shimmer_ = pressed;
            break;
        default:
            mix_ = x;
            outputLevel_ = 1024 + ((y * 3071) >> 12);
            freeze_ = pressed;
            break;
        }
    }

    void UpdateLeds(uint8_t mode, bool pressed)
    {
        // Two LEDs per mode: one steady mode marker and one that brightens
        // with input level.  It remains readable in a dark performance.
        for (uint32_t i = 0; i < 6; ++i) LedOff(i);
        const uint32_t first = mode < 3 ? mode * 2 : 4;
        LedOn(first, true);
        const int32_t meter = Abs(AudioIn1()) * 2;
        LedBrightness(first + 1, static_cast<uint16_t>(meter > 4095 ? 4095 : meter));
        if (pressed) LedOn(5, true);
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

    int32_t Reverb(int32_t input, int32_t feedback, bool shimmer)
    {
        const int32_t a = combA[combAPos_];
        const int32_t b = combB[combBPos_];
        // Shimmer here is deliberately a bright feedback lift, not a faux
        // pitch shifter.  It is stable, light on CPU, and clearly labelled in
        // the documentation as a colour rather than an octave effect.
        const int32_t colour = shimmer ? ((a - b) >> 2) : 0;
        combA[combAPos_] = static_cast<int16_t>(ClampAudio(input + ((a * feedback) >> 12) + colour));
        combB[combBPos_] = static_cast<int16_t>(ClampAudio(input + ((b * feedback) >> 12) - colour));
        if (++combAPos_ == kCombA) combAPos_ = 0;
        if (++combBPos_ == kCombB) combBPos_ = 0;

        const int32_t tank = (a + b) >> 1;
        const int32_t delayed = diffuser[diffuserPos_];
        diffuser[diffuserPos_] = static_cast<int16_t>(ClampAudio(tank + ((delayed * 2300) >> 12)));
        if (++diffuserPos_ == kDiffuser) diffuserPos_ = 0;
        return delayed - ((tank * 2300) >> 12);
    }

    bool wasPressed_ = false;
};

int main()
{
    Bib bib;
    bib.Run();
}

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
constexpr uint32_t kDelaySize = 32768; // power of two: 683 ms at 48 kHz
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
        { // Up: currently used by the Delay tape page.
            {2048, 2048},
            {2048,    0}, // normal transport speed, no wobble
            {1500, 1966},
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
        // Up is a latching secondary layer.  At present it belongs to the
        // delay page, where it becomes a tape-transport page.  The other
        // pages retain their main controls until their own Up functions are
        // designed.
        const bool tapePage = SwitchVal() == Switch::Up && mode == 1;
        pickup_.Select(mode, tapePage);
        const int32_t x = pickup_.Update(0, KnobVal(Knob::X));
        const int32_t y = pickup_.Update(1, KnobVal(Knob::Y));
        const bool pressed = SwitchVal() == Switch::Down;

        UpdateControls(mode, x, y, pressed, tapePage);
        UpdateLeds(mode);

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
        // Freeze preserves the material already circulating in the delay by
        // raising its feedback as it closes the input.  Simply muting input
        // made the previous version's wet signal disappear at ordinary
        // feedback settings, which is not a useful dub freeze.
        const int32_t writeFeedback = freeze_ ? 3900 : delayFeedback_;
        const int16_t writeL = static_cast<int16_t>(DelaySoftLimit(
            ((drivenL * inputSend) + (delayedR * writeFeedback)) >> 12));
        const int16_t writeR = static_cast<int16_t>(DelaySoftLimit(
            ((drivenR * inputSend) + (delayedL * writeFeedback)) >> 12));
        const uint32_t advances = AdvanceTape();
        // At normal speed this writes once.  Below normal it occasionally
        // holds a tape position; above normal it duplicates a sample into
        // consecutive positions.  That is a cheap, intentional tape-style
        // pitch bend with no floating point or resampling buffer.
        for (uint32_t i = 0; i < advances; ++i) {
            delayLeft[write_] = writeL;
            delayRight[write_] = writeR;
            write_ = (write_ + 1) & kDelayMask;
        }

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
    PagePickup pickup_;
    uint32_t transportQ16_ = 65536;
    uint32_t transportRemainder_ = 0;
    uint16_t wowPhase_ = 0;
    int32_t wobbleDepth_ = 0;
    bool tapTimeActive_ = false;
    int32_t tapTimeKnob_ = 0;
    uint32_t ledPhase_ = 0;

    void UpdateControls(uint8_t mode, int32_t x, int32_t y, bool pressed, bool tapePage)
    {
        // Count in samples so a pair of Z presses in delay mode becomes a
        // reliable, tempo-like tap time without timers or floating point.
        if (samplesSinceTap_ < kDelaySize - 1) ++samplesSinceTap_;
        const bool newPress = pressed && !wasPressed_;
        wasPressed_ = pressed;

        freeze_ = false;
        shimmer_ = false;
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

        switch (mode) {
        case 0:
            drive_ = x;
            // Bib's send taper leaves useful headroom at ordinary settings;
            // the final part of the turn is reserved for deliberate overload.
            delaySend_ = (y * y) >> 12;
            if (newPress) wavefold_ = !wavefold_;
            break;
        case 1:
            // 4.3 ms to 341 ms: long enough for slap, echo and short loops.
            // A tapped time stays active until X is deliberately moved.
            if (!tapTimeActive_ || Abs(x - tapTimeKnob_) > 512) {
                tapTimeActive_ = false;
                delaySamples_ = 208 + static_cast<uint32_t>((x * (kDelaySize - 209)) >> 12);
            }
            // Leave enough feedback for long repeats, but below the hard
            // clipping loop this compact delay otherwise reaches at maximum.
            delayFeedback_ = (y * 3400) >> 12;
            if (newPress) {
                if (tapCounter_ != 0 && samplesSinceTap_ > 240) {
                    delaySamples_ = samplesSinceTap_;
                    tapTimeActive_ = true;
                    tapTimeKnob_ = x;
                }
                samplesSinceTap_ = 0;
                tapCounter_ = 1;
            }
            break;
        case 2:
            reverbSend_ = x;
            reverbFeedback_ = 1400 + ((y * 1800) >> 12);
            shimmer_ = pressed;
            break;
        default:
            mix_ = x;
            outputLevel_ = 1024 + ((y * 3071) >> 12);
            freeze_ = pressed;
            break;
        }
    }

    uint32_t AdvanceTape()
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

        transportRemainder_ += static_cast<uint32_t>(speed);
        const uint32_t advances = transportRemainder_ >> 16;
        transportRemainder_ &= 0xffff;
        return advances;
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

    int32_t Reverb(int32_t input, int32_t feedback, bool shimmer)
    {
        const int32_t a = combA[combAPos_];
        const int32_t b = combB[combBPos_];
        // Shimmer here is deliberately a bright feedback lift, not a faux
        // pitch shifter.  It is stable, light on CPU, and clearly labelled in
        // the documentation as a colour rather than an octave effect.
        // A small cross term gives brightness without adding enough gain to
        // make the two combs self-oscillate when Z is held for shimmer.
        const int32_t colour = shimmer ? ((a - b) >> 5) : 0;
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

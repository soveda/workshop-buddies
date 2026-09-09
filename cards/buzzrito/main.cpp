#include "pico/stdlib.h"
#include "hardware/clocks.h"

#include <cstdlib>
#include <cstring>

#include "ComputerCard.h"
#include "common.h"
#include "tanh_table.h"
#include "sintab.h"
#include "buzzrito_dsp.h"

class WorkshopBuzzrito : public ComputerCard
{
public:
    WorkshopBuzzrito()
    {
        std::memcpy(bp.presets, default_presets, sizeof(default_presets));
    }

    void __not_in_flash_func(ProcessSample)() override
    {
        update_controls();
        render_stable_swarm();
        AudioOut1(frame_[0] >> 4);
        AudioOut2(frame_[1] >> 4);
    }

private:
    static constexpr int kNumSaws = 16;
    static constexpr bool kInvertXKnob = false;
    static constexpr bool kInvertYKnob = false;
    static constexpr int32_t kSwitchHoldSamples = SAMPLE_FREQ / 4;
    int16_t frame_[2] = {0};
    uint32_t saw_phase_[kNumSaws] = {0};
    uint32_t sub_phase_ = 0;
    uint32_t saw_delta_[kNumSaws] = {0};
    uint32_t sub_delta_ = 0;
    int32_t saw_level_ = 2048;
    int32_t sub_level_ = 1024;
    int32_t gate_q16_ = 65535;
    int32_t pitch_mv_ = 1000;
    int32_t gate_level_ = 65535;
    int32_t led_counter_ = 0;
    int32_t pad_x_smooth_ = 0;
    int32_t pad_y_smooth_ = 0;
    int32_t switch_down_samples_ = 0;
    int chord_mode_ = 1;

    void __not_in_flash_func(update_controls)()
    {
        const int32_t main = KnobVal(Knob::Main);
        const int32_t x = KnobVal(Knob::X);
        const int32_t y = KnobVal(Knob::Y);

        // Original Buzzrito drones at 1000 mV when no pitch jack is patched.
        // Main now acts as a narrower tune offset around a lower test center.
        int32_t pitch_mv = -500 + ((main * 3000) >> 12);
        if (Connected(Input::Audio1))
        {
            // Audio/CV In 1 is signed 12-bit and serves as Buzzrito pitch CV.
            pitch_mv += AudioIn1();
        }
        pitch_mv_ += make_lpf_delta(pitch_mv, pitch_mv_, 6);

        // X/Y knobs are raw pot readings; translate them into the original
        // Buzzrito pad coordinate space before applying CV pad modulation.
        int32_t pad_x = knob_to_pad(x, kInvertXKnob);
        int32_t pad_y = knob_to_pad(y, kInvertYKnob);
        if (Connected(Input::CV1))
        {
            pad_x += CVIn1() * 2;
        }
        if (Connected(Input::CV2))
        {
            pad_y += CVIn2() * 2;
        }
        pad_x = clampi(pad_x, -4096, 4095);
        pad_y = clampi(pad_y, -4096, 4095);
        pad_x_smooth_ += make_lpf_delta(pad_x, pad_x_smooth_, 4);
        pad_y_smooth_ += make_lpf_delta(pad_y, pad_y_smooth_, 4);

        update_switch();

        const bool pulse1_connected = Connected(Input::Pulse1);
        const bool switch_held = switch_down_samples_ >= kSwitchHoldSamples;
        int32_t gate_q16 = 65535;
        if (switch_held)
        {
            gate_q16 = pulse1_connected ? 65535 : 0;
            gate_level_ = gate_q16;
        }
        else if (pulse1_connected)
        {
            const int32_t target = PulseIn1() ? 65535 : 0;
            gate_level_ += make_lpf_delta(target, gate_level_, 7);
            gate_q16 = gate_level_;
        }
        else
        {
            gate_level_ = 65535;
        }
        gate_q16_ = gate_q16;

        buzzypreset preset = buzzy_xyinterpolate(pad_x_smooth_, pad_y_smooth_);
        preset.boc_amount = 0;
        preset.wobble_amount = 0;
        preset.wobble_speed = 0;
        saw_level_ += make_lpf_delta((preset.saw_level * preset.saw_level) >> 12, saw_level_, 5);
        sub_level_ += make_lpf_delta((preset.sub_level * preset.sub_level) >> 12, sub_level_, 5);
        update_pitch_deltas(pitch_mv_, preset.spread);

        PulseOut1(gate_q16 > 32768);
        CVOut1(clamp12(pitch_mv_ / 3));

        update_leds(pad_x_smooth_, pad_y_smooth_, gate_q16);
    }

    void __not_in_flash_func(update_switch)()
    {
        if (SwitchVal() == Switch::Down)
        {
            if (switch_down_samples_ < SAMPLE_FREQ)
            {
                switch_down_samples_++;
            }
            return;
        }

        if (switch_down_samples_ > 0 && switch_down_samples_ < kSwitchHoldSamples)
        {
            chord_mode_++;
            if (chord_mode_ > 4)
            {
                chord_mode_ = 1;
            }
        }
        switch_down_samples_ = 0;
    }

    void __not_in_flash_func(update_pitch_deltas)(int32_t pitch_mv, int32_t spread)
    {
        const static int middle_c_offset_q19 = (int)(23.4806373824f * (1 << 19));
        const int32_t base_log_q19 = pitch_to_log_q19(pitch_mv) + middle_c_offset_q19;
        sub_delta_ = exp2_table(base_log_q19 - (1 << 19));

        for (int i = 0; i < kNumSaws; ++i)
        {
            const int32_t chord_pitch_mv = pitch_mv + chord_offset_mv(i);
            const int32_t chord_log_q19 = middle_c_offset_q19 + pitch_to_log_q19(chord_pitch_mv);
            const int32_t detune = (i - (kNumSaws / 2)) * spread;
            uint32_t target = exp2_table(chord_log_q19 + detune);
            saw_delta_[i] += static_cast<int32_t>(target - saw_delta_[i]) >> 5;
        }
    }

    static int32_t pitch_to_log_q19(int32_t pitch_mv)
    {
        return (pitch_mv << 17) / 250;
    }

    int32_t chord_offset_mv(int32_t saw_index) const
    {
        static constexpr int32_t kOffsets[4][4] = {
            {0, 0, 0, 0},
            {0, 700, 0, 700},
            {0, 700, 1200, 700},
            {0, 400, 700, 1200},
        };
        const int32_t mode = clampi(chord_mode_, 1, 4);
        return kOffsets[mode - 1][saw_index & 3];
    }

    void __not_in_flash_func(render_stable_swarm)()
    {
        int32_t l_samp = 0;
        int32_t r_samp = 0;

        for (int i = 0; i < kNumSaws; ++i)
        {
            saw_phase_[i] += saw_delta_[i];
            const int32_t saw = static_cast<int32_t>(saw_phase_[i] >> 16) - 32768;
            if (i & 1)
            {
                r_samp += saw;
            }
            else
            {
                l_samp += saw;
            }
        }

        l_samp = ((l_samp >> 3) * saw_level_) >> 12;
        r_samp = ((r_samp >> 3) * saw_level_) >> 12;

        sub_phase_ += sub_delta_;
        int32_t sub = static_cast<int32_t>(sub_phase_ >> 16);
        sub = (sub < 32768) ? sub : 65535 - sub;
        sub = (sub - 16384) << 1;
        sub = (sub * sub_level_) >> 12;
        l_samp += sub;
        r_samp += sub;

        const int32_t gate_mul = static_cast<int32_t>((static_cast<int64_t>(gate_q16_) * gate_q16_) >> 16);
        l_samp = static_cast<int32_t>((static_cast<int64_t>(l_samp) * gate_mul) >> 16);
        r_samp = static_cast<int32_t>((static_cast<int64_t>(r_samp) * gate_mul) >> 16);

        frame_[0] = clamp16(l_samp);
        frame_[1] = clamp16(r_samp);
    }

    static int16_t clamp12(int32_t value)
    {
        if (value > 2047) return 2047;
        if (value < -2048) return -2048;
        return static_cast<int16_t>(value);
    }

    static int32_t knob_to_pad(int32_t knob, bool invert)
    {
        int32_t pad = ((knob - 2048) * 4096) >> 11;
        return invert ? -pad : pad;
    }

    static int16_t clamp16(int32_t value)
    {
        if (value > 32767) return 32767;
        if (value < -32768) return -32768;
        return static_cast<int16_t>(value);
    }

    void update_leds(int32_t pad_x, int32_t pad_y, int32_t gate_q16)
    {
        led_counter_++;
        if (led_counter_ < 480) return;
        led_counter_ = 0;

        const int32_t x_amt = abs(pad_x);
        const int32_t y_amt = abs(pad_y);
        LedBrightness(0, clampi(4095 - x_amt, 0, 4095));
        LedBrightness(1, clampi(x_amt, 0, 4095));
        LedBrightness(2, clampi(4095 - y_amt, 0, 4095));
        LedBrightness(3, clampi(y_amt, 0, 4095));
        LedBrightness(4, gate_q16 >> 4);
        LedBrightness(5, chord_mode_ * 1024);
    }
};

int main()
{
    set_sys_clock_khz(192000, true);

    WorkshopBuzzrito card;
    card.EnableNormalisationProbe();
    card.Run();
}

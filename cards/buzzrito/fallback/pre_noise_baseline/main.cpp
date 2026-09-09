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
        for (int i = 0; i < kNumSaws; ++i)
        {
            wobble_phase_[i] = static_cast<uint32_t>(i) * (UINT32_MAX / kNumSaws);
        }
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
    static constexpr bool kInvertYKnob = true;
    static constexpr int32_t kSwitchHoldSamples = SAMPLE_FREQ / 4;
    int16_t frame_[2] = {0};
    uint32_t saw_phase_[kNumSaws] = {0};
    uint32_t sub_phase_ = 0;
    uint32_t saw_delta_[kNumSaws] = {0};
    uint32_t sub_delta_ = 0;
    int32_t saw_level_ = 2048;
    int32_t sub_level_ = 1024;
    int32_t comb_depth_ = 0;
    int32_t comb_mul_ = 7 * 256;
    int32_t delay_time_q8_ = 16 * 256;
    int32_t delay_l_ = 0;
    int32_t delay_r_ = 0;
    int32_t l_dc_ = 0;
    int32_t r_dc_ = 0;
    int32_t wobble_depth_q19_ = 0;
    int32_t wobble_speed_ = 0;
    uint32_t wobble_phase_[kNumSaws] = {0};
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
        // Put that original base note near noon so Main has range below/above.
        int32_t pitch_mv = -1500 + ((main * 5000) >> 12);
        if (Connected(Input::Audio1))
        {
            // Audio/CV In 1 is signed 12-bit and serves as Buzzrito pitch CV.
            pitch_mv += AudioIn1();
        }
        pitch_mv_ += make_lpf_delta(pitch_mv, pitch_mv_, 6);

        // X/Y knobs are raw pot readings; translate them into the original
        // Buzzrito pad coordinate space before applying CV pad modulation.
        int32_t raw_x = knob_to_pad(x, kInvertXKnob);
        int32_t raw_y = knob_to_pad(y, kInvertYKnob);
        if (Connected(Input::CV1))
        {
            raw_x += CVIn1() * 2;
        }
        if (Connected(Input::CV2))
        {
            raw_y += CVIn2() * 2;
        }
        raw_x = clampi(raw_x, -4096, 4095);
        raw_y = clampi(raw_y, -4096, 4095);

        // The original capacitive pad has a tapered horizontal range towards
        // its top and bottom edges. Warp the two independent controls into
        // that shape so either knob continues to affect the morph at its ends.
        const int32_t pad_y = raw_y;
        const int32_t pad_x = (raw_x * (8192 - abs(raw_y))) >> 13;
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
        // First wobble increment: a tiny deterministic per-saw detune. It
        // uses the source preset map but avoids shared pitch drift and random
        // XY motion, which were too active with fixed knob positions.
        const int32_t wobble_depth_target = (preset.wobble_amount * 3) >> 4;
        wobble_depth_q19_ += make_lpf_delta(wobble_depth_target, wobble_depth_q19_, 10);
        wobble_speed_ += make_lpf_delta(preset.wobble_speed, wobble_speed_, 10);
        const int32_t saw_target = (preset.saw_level * preset.saw_level) >> 12;
        const int32_t raw_sub_target = (preset.sub_level * preset.sub_level) >> 12;
        // Edge presets on the original pad can intentionally silence the saw.
        // With pots that makes a broad, easy-to-hit sub-only area, so retain
        // enough saw presence for X and Y morphing to remain audible.
        const int32_t sub_target = mini(raw_sub_target, maxi(1024, saw_target + 1024));
        saw_level_ += make_lpf_delta(saw_target, saw_level_, 5);
        sub_level_ += make_lpf_delta(sub_target, sub_level_, 5);
        update_comb(preset.comb_depth, preset.comb_mul, pitch_mv_);
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
            // A slow, phase-offset sine creates subtle movement between saws
            // without moving the entire instrument's pitch.
            const uint32_t base_increment = 2048 + static_cast<uint32_t>(wobble_speed_) * 12;
            const uint32_t increment = (base_increment * (12 + i)) >> 4;
            wobble_phase_[i] += increment;
            const int32_t wobble = (sin_table[wobble_phase_[i] >> 24] * wobble_depth_q19_) >> 15;
            uint32_t target = exp2_table(chord_log_q19 + detune + wobble);
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

    void update_comb(int32_t source_depth, int32_t source_mul, int32_t pitch_mv)
    {
        const bool negative = source_depth < 0;
        int32_t target_depth = 4096 - abs(source_depth);
        target_depth = (target_depth * target_depth) >> 12;
        target_depth = 4096 - target_depth;
        if (negative) target_depth = -target_depth;

        comb_depth_ += make_lpf_delta(target_depth, comb_depth_, 10);
        comb_mul_ += make_lpf_delta(source_mul, comb_mul_, 10);

        static constexpr int32_t kMiddleCOffsetQ19 = static_cast<int32_t>(23.4806373824f * (1 << 19));
        const int32_t pitch_log_q19 = kMiddleCOffsetQ19 + pitch_to_log_q19(pitch_mv);
        const int32_t comb_shift = comb_mul_ * ((1 << 11) / 12);
        int32_t target_delay_q8 = exp2_table((40 << 19) - pitch_log_q19 - comb_shift);
        while (target_delay_q8 > 2046 * 256) target_delay_q8 >>= 1;
        delay_time_q8_ += make_lpf_delta(target_delay_q8, delay_time_q8_, 10);
    }

    void __not_in_flash_func(render_stable_swarm)()
    {
        int32_t l_samp = 0;
        int32_t r_samp = 0;

        for (int i = 0; i < kNumSaws; ++i)
        {
            saw_phase_[i] += saw_delta_[i];
            const int32_t saw = (static_cast<int32_t>(saw_phase_[i] >> 16) - 32768) << 2;
            if (i & 1)
            {
                r_samp += saw;
            }
            else
            {
                l_samp += saw;
            }
        }

        l_samp = ((l_samp >> 3) * saw_level_) >> 14;
        r_samp = ((r_samp >> 3) * saw_level_) >> 14;

        sub_phase_ += sub_delta_;
        int32_t sub = static_cast<int32_t>(sub_phase_ >> 16);
        sub = (sub < 32768) ? sub : 65535 - sub;
        sub = (sub - 16384) << 3;
        sub = (sub * sub_level_) >> 14;
        l_samp += sub;
        r_samp += sub;

        // The original Buzzrito's character is dominated by this tuned,
        // signed-feedback comb. It is retained here without its motion/noise
        // generators so pad regions remain distinct but stationary.
        l_dc_ += make_lpf_delta(l_samp << 8, l_dc_, 8);
        r_dc_ += make_lpf_delta(r_samp << 8, r_dc_, 8);
        l_samp -= l_dc_ >> 8;
        r_samp -= r_dc_ >> 8;

        int32_t read_pos = delay_pos - (delay_time_q8_ >> 8);
        const int32_t delay_l0 = delay_buf_l[read_pos & 2047];
        const int32_t delay_r0 = delay_buf_r[read_pos & 2047];
        read_pos--;
        const int32_t delay_l1 = delay_buf_l[read_pos & 2047];
        const int32_t delay_r1 = delay_buf_r[read_pos & 2047];
        const int32_t fraction = delay_time_q8_ & 255;
        const int32_t delay_l_raw = delay_l0 + (((delay_l1 - delay_l0) * fraction) >> 8);
        const int32_t delay_r_raw = delay_r0 + (((delay_r1 - delay_r0) * fraction) >> 8);
        delay_l_ += make_lpf_delta(delay_l_raw, delay_l_, 1);
        delay_r_ += make_lpf_delta(delay_r_raw, delay_r_, 1);
        l_samp += (delay_l_ * comb_depth_) >> 12;
        r_samp += (delay_r_ * comb_depth_) >> 12;

        l_samp = soft_clip(l_samp);
        r_samp = soft_clip(r_samp);
        delay_buf_l[delay_pos] = static_cast<int16_t>(l_samp);
        delay_buf_r[delay_pos] = static_cast<int16_t>(r_samp);
        delay_pos = (delay_pos + 1) & 2047;

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

        // The first four LEDs are the physical pad corners: top-left,
        // top-right, bottom-left, bottom-right. The physical Y orientation
        // is opposite the virtual sound coordinate, while X is shared.
        const int32_t top_left = clampi((-pad_x - pad_y) >> 1, 0, 4095);
        const int32_t top_right = clampi((pad_x - pad_y) >> 1, 0, 4095);
        const int32_t bottom_left = clampi((-pad_x + pad_y) >> 1, 0, 4095);
        const int32_t bottom_right = clampi((pad_x + pad_y) >> 1, 0, 4095);
        LedBrightness(0, top_left);
        LedBrightness(1, top_right);
        LedBrightness(2, bottom_left);
        LedBrightness(3, bottom_right);
        LedBrightness(4, gate_q16 >> 4);
        LedBrightness(5, clampi(chord_mode_ * 1024, 0, 4095));
    }
};

int main()
{
    set_sys_clock_khz(192000, true);

    WorkshopBuzzrito card;
    card.EnableNormalisationProbe();
    card.Run();
}

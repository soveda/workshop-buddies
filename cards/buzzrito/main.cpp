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
        if (block_index_ == 0)
        {
            render_block();
        }

        AudioOut1(block_[block_index_ * 2 + 0] >> 4);
        AudioOut2(block_[block_index_ * 2 + 1] >> 4);

        block_index_ = (block_index_ + 1) & (BLOCK_SIZE - 1);
    }

private:
    int16_t block_[BLOCK_SIZE * 2] = {0};
    int32_t block_index_ = 0;
    int32_t gate_level_ = 65535;
    int32_t led_counter_ = 0;
    int32_t pad_x_smooth_ = 0;
    int32_t pad_y_smooth_ = 0;
    int chord_mode_ = 1;

    void __not_in_flash_func(render_block)()
    {
        const int32_t main = KnobVal(Knob::Main);
        const int32_t x = KnobVal(Knob::X);
        const int32_t y = KnobVal(Knob::Y);

        // Main is an overall tune control. The Buddies oscillator expects a
        // pitch value in millivolts, so this gives about five octaves of sweep.
        int32_t pitch_mv = -2000 + ((main * 5000) >> 12);
        if (Connected(Input::Audio1))
        {
            // Audio/CV In 1 is signed 12-bit and serves as Buzzrito pitch CV.
            pitch_mv += AudioIn1() * 3;
        }

        // X/Y knobs replace the original capacitive XY pad, with CV1/CV2
        // acting as the original Buzzrito X/Y CV inputs.
        int32_t pad_x = ((x - 2048) * 4096) >> 11;
        int32_t pad_y = ((y - 2048) * 4096) >> 11;
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

        int32_t gate_q16 = 65535;
        if (Connected(Input::Pulse1))
        {
            const int32_t target = PulseIn1() ? 65535 : 0;
            gate_level_ += make_lpf_delta(target, gate_level_, 7);
            gate_q16 = gate_level_;
        }
        else
        {
            gate_level_ = 65535;
        }

        buzzypreset preset = buzzy_xyinterpolate(pad_x_smooth_, pad_y_smooth_);
        preset.boc_amount = 0;
        preset.wobble_amount = 0;
        preset.wobble_speed = 0;
        process_buzzrito(block_, pitch_mv, gate_q16, preset.spread, preset.glide,
                         preset.wobble_amount, preset.boc_amount, preset.wobble_speed,
                         preset.saw_level, preset.sub_level, preset.noise_level,
                         preset.comb_depth, preset.comb_mul, chord_mode_);

        PulseOut1(gate_q16 > 32768);
        CVOut1(clamp12(pitch_mv / 3));

        update_leds(pad_x_smooth_, pad_y_smooth_, gate_q16);
    }

    static int16_t clamp12(int32_t value)
    {
        if (value > 2047) return 2047;
        if (value < -2048) return -2048;
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
        LedBrightness(5, 512 + ((pad_x + pad_y + 8192) >> 3));
    }
};

int main()
{
    set_sys_clock_khz(192000, true);

    WorkshopBuzzrito card;
    card.EnableNormalisationProbe();
    card.Run();
}

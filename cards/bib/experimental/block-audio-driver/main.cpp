// Workshop block-audio transport experiment.
//
// This does not use ComputerCard.h. It borrows the Workshop pin map, ADC DNL
// correction, mux cadence, and MCP4822 formatting from ComputerCard v0.3.0
// (MIT, Chris Johnson) while providing a 64-frame block hand-off to core 1.

#include <cstdint>

#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "hardware/sync.h"

namespace
{
constexpr uint32_t kSampleRate = 48000;
constexpr uint8_t kBlockFrames = 64;
constexpr uint8_t kBlockCount = 4;
constexpr uint8_t kBlockMask = kBlockCount - 1;
constexpr uint8_t kOutputDelayBlocks = 2;

static_assert((kBlockCount & kBlockMask) == 0, "block count must be a power of two");

// Workshop Computer hardware map.
constexpr uint kPulseIn1 = 2;
constexpr uint kPulseIn2 = 3;
constexpr uint kLedPins[] = {10, 11, 12, 13, 14, 15};
constexpr uint kDacSck = 18;
constexpr uint kDacTx = 19;
constexpr uint kDacCs = 21;
constexpr uint kMuxA = 24;
constexpr uint kMuxB = 25;
constexpr uint kAudioRight = 26;
constexpr uint kAudioLeft = 27;
constexpr uint kMuxIo1 = 28;
constexpr uint kMuxIo2 = 29;

constexpr uint16_t kDacChannelA = 0x0000;
constexpr uint16_t kDacChannelB = 0x8000;

struct StereoFrame
{
    int16_t left;
    int16_t right;
};

struct StereoBlock
{
    StereoFrame frames[kBlockFrames];
};

// Each slot has one producer and one consumer. Core 0 writes input slots and
// consumes output slots; core 1 does the opposite. The flags are release/acquire
// hand-offs, with no locks or FIFO traffic in the sample ISR.
alignas(4) StereoBlock inputBlocks[kBlockCount] = {};
alignas(4) StereoBlock outputBlocks[kBlockCount] = {};
volatile bool inputReady[kBlockCount] = {};
volatile bool outputReady[kBlockCount] = {};

uint16_t adcBuffers[2][8] = {};
uint16_t dacBuffers[2][2] = {};
uint8_t adcPhase = 0;
uint8_t muxState = 0;
uint8_t captureBlock = 0;
uint8_t captureFrame = 0;
uint8_t activeOutputBlock = 0;
bool activeOutputValid = false;
uint32_t blocksCaptured = 0;
volatile uint32_t inputOverruns = 0;
volatile uint32_t outputUnderruns = 0;
uint8_t adcDma = 0;
uint8_t spiDma = 0;

int32_t Clamp12(int32_t value)
{
    if (value < -2048) return -2048;
    if (value > 2047) return 2047;
    return value;
}

uint16_t DacWord(int32_t value, uint16_t channel)
{
    const uint16_t code = static_cast<uint16_t>((Clamp12(value) & 0x0fff) + 0x0800) & 0x0fff;
    return channel | 0x3000 | code;
}

void CorrectAdcDnl(uint16_t &value)
{
    const uint16_t shifted = value + 512;
    value += ((value & 0x03ff) == 0x01ff) << 2;
    value += (shifted >> 10) << 3;
    value = static_cast<uint32_t>(value * 520349) >> 19;
}

int16_t ToBibRange(int32_t sample12)
{
    return static_cast<int16_t>(Clamp12(sample12) << 4);
}

int32_t FromBibRange(int16_t sample16)
{
    return sample16 >> 4;
}

void SetLed(uint8_t index, bool on)
{
    pwm_set_gpio_level(kLedPins[index], on ? 65535 : 0);
}

void ProcessBlock(const StereoBlock &input, StereoBlock &output)
{
    // First transport gate: intentionally simple. Replacing this loop with
    // original Bib's process_bib() is a later phase, after I/O stability has
    // been demonstrated on hardware.
    for (uint8_t frame = 0; frame < kBlockFrames; ++frame)
    {
        output.frames[frame] = input.frames[frame];
    }
}

void __not_in_flash_func(BlockWorker)()
{
    uint8_t readBlock = 0;
    while (true)
    {
        if (!inputReady[readBlock] || outputReady[readBlock])
        {
            tight_loop_contents();
            continue;
        }

        // Acquire a fully written input slot before reading it.
        __dmb();
        ProcessBlock(inputBlocks[readBlock], outputBlocks[readBlock]);
        // Publish output before allowing core 0 to reuse the input slot.
        __dmb();
        outputReady[readBlock] = true;
        __dmb();
        inputReady[readBlock] = false;
        readBlock = (readBlock + 1) & kBlockMask;
    }
}

void __isr __not_in_flash_func(AudioDmaComplete)()
{
    const uint8_t completedPhase = adcPhase;
    adcPhase = 1 - adcPhase;

    // Restart ADC capture and DAC transmission before doing any conversion.
    dma_hw->ints0 = 1u << adcDma;
    dma_channel_set_write_addr(adcDma, adcBuffers[adcPhase], true);
    dma_channel_set_read_addr(spiDma, dacBuffers[adcPhase], true);

    uint16_t right0 = adcBuffers[completedPhase][0];
    uint16_t left0 = adcBuffers[completedPhase][1];
    uint16_t right1 = adcBuffers[completedPhase][4];
    uint16_t left1 = adcBuffers[completedPhase][5];
    CorrectAdcDnl(right0);
    CorrectAdcDnl(left0);
    CorrectAdcDnl(right1);
    CorrectAdcDnl(left1);

    // Audio input op-amps are inverting. Match ComputerCard's two-conversion
    // average, then convert at the boundary to Bib's native 16-bit range.
    const int32_t inputRight = -((static_cast<int32_t>(right0) + right1 - 0x1000) >> 1);
    const int32_t inputLeft = -((static_cast<int32_t>(left0) + left1 - 0x1000) >> 1);
    inputBlocks[captureBlock].frames[captureFrame] = {ToBibRange(inputLeft), ToBibRange(inputRight)};

    if (captureFrame == 0)
    {
        activeOutputBlock = (captureBlock + kBlockCount - kOutputDelayBlocks) & kBlockMask;
        activeOutputValid = outputReady[activeOutputBlock];
        if (!activeOutputValid && blocksCaptured >= kOutputDelayBlocks)
        {
            ++outputUnderruns;
            SetLed(1, true);
        }
    }

    StereoFrame output = {};
    if (activeOutputValid)
    {
        output = outputBlocks[activeOutputBlock].frames[captureFrame];
    }
    // DAC output hardware is also inverting.
    dacBuffers[completedPhase][0] = DacWord(-FromBibRange(output.left), kDacChannelA);
    dacBuffers[completedPhase][1] = DacWord(-FromBibRange(output.right), kDacChannelB);

    // Keep the external mux in the proven ComputerCard cadence. This first
    // transport pass does not yet expose control/CV values to a card program.
    muxState = (muxState + 1) & 0x03;
    gpio_put(kMuxA, muxState & 0x01);
    gpio_put(kMuxB, muxState & 0x02);

    ++captureFrame;
    if (captureFrame != kBlockFrames) return;

    captureFrame = 0;
    ++blocksCaptured;
    __dmb();
    inputReady[captureBlock] = true;
    __dmb();

    if (activeOutputValid)
    {
        __dmb();
        outputReady[activeOutputBlock] = false;
    }

    const uint8_t nextCapture = (captureBlock + 1) & kBlockMask;
    if (inputReady[nextCapture])
    {
        ++inputOverruns;
        SetLed(0, true);
    }
    captureBlock = nextCapture;
    SetLed(2, (blocksCaptured & 0x20) != 0);
}

void InitialiseLeds()
{
    for (uint8_t index = 0; index < 6; index += 2)
    {
        gpio_set_function(kLedPins[index], GPIO_FUNC_PWM);
        gpio_set_function(kLedPins[index + 1], GPIO_FUNC_PWM);
        pwm_config config = pwm_get_default_config();
        pwm_config_set_wrap(&config, 65535);
        pwm_init(pwm_gpio_to_slice_num(kLedPins[index]), &config, true);
        pwm_init(pwm_gpio_to_slice_num(kLedPins[index + 1]), &config, true);
        SetLed(index, false);
        SetLed(index + 1, false);
    }
}

void InitialiseHardware()
{
    InitialiseLeds();

    gpio_init(kMuxA);
    gpio_init(kMuxB);
    gpio_set_dir(kMuxA, GPIO_OUT);
    gpio_set_dir(kMuxB, GPIO_OUT);
    gpio_put(kMuxA, false);
    gpio_put(kMuxB, false);

    gpio_init(kPulseIn1);
    gpio_init(kPulseIn2);
    gpio_set_dir(kPulseIn1, GPIO_IN);
    gpio_set_dir(kPulseIn2, GPIO_IN);
    gpio_pull_up(kPulseIn1);
    gpio_pull_up(kPulseIn2);

    adc_init();
    adc_gpio_init(kAudioRight);
    adc_gpio_init(kAudioLeft);
    adc_gpio_init(kMuxIo1);
    adc_gpio_init(kMuxIo2);
    adc_select_input(0);
    adc_set_round_robin(0x0f);
    adc_fifo_setup(true, true, 1, false, false);
    // 48 MHz / 125 = 384 kHz: two samples from each ADC input per frame.
    adc_set_clkdiv(124);

    spi_init(spi0, 15625000);
    spi_set_format(spi0, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(kDacSck, GPIO_FUNC_SPI);
    gpio_set_function(kDacTx, GPIO_FUNC_SPI);
    gpio_set_function(kDacCs, GPIO_FUNC_SPI);

    adcDma = dma_claim_unused_channel(true);
    spiDma = dma_claim_unused_channel(true);

    dma_channel_config adcConfig = dma_channel_get_default_config(adcDma);
    channel_config_set_transfer_data_size(&adcConfig, DMA_SIZE_16);
    channel_config_set_read_increment(&adcConfig, false);
    channel_config_set_write_increment(&adcConfig, true);
    channel_config_set_dreq(&adcConfig, DREQ_ADC);
    dma_channel_configure(adcDma, &adcConfig, adcBuffers[adcPhase], &adc_hw->fifo, 8, false);

    dma_channel_config spiConfig = dma_channel_get_default_config(spiDma);
    channel_config_set_transfer_data_size(&spiConfig, DMA_SIZE_16);
    channel_config_set_dreq(&spiConfig, DREQ_SPI0_TX);
    dma_channel_configure(spiDma, &spiConfig, &spi_get_hw(spi0)->dr, nullptr, 2, false);

    dma_channel_set_irq0_enabled(adcDma, true);
    irq_set_exclusive_handler(DMA_IRQ_0, AudioDmaComplete);
    irq_set_enabled(DMA_IRQ_0, true);
    adc_run(true);
    dma_channel_set_write_addr(adcDma, adcBuffers[adcPhase], true);
}
} // namespace

int main()
{
    // Keep the known-good Bib card clock. The block runtime can later be
    // measured at 192 and 240 MHz without changing its scheduling design.
    set_sys_clock_khz(192000, true);
    InitialiseHardware();
    multicore_launch_core1(BlockWorker);

    while (true)
    {
        tight_loop_contents();
    }
}

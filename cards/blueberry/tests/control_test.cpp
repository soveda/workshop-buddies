#include <cassert>
#include <cstdint>
#include <cstdio>

// Replace hardware I/O only; execute the actual card's control/recording code.
#define COMPUTERCARD_H
#define __not_in_flash_func(name) name
class ComputerCard
{
public:
    enum class Input { CV1, Pulse1, Pulse2 };
    enum class Knob { Main };
    enum class Switch { Middle, Down, Up };
    bool connected[3] = {true, false, false};
    bool gate = false, edge = false, gateOut = false, loopOut = false;
    int32_t pitch = 1000, pitchOut = 0, mainKnob = 2048;
    Switch mode = Switch::Middle;
    virtual void ProcessSample() {}
    void EnableNormalisationProbe() {}
    bool Connected(Input i) { return connected[static_cast<int>(i)]; }
    bool PulseIn1RisingEdge() { return edge; }
    bool PulseIn2() { return gate; }
    int32_t CVIn1Millivolts() { return pitch; }
    int32_t KnobVal(Knob) { return mainKnob; }
    Switch SwitchVal() { return mode; }
    void AudioOut1(int) {}
    void AudioOut2(int) {}
    void CVOut1Millivolts(int32_t mv) { pitchOut = mv; }
    void CVOut2(int) {}
    void PulseOut1(bool high) { gateOut = high; }
    void PulseOut2(bool high) { loopOut = high; }
    bool InputsCalibrated() { return true; }
    void LedOn(int, bool) {}
    void LedBrightness(int, uint16_t) {}
    void Run() {}
};

#define main firmware_main
#include "../main.cpp"
#undef main

void Tick(WorkshopBlueberry &card, int shortGateSample = -1, int clockSample = -1)
{
    const bool gate = card.gate;
    for (int sample = 0; sample < 48; ++sample)
    {
        card.gate = gate || sample == shortGateSample;
        card.edge = sample == clockSample;
        card.ProcessSample();
    }
    card.gate = gate;
    card.edge = false;
}

int main()
{
    WorkshopBlueberry card;
    Tick(card);
    assert(card.pitchOut == 1000 && !card.gateOut);
    card.pitch = 2000;
    Tick(card);
    assert(card.pitchOut == 2000 && !card.gateOut);
    card.connected[2] = true;
    Tick(card, 7);
    assert(card.gateOut); // Short trigger away from the control-tick sample.
    Tick(card);
    assert(!card.gateOut);
    card.gate = true;
    Tick(card);
    assert(card.gateOut);
    card.connected[2] = false;
    Tick(card);
    assert(!card.gateOut);

    card.pitch = 1000;
    card.connected[2] = true;
    card.mode = ComputerCard::Switch::Down;
    Tick(card);
    card.gate = false;
    Tick(card);
    card.mode = ComputerCard::Switch::Middle;
    Tick(card);
    card.connected[1] = true;
    card.connected[2] = false;
    card.pitch = 3000;
    card.mode = ComputerCard::Switch::Up;
    Tick(card);
    assert(card.pitchOut == 1000 && card.gateOut); // Recorded state, not live.
    Tick(card, -1, 7);
    assert(!card.gateOut); // Captured clock edge advanced to gate-off event.
    Tick(card);
    assert(!card.gateOut);
    Tick(card, -1, 2);
    assert(card.gateOut && card.loopOut);
    card.mode = ComputerCard::Switch::Middle;
    Tick(card);
    assert(card.pitchOut == 3000 && !card.gateOut);

    card.pitch = 1000;
    card.mainKnob = 3500;
    Tick(card);
    assert(card.pitchOut == 1583);
    Tick(card);
    assert(card.pitchOut == 1583);
    card.mainKnob = 2048;
    Tick(card);
    card.mainKnob = 3500;
    Tick(card);
    assert(card.pitchOut == 2000);
    std::puts("Passed: independent CV/gate, short triggers, recorded gates, clock edges, live return and 1 V/oct transposition.");
}

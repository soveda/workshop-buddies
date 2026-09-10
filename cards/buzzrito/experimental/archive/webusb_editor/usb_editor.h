#pragma once

#include <stdint.h>

enum class UsbEditorCommandType : uint8_t
{
    SetXY,
    SetPreset,
    SetParameter,
};

struct UsbEditorCommand
{
    UsbEditorCommandType type;
    uint8_t index[4];
    int16_t values[10];
};

bool usb_editor_pop(UsbEditorCommand &command);
void usb_editor_service();

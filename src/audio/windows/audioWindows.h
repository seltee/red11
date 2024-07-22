// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "audio/audio.h"

#ifdef WINDOWS_ONLY
#include "audioWindowsHelper.h"
#include <windows.h>
#include <dsound.h>

class AudioWindows : public Audio
{
public:
    EXPORT AudioWindows();

    EXPORT std::vector<AudioDeviceWindows> getWindowsDevicesList();
    EXPORT std::vector<AudioDevice> getDevicesList() override final;

    EXPORT bool setup() override final;
    EXPORT bool setup(std::string deviceName) override final;
    virtual void fillBuffers() override final;

protected:
    bool setupPinPongBuffers();
    void fillBuffer(unsigned int bufferPosition);

    LPDIRECTSOUND8 lpds = nullptr;

    // Accumulates sound
    unsigned char *pDataBuffer = nullptr;
    // Receives single sound data
    unsigned char *pShiftBuffer = nullptr;

    // Create secondary buffers
    IDirectSoundBuffer *pDXBuffer = nullptr;

    bool bFirstPartPlaying = true;
    bool bUsePcm = false;
};
#endif
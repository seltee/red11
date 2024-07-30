// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "utils/utils.h"
#include "utils/algorithms.h"
#include "utils/primitives.h"
#include "data/usable.h"
#include <string>
#include <vector>

class Sound : public Usable
{
public:
    Sound();
    virtual ~Sound();

    virtual bool isStreamable() = 0;

    virtual unsigned char *getData() = 0;
    virtual int getDataSize() = 0;
    virtual unsigned int getSampleRate() = 0;
    virtual bool isReady() = 0;

    virtual bool setupStream(SoundStream *stream);
    virtual int processStream(SoundStream *stream, unsigned char *buffer, int bufferSize);
    virtual void closeStream(SoundStream *stream);

    unsigned int getChannelsNumber();
    unsigned int getSampleSize();

    inline AudioFormat getAudioFormat() { return format; }
    inline unsigned int getIndex() { return index; }

    void destroy();

    void unload() override;

    static inline std::vector<Sound *> *getSoundsList() { return &sounds; }

protected:
    unsigned int index;
    AudioFormat format = AudioFormat::UNKNOWN;

    static std::vector<Sound *> sounds;
};
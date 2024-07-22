// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "sound.h"

Sound::Sound()
{
    static unsigned int lastIndex = 0;
    index = lastIndex;
}

bool Sound::setupStream(SoundStream *stream)
{
    return false;
}

int Sound::processStream(SoundStream *stream, unsigned char *buffer, int bufferSize)
{
    return 0;
}

void Sound::closeStream(SoundStream *stream)
{
}

unsigned int Sound::getChannelsNumber()
{
    if ((int)format >= (int)AudioFormat::STEREO_8)
        return 2;
    return 1;
}

unsigned int Sound::getSampleSize()
{
    switch (format)
    {
    case AudioFormat::UNKNOWN:
    case AudioFormat::MONO_8:
    case AudioFormat::STEREO_8:
        return 1;
    case AudioFormat::MONO_16:
    case AudioFormat::STEREO_16:
        return 2;
    case AudioFormat::MONO_32:
    case AudioFormat::MONO_32F:
    case AudioFormat::STEREO_32:
    case AudioFormat::STEREO_32F:
        return 4;
    }
}
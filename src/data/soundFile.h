// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "sound.h"

class SoundFile : public Sound
{
public:
    EXPORT SoundFile(std::string path, bool bForceMono = false);

    inline bool isPath(std::string path) { return this->path == path; }
    inline bool isLoaded() { return bIsLoaded; }
    EXPORT bool isStreamable() override;
    EXPORT bool isReady() override;

    EXPORT unsigned char *getData() override;
    EXPORT int getDataSize() override;
    EXPORT unsigned int getSampleRate() override;

    EXPORT bool setupStream(SoundStream *stream) override;
    EXPORT int processStream(SoundStream *stream, unsigned char *buffer, int bufferSize) override;
    EXPORT void closeStream(SoundStream *stream) override;

    EXPORT void load() override;

protected:
    bool loadWAV();
    bool loadOGG();

    std::string path;
    bool bIsLoaded = false, bIsStreamable = false;
    bool bForceMono = false;

    unsigned char *data;
    unsigned int dataSize;
    unsigned int sampleRate;

    Extension extension = Extension::UNKNOWN;
};
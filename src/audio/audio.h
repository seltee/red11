// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "data/entity.h"
#include "data/sound.h"
#include "audioHelper.h"
#include "audioSource.h"
#include <string>
#include <vector>

class Audio
{
public:
    EXPORT void process(float delta);

    virtual std::vector<AudioDevice> getDevicesList() = 0;
    virtual bool setup() = 0;
    virtual bool setup(std::string deviceName) = 0;
    virtual void fillBuffers() = 0;

    EXPORT AudioSource *createAudioSource();

    EXPORT void playSound(Sound *sound, float volume = 0.8f);
    EXPORT void playSound3d(Sound *sound, Vector3 position, float volume = 0.8f, float maxDistance = 10.0f, float refDistance = 1.0f);

    EXPORT void syncPosition(Entity *entity);

protected:
    Vector3 vPosition = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 vDirection = Vector3(0.0f, 0.0f, -1.0f);

    std::vector<AudioSource *> sources;

    int freq = 0;
    int channels = 0;
    AudioFormat format = AudioFormat::STEREO_32F;
    int bufferSize = 0;

    bool bIsReady = false;
};